//
// Created by sam1203 on 10/31/17.
//

#ifndef __PERF_TEST_ONLY___

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "OLI_Logger.h"
#include "OBProcessor.h"

#include <iostream>
#include <map>
#include <atomic>
#include <memory>
#include <vector>

#include "METransportAdaptor.h"
#include "MEOrderBookCache.h"
#include "MatchingEngine.h"

#include "oli_messageslib.pb.h"

#include <sched.h>
#include <unistd.h>

using namespace oli::mengine;
using namespace oli::msg;
using namespace std::chrono;


namespace{
    int stick_this_thread_to_core(int core_id) {
        int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
        if (core_id < 0 || core_id >= num_cores)
            return EINVAL;

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);

        pthread_t current_thread = pthread_self();
        return pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
    }


    typedef std::function<void()> TestFuncT;
    class PerfTestScenario {
    public:
        const size_t BUCKETS_COUNT = 20;
        const size_t SUBBUCKETS_COUNT = 10;

        PerfTestScenario(): buckets_(SUBBUCKETS_COUNT + BUCKETS_COUNT + 1, 0)
        {}
        ~PerfTestScenario()
        {}

        void execute(size_t packageCount, size_t samplesCount, TestFuncT testCase, TestFuncT postProcess) {
            std::vector<double> durSamples(samplesCount, 0);
            for (size_t i = 0; i < samplesCount; ++i) {
                high_resolution_clock::time_point t1 = high_resolution_clock::now();
                for (size_t j = 0; j < packageCount; ++j) {
                    testCase();
                }
                high_resolution_clock::time_point t2 = high_resolution_clock::now();
                duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
                durSamples[i] = time_span.count() * 1000000;

                postProcess();
            }

            std::sort(durSamples.begin(), durSamples.end());
            minDur_ = durSamples[0];
            intervalLen_ = (durSamples[samplesCount - 1] - minDur_) / BUCKETS_COUNT;
            intervalLenSmall_ = (intervalLen_) / SUBBUCKETS_COUNT;
            for (auto d : durSamples) {
                if (d < minDur_ + intervalLen_) {
                    double index = (d - minDur_) / intervalLenSmall_;
                    ++buckets_[(size_t) index];
                }
                double index = (d - minDur_) / intervalLen_;
                ++buckets_[SUBBUCKETS_COUNT + (size_t) index];
            }

        }

        void writeResult() {
            double subminDur = minDur_;
            for (size_t i = 0; i < buckets_.size(); ++i) {
                if (i < SUBBUCKETS_COUNT) {
                    std::cout << "\tInterval [" << subminDur << ", " << subminDur + intervalLenSmall_ << "]: " << buckets_[i]
                              << std::endl;
                    subminDur += intervalLenSmall_;
                } else {
                    std::cout << "Interval [" << minDur_ << ", " << minDur_ + intervalLen_ << "]: " << buckets_[i]
                              << std::endl;
                    minDur_ += intervalLen_;
                }
            }
        }

    private:
        std::vector<size_t> buckets_;
        double minDur_;
        double intervalLenSmall_;
        double intervalLen_;

    };
}



BOOST_AUTO_TEST_SUITE( MatchAdaptorTest )

    BOOST_AUTO_TEST_CASE( MatchMarketOrderOnce_PerfTest )
    {
        oli::idT instrId = 1000;
        oli::priceT price = 9000;

        stick_this_thread_to_core(3);

        MEOrderBookCache obookCache(instrId);
        MatchingEngine mEngine;
        METransportAdaptor testAdapt(&obookCache, &mEngine);
        mEngine.attach(&obookCache);
        mEngine.attach(&testAdapt);

        oli::msg::OBBookChangeMsg obChangeMsg;
        obChangeMsg.set_type(add_OBookChangeType);
        obChangeMsg.set_side(buySide);
        obChangeMsg.set_instrumentid(instrId);
        obChangeMsg.set_orderid(98765);
        obChangeMsg.set_price(price);
        obChangeMsg.set_volume(200000000);
        obChangeMsg.set_prevprice(0);
        obChangeMsg.set_prevvolume(0);
        testAdapt.onMessage(&obChangeMsg);

        oli::idT orderId = 12345;
        const size_t PACKAGE_COUNT = 4; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 4000000 / PACKAGE_COUNT; //20000000
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            oli::msg::OMSNewOrderMsg omsNewOrderMsg;
            omsNewOrderMsg.set_orderid(++orderId);
            omsNewOrderMsg.set_instrumentid(instrId);
            omsNewOrderMsg.set_price(0);
            omsNewOrderMsg.set_side(sellSide);
            omsNewOrderMsg.set_volumeleft(1);
            omsNewOrderMsg.set_ordtype(marketOrderType);

            testAdapt.onMessage(&omsNewOrderMsg);
        }, [](){});

        std::cout << "Latency of Market order processing:" << std::endl;
        perfTestCase.writeResult();

        {
            high_resolution_clock::time_point t1 = high_resolution_clock::now();
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            std::cout << "Timer granularity [" << time_span.count()*1000000<< "] mksec"<< std::endl;
        }

        std::cout << std::endl;
    }

    BOOST_AUTO_TEST_CASE( MatchMarketOrdersMultiple_PerfTest )
    {
        oli::idT instrId = 1000;
        oli::priceT price = 9000;
        oli::priceT priceLvl2 = 8000;

        stick_this_thread_to_core(3);

        MEOrderBookCache obookCache(instrId);
        MatchingEngine mEngine;
        METransportAdaptor testAdapt(&obookCache, &mEngine);
        mEngine.attach(&obookCache);
        mEngine.attach(&testAdapt);

        oli::msg::OBBookChangeMsg obChangeMsg;
        obChangeMsg.set_type(add_OBookChangeType);
        obChangeMsg.set_side(buySide);
        obChangeMsg.set_instrumentid(instrId);
        obChangeMsg.set_orderid(98765);
        obChangeMsg.set_price(price);
        obChangeMsg.set_volume(300);
        obChangeMsg.set_prevprice(0);
        obChangeMsg.set_prevvolume(0);
        testAdapt.onMessage(&obChangeMsg);
        obChangeMsg.set_type(add_OBookChangeType);
        obChangeMsg.set_orderid(98764);
        obChangeMsg.set_volume(300);
        testAdapt.onMessage(&obChangeMsg);
        obChangeMsg.set_type(add_OBookChangeType);
        obChangeMsg.set_orderid(98763);
        obChangeMsg.set_price(priceLvl2);
        obChangeMsg.set_volume(300);
        testAdapt.onMessage(&obChangeMsg);
        obChangeMsg.set_type(add_OBookChangeType);
        obChangeMsg.set_orderid(98762);
        obChangeMsg.set_price(priceLvl2);
        obChangeMsg.set_volume(300);
        testAdapt.onMessage(&obChangeMsg);

        oli::idT orderId = 12345;
        const size_t PACKAGE_COUNT = 4; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 4000000 / PACKAGE_COUNT; //20000000
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT,
                [&]() {
                    oli::msg::OMSNewOrderMsg omsNewOrderMsg;
                    omsNewOrderMsg.set_orderid(++orderId);
                    omsNewOrderMsg.set_instrumentid(instrId);
                    omsNewOrderMsg.set_price(0);
                    omsNewOrderMsg.set_side(sellSide);
                    omsNewOrderMsg.set_volumeleft(1200);
                    omsNewOrderMsg.set_ordtype(marketOrderType);

                    testAdapt.onMessage(&omsNewOrderMsg);
                },
                [&](){
                    /// restore order book
                    obChangeMsg.set_type(change_OBookChangeType);
                    obChangeMsg.set_orderid(98765);
                    obChangeMsg.set_price(price);
                    obChangeMsg.set_volume(300);
                    testAdapt.onMessage(&obChangeMsg);
                    obChangeMsg.set_type(change_OBookChangeType);
                    obChangeMsg.set_orderid(98764);
                    obChangeMsg.set_price(price);
                    obChangeMsg.set_volume(300);
                    testAdapt.onMessage(&obChangeMsg);
                    obChangeMsg.set_type(change_OBookChangeType);
                    obChangeMsg.set_orderid(98763);
                    obChangeMsg.set_price(priceLvl2);
                    obChangeMsg.set_volume(300);
                    testAdapt.onMessage(&obChangeMsg);
                    obChangeMsg.set_type(change_OBookChangeType);
                    obChangeMsg.set_orderid(98762);
                    obChangeMsg.set_price(priceLvl2);
                    obChangeMsg.set_volume(300);
                    testAdapt.onMessage(&obChangeMsg);
                });

        std::cout << "Latency of Market order processing:" << std::endl;
        perfTestCase.writeResult();

        std::cout << std::endl;
    }

    BOOST_AUTO_TEST_CASE( MatchLimitOrders_PerfTest )
    {
        oli::idT instrId = 1000;
        oli::priceT price = 9000;

        stick_this_thread_to_core(3);

        MEOrderBookCache obookCache(instrId);
        MatchingEngine mEngine;
        METransportAdaptor testAdapt(&obookCache, &mEngine);
        mEngine.attach(&obookCache);
        mEngine.attach(&testAdapt);

        oli::msg::OBBookChangeMsg obChangeMsg;
        obChangeMsg.set_type(add_OBookChangeType);
        obChangeMsg.set_side(buySide);
        obChangeMsg.set_instrumentid(instrId);
        obChangeMsg.set_orderid(98765);
        obChangeMsg.set_price(price);
        obChangeMsg.set_volume(200000000);
        obChangeMsg.set_prevprice(0);
        obChangeMsg.set_prevvolume(0);
        testAdapt.onMessage(&obChangeMsg);

        oli::idT orderId = 12345;
        const size_t PACKAGE_COUNT = 4; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 4000000 / PACKAGE_COUNT; //20000000
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT,
                [&]() {
                    oli::msg::OMSNewOrderMsg omsNewOrderMsg;
                    omsNewOrderMsg.set_orderid(++orderId);
                    omsNewOrderMsg.set_instrumentid(instrId);
                    omsNewOrderMsg.set_price(price);
                    omsNewOrderMsg.set_side(sellSide);
                    omsNewOrderMsg.set_volumeleft(1);
                    omsNewOrderMsg.set_ordtype(limitOrderType);

                    testAdapt.onMessage(&omsNewOrderMsg);
                },
                [&](){});

        std::cout << "Latency of Limit order processing:" << std::endl;
        perfTestCase.writeResult();

        std::cout << std::endl;
    }


    BOOST_AUTO_TEST_CASE( AddOrderToOrderBook_PerfTest )
    {
        oli::idT instrId = 1000;
        oli::priceT price = 9000;
        oli::priceT priceLvl2 = 8000;

        stick_this_thread_to_core(3);

        MEOrderBookCache obookCache(instrId);
        MatchingEngine mEngine;
        METransportAdaptor testAdapt(&obookCache, &mEngine);
        mEngine.attach(&obookCache);
        mEngine.attach(&testAdapt);

        oli::idT orderId = 12345;
        const size_t PACKAGE_COUNT = 1; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 1000000/PACKAGE_COUNT; //20000000
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT,
                             [&]() {
                                 oli::msg::OBBookChangeMsg obChangeMsg;
                                 obChangeMsg.set_type(add_OBookChangeType);
                                 obChangeMsg.set_side(buySide);
                                 obChangeMsg.set_instrumentid(instrId);
                                 obChangeMsg.set_orderid(orderId++);
                                 obChangeMsg.set_price(price);
                                 obChangeMsg.set_volume(300);
                                 obChangeMsg.set_prevprice(0);
                                 obChangeMsg.set_prevvolume(0);
                                 testAdapt.onMessage(&obChangeMsg);

                                 obChangeMsg.set_type(add_OBookChangeType);
                                 obChangeMsg.set_side(buySide);
                                 obChangeMsg.set_instrumentid(instrId);
                                 obChangeMsg.set_orderid(orderId++);
                                 obChangeMsg.set_price(price);
                                 obChangeMsg.set_volume(300);
                                 obChangeMsg.set_prevprice(0);
                                 obChangeMsg.set_prevvolume(0);
                                 testAdapt.onMessage(&obChangeMsg);

                                 obChangeMsg.set_type(add_OBookChangeType);
                                 obChangeMsg.set_side(buySide);
                                 obChangeMsg.set_instrumentid(instrId);
                                 obChangeMsg.set_orderid(orderId++);
                                 obChangeMsg.set_price(price);
                                 obChangeMsg.set_volume(300);
                                 obChangeMsg.set_prevprice(0);
                                 obChangeMsg.set_prevvolume(0);
                                 testAdapt.onMessage(&obChangeMsg);
                             },
                             [&](){
                                 /// restore order book
                                 oli::msg::OBBookChangeMsg obChangeMsg;
                                 obChangeMsg.set_side(buySide);
                                 obChangeMsg.set_type(delete_OBookChangeType);
                                 obChangeMsg.set_orderid(orderId - 1);
                                 testAdapt.onMessage(&obChangeMsg);
                                 obChangeMsg.set_type(delete_OBookChangeType);
                                 obChangeMsg.set_orderid(orderId - 2);
                                 testAdapt.onMessage(&obChangeMsg);
                                 obChangeMsg.set_type(delete_OBookChangeType);
                                 obChangeMsg.set_orderid(orderId - 3);
                                 testAdapt.onMessage(&obChangeMsg);
                             });

        std::cout << "Latency of Add order to OB processing:" << std::endl;
        perfTestCase.writeResult();

        std::cout << std::endl;
    }


/*    BOOST_AUTO_TEST_CASE( AddOrderToOrderBook_RandPrice_PerfTest )
    {
        oli::idT instrId = 1000;
        oli::priceT price = 9000;
        oli::priceT priceLvl2 = 8000;

        stick_this_thread_to_core(3);

        MEOrderBookCache obookCache(instrId);
        MatchingEngine mEngine;
        METransportAdaptor testAdapt(&obookCache, &mEngine);
        mEngine.attach(&obookCache);
        mEngine.attach(&testAdapt);

        oli::idT orderId = 12345;
        const size_t PACKAGE_COUNT = 1; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 1000000/PACKAGE_COUNT; //20000000
        std::vector<double> durSamples(SAMPLES_COUNT, 0);
        durSamples.reserve(SAMPLES_COUNT);
        for(size_t i = 0; i < SAMPLES_COUNT; ++i)
        {
            high_resolution_clock::time_point t1 = high_resolution_clock::now();
            for(size_t j = 0; j < PACKAGE_COUNT; ++j)
            {
                oli::msg::OBBookChangeMsg obChangeMsg;
                obChangeMsg.set_type(add_OBookChangeType);
                obChangeMsg.set_side(buySide);
                obChangeMsg.set_instrumentid(instrId);
                obChangeMsg.set_orderid(orderId++);
                obChangeMsg.set_price(price);
                obChangeMsg.set_volume(300);
                obChangeMsg.set_prevprice(0);
                obChangeMsg.set_prevvolume(0);
                testAdapt.onMessage(&obChangeMsg);

                obChangeMsg.set_type(add_OBookChangeType);
                obChangeMsg.set_side(buySide);
                obChangeMsg.set_instrumentid(instrId);
                obChangeMsg.set_orderid(orderId++);
                obChangeMsg.set_price(price);
                obChangeMsg.set_volume(300);
                obChangeMsg.set_prevprice(0);
                obChangeMsg.set_prevvolume(0);
                testAdapt.onMessage(&obChangeMsg);

                obChangeMsg.set_type(add_OBookChangeType);
                obChangeMsg.set_side(buySide);
                obChangeMsg.set_instrumentid(instrId);
                obChangeMsg.set_orderid(orderId++);
                obChangeMsg.set_price(price);
                obChangeMsg.set_volume(300);
                obChangeMsg.set_prevprice(0);
                obChangeMsg.set_prevvolume(0);
                testAdapt.onMessage(&obChangeMsg);
            }
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            durSamples[i] = time_span.count()*1000000;

            /// restore order book
            oli::msg::OBBookChangeMsg obChangeMsg;
            obChangeMsg.set_side(buySide);
            obChangeMsg.set_price(price);
            obChangeMsg.set_type(delete_OBookChangeType);
            obChangeMsg.set_orderid(orderId - 1);
            testAdapt.onMessage(&obChangeMsg);
            obChangeMsg.set_type(delete_OBookChangeType);
            obChangeMsg.set_orderid(orderId - 2);
            testAdapt.onMessage(&obChangeMsg);
            obChangeMsg.set_type(delete_OBookChangeType);
            obChangeMsg.set_orderid(orderId - 3);
            testAdapt.onMessage(&obChangeMsg);
        }

        std::sort(durSamples.begin(), durSamples.end());
        const size_t BUCKETS_COUNT = 20;
        std::vector<size_t> buckets(10 + BUCKETS_COUNT + 1, 0);
        double minDur = durSamples[0];
        double intervalLenSmall = (1.0 - minDur)/ 10;
        double intervalLen = (durSamples[SAMPLES_COUNT - 1] - 1.0)/ BUCKETS_COUNT;
        for(auto d : durSamples){
            if(d <= 1.0){
                double index = (d - minDur)/intervalLenSmall;
                ++buckets[(size_t)index];
            }else{
                double index = (d - 1.0)/intervalLen;
                ++buckets[10 + (size_t)index];
            }
        }

        std::cout << "Latency of Add order to OB processing:" << std::endl;
        for(size_t i = 0; i < buckets.size(); ++i){
            if(i < 10){
                std::cout << "Interval [" << minDur<< ", "<< minDur + intervalLenSmall<< "]: " << buckets[i]<< std::endl;
                minDur += intervalLenSmall;
            }else{
                std::cout << "Interval [" << minDur<< ", "<< minDur + intervalLen<< "]: " << buckets[i]<< std::endl;
                minDur += intervalLen;
            }
        }

        {
            high_resolution_clock::time_point t1 = high_resolution_clock::now();
            high_resolution_clock::time_point t2 = high_resolution_clock::now();
            duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
            std::cout << "Timer granularity [" << time_span.count()*1000000<< "] mksec"<< std::endl;
        }

        std::cout << std::endl;
    }*/

BOOST_AUTO_TEST_SUITE_END()


#endif