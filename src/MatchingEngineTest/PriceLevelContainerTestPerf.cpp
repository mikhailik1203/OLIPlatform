// Created by sam1203 on 11/13/17.

//#define __PERF_TEST_ONLY___

#ifndef __PERF_TEST_ONLY___

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "OLI_Logger.h"
#include "OBProcessor.h"
#include "PriceLevelContainer.h"

#include <iostream>
#include <map>
#include <atomic>
#include <memory>

#include <sched.h>
#include <unistd.h>


using namespace oli::mengine;
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

    const size_t ORDER_BLOCK_SIZE = 32;
    const size_t INVALID_PRICELEVEL_BLOCK = 0;
    const size_t FIRST_PRICE_INDEX_BLOCK = 2;
    const size_t LAST_PRICE_INDEX_BLOCK = 31;


    char getFirstUsedIndex(uint32_t val){
        unsigned bitsInMask[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
                                 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000,
                                 0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
                                 0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000};
        char index = FIRST_PRICE_INDEX_BLOCK;
        uint32_t mask = val;
        while(index < LAST_PRICE_INDEX_BLOCK + 1)
        {
            if(0 != (mask & bitsInMask[index]))
                return index;
            ++index;
        }
        return 0;
    }

    char getFirstUsedIndexCycle(uint32_t val){
        uint32_t mask = val;
        char index = FIRST_PRICE_INDEX_BLOCK;
        while(index < LAST_PRICE_INDEX_BLOCK + 1)
        {
            if(0 != (mask & 4))
                return index;
            mask >>= 1;
            ++index;
        }
        return 0;
    }

    char getFirstUsedIndexFFS(uint32_t val){
        char pos = ffs(val/* & 0xFFFFFFFC*/);
        //if(pos >= FIRST_PRICE_INDEX_BLOCK)
            return pos - 1;
        //return 0;
    }

    char getLastUsedIndex(uint32_t val){
        uint32_t mask = val;
        char index = LAST_PRICE_INDEX_BLOCK;
        while(mask > 0)
        {
            if(0 != (mask & 0x80000000))
                return index;
            mask <<= 1;
            --index;
        }
        return 0;
    }

    char getLastUsedIndexCLZ(uint32_t val){
        int res = 31 - __builtin_clz(val);
        return res;
/*        uint32_t mask = val;
        val |= (val >> 1);
        val |= (val >> 2);
        val |= (val >> 4);
        val |= (val >> 8);
        val |= (val >> 16);
        int res = (val & ~(val >> 1));
        return res;*/
    }

}

BOOST_AUTO_TEST_SUITE( PriceLevelContainerPerf )

    BOOST_AUTO_TEST_CASE( PriceLevelContainer_FirstUsedCycleCheck )
    {
        int32_t value = 1;
        BOOST_CHECK(0 == getFirstUsedIndex(0));
        BOOST_CHECK(2 == getFirstUsedIndex(0xFFFFFFFF));
        BOOST_CHECK(2 == getFirstUsedIndex(0xFFFFFFFF << 1));
        for(int i = 2; i < 32; ++i){
            BOOST_CHECK(i == getFirstUsedIndex(0xFFFFFFFF << i));
        }
    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainer_FirstUsedIndexCheck )
    {
        int32_t value = 1;
        BOOST_CHECK(0 == getFirstUsedIndexCycle(0));
        BOOST_CHECK(2 == getFirstUsedIndexCycle(0xFFFFFFFF));
        BOOST_CHECK(2 == getFirstUsedIndexCycle(0xFFFFFFFF << 1));
        for(int i = 2; i < 32; ++i){
            BOOST_CHECK(i == getFirstUsedIndexCycle(0xFFFFFFFF << i));
        }
    }

/*    BOOST_AUTO_TEST_CASE( PriceLevelContainer_FirstUsedFFSCheck )
    {
        int32_t value = 1;
        BOOST_CHECK(0 == getFirstUsedIndexFFS(0));
        BOOST_CHECK(2 == getFirstUsedIndexFFS(0xFFFFFFFF));
        BOOST_CHECK(2 == getFirstUsedIndexFFS(0xFFFFFFFF << 1));
        for(int i = 2; i < 32; ++i){
            BOOST_CHECK(i == getFirstUsedIndexFFS(0xFFFFFFFF << i));
        }
    }*/

    /*BOOST_AUTO_TEST_CASE( PriceLevelContainer_FirstUsedIndex )
    {

        stick_this_thread_to_core(3);

        const size_t PACKAGE_COUNT = 100; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 100000; //20000000

        int32_t val = 0;
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            char res = getFirstUsedIndex(0xFFFFFFFF);
            res = getFirstUsedIndex(0xFFFFFFFE);
            res = getFirstUsedIndex(0xFFFFFFFC);
            res = getFirstUsedIndex(0xFFFFFFF8);
            res = getFirstUsedIndex(0xFFFFFFF0);
            res = getFirstUsedIndex(0xFFFFFFE0);
            res = getFirstUsedIndex(0xFFFFFFC0);
            res = getFirstUsedIndex(0xFFFFFF80);
            res = getFirstUsedIndex(0xFFFFFF00);
            res = getFirstUsedIndex(0xFFFFFE00);
            res = getFirstUsedIndex(0xFFFFFC00);
            res = getFirstUsedIndex(0xFFFFF800);
            res = getFirstUsedIndex(0xFFFFF000);
            res = getFirstUsedIndex(0xFFFFE000);
            res = getFirstUsedIndex(0xFFFFC000);
            res = getFirstUsedIndex(0xFFFF8000);
            res = getFirstUsedIndex(0xFFFF0000);
            res = getFirstUsedIndex(0xFFFE0000);
            res = getFirstUsedIndex(0xFFFC0000);
            res = getFirstUsedIndex(0xFFF80000);
            res = getFirstUsedIndex(0xFFF00000);
            res = getFirstUsedIndex(0xFFE00000);
            res = getFirstUsedIndex(0xFFC00000);
            res = getFirstUsedIndex(0xFF800000);
            res = getFirstUsedIndex(0xFF000000);
            res = getFirstUsedIndex(0xFE000000);
            res = getFirstUsedIndex(0xFC000000);
            res = getFirstUsedIndex(0xF8000000);
            res = getFirstUsedIndex(0xF0000000);
            res = getFirstUsedIndex(0xE0000000);
            res = getFirstUsedIndex(0xC0000000);
            res = getFirstUsedIndex(0x80000000);
        }, [](){});

        std::cout << "Latency of getFirstUsedIndex processing:" << std::endl;
        perfTestCase.writeResult();

    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainer_FirstUsedIndexCycle )
    {

        stick_this_thread_to_core(3);

        const size_t PACKAGE_COUNT = 100; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 100000; //20000000

        int32_t val = 0;
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            char res = getFirstUsedIndexCycle(0xFFFFFFFF);
            res = getFirstUsedIndexCycle(0xFFFFFFFE);
            res = getFirstUsedIndexCycle(0xFFFFFFFC);
            res = getFirstUsedIndexCycle(0xFFFFFFF8);
            res = getFirstUsedIndexCycle(0xFFFFFFF0);
            res = getFirstUsedIndexCycle(0xFFFFFFE0);
            res = getFirstUsedIndexCycle(0xFFFFFFC0);
            res = getFirstUsedIndexCycle(0xFFFFFF80);
            res = getFirstUsedIndexCycle(0xFFFFFF00);
            res = getFirstUsedIndexCycle(0xFFFFFE00);
            res = getFirstUsedIndexCycle(0xFFFFFC00);
            res = getFirstUsedIndexCycle(0xFFFFF800);
            res = getFirstUsedIndexCycle(0xFFFFF000);
            res = getFirstUsedIndexCycle(0xFFFFE000);
            res = getFirstUsedIndexCycle(0xFFFFC000);
            res = getFirstUsedIndexCycle(0xFFFF8000);
            res = getFirstUsedIndexCycle(0xFFFF0000);
            res = getFirstUsedIndexCycle(0xFFFE0000);
            res = getFirstUsedIndexCycle(0xFFFC0000);
            res = getFirstUsedIndexCycle(0xFFF80000);
            res = getFirstUsedIndexCycle(0xFFF00000);
            res = getFirstUsedIndexCycle(0xFFE00000);
            res = getFirstUsedIndexCycle(0xFFC00000);
            res = getFirstUsedIndexCycle(0xFF800000);
            res = getFirstUsedIndexCycle(0xFF000000);
            res = getFirstUsedIndexCycle(0xFE000000);
            res = getFirstUsedIndexCycle(0xFC000000);
            res = getFirstUsedIndexCycle(0xF8000000);
            res = getFirstUsedIndexCycle(0xF0000000);
            res = getFirstUsedIndexCycle(0xE0000000);
            res = getFirstUsedIndexCycle(0xC0000000);
            res = getFirstUsedIndexCycle(0x80000000);
        }, [](){});

        std::cout << "Latency of getFirstUsedIndexCycle processing:" << std::endl;
        perfTestCase.writeResult();

    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainer_FirstUsedIndexFFS )
    {

        stick_this_thread_to_core(3);

        const size_t PACKAGE_COUNT = 100; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 100000; //20000000

        int32_t val = 0;
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            char res = getFirstUsedIndexFFS(0xFFFFFFFF);
            res = getFirstUsedIndexFFS(0xFFFFFFFE);
            res = getFirstUsedIndexFFS(0xFFFFFFFC);
            res = getFirstUsedIndexFFS(0xFFFFFFF8);
            res = getFirstUsedIndexFFS(0xFFFFFFF0);
            res = getFirstUsedIndexFFS(0xFFFFFFE0);
            res = getFirstUsedIndexFFS(0xFFFFFFC0);
            res = getFirstUsedIndexFFS(0xFFFFFF80);
            res = getFirstUsedIndexFFS(0xFFFFFF00);
            res = getFirstUsedIndexFFS(0xFFFFFE00);
            res = getFirstUsedIndexFFS(0xFFFFFC00);
            res = getFirstUsedIndexFFS(0xFFFFF800);
            res = getFirstUsedIndexFFS(0xFFFFF000);
            res = getFirstUsedIndexFFS(0xFFFFE000);
            res = getFirstUsedIndexFFS(0xFFFFC000);
            res = getFirstUsedIndexFFS(0xFFFF8000);
            res = getFirstUsedIndexFFS(0xFFFF0000);
            res = getFirstUsedIndexFFS(0xFFFE0000);
            res = getFirstUsedIndexFFS(0xFFFC0000);
            res = getFirstUsedIndexFFS(0xFFF80000);
            res = getFirstUsedIndexFFS(0xFFF00000);
            res = getFirstUsedIndexFFS(0xFFE00000);
            res = getFirstUsedIndexFFS(0xFFC00000);
            res = getFirstUsedIndexFFS(0xFF800000);
            res = getFirstUsedIndexFFS(0xFF000000);
            res = getFirstUsedIndexFFS(0xFE000000);
            res = getFirstUsedIndexFFS(0xFC000000);
            res = getFirstUsedIndexFFS(0xF8000000);
            res = getFirstUsedIndexFFS(0xF0000000);
            res = getFirstUsedIndexFFS(0xE0000000);
            res = getFirstUsedIndexFFS(0xC0000000);
            res = getFirstUsedIndexFFS(0x80000000);

        }, [](){});

        std::cout << "Latency of getFirstUsedIndexFFS processing:" << std::endl;
        perfTestCase.writeResult();

    }*/

    BOOST_AUTO_TEST_CASE( PriceLevelContainer_LastUsedIndexCheck )
    {
        BOOST_CHECK(0 == getLastUsedIndex(0));
        BOOST_CHECK(31 == getLastUsedIndex(0xFFFFFFFF));
        for(int i = 31; i > 1; --i){
            BOOST_CHECK(i == getLastUsedIndex(1 << i));
        }
    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainer_LastUsedShiftCheck )
    {
        /*todo: fix it
        BOOST_CHECK(0 == getLastUsedIndexCLZ(0));
        BOOST_CHECK(31 == getLastUsedIndexCLZ(0xFFFFFFFF));
        for(int i = 31; i > 1; --i){
            BOOST_CHECK(i == getLastUsedIndexCLZ(1 << i));
        }*/
    }

/*    BOOST_AUTO_TEST_CASE( PriceLevelContainer_LastUsedIndex )
    {

        stick_this_thread_to_core(3);

        const size_t PACKAGE_COUNT = 100; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 1000000; //20000000

        int32_t val = 0;
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            char res = getLastUsedIndex(0xFFFFFFFF);
            res = getLastUsedIndex(0x7FFFFFFF);
            res = getLastUsedIndex(0x3FFFFFFF);
            res = getLastUsedIndex(0x1FFFFFFF);
            res = getLastUsedIndex(0xFFFFFFF);
            res = getLastUsedIndex(0x7FFFFFF);
            res = getLastUsedIndex(0x3FFFFFF);
            res = getLastUsedIndex(0x1FFFFFF);
            res = getLastUsedIndex(0xFFFFFF);
            res = getLastUsedIndex(0x7FFFFF);
            res = getLastUsedIndex(0x3FFFFF);
            res = getLastUsedIndex(0x1FFFFF);
            res = getLastUsedIndex(0xFFFFF);
            res = getLastUsedIndex(0x7FFFF);
            res = getLastUsedIndex(0x3FFFF);
            res = getLastUsedIndex(0x1FFFF);
            res = getLastUsedIndex(0xFFFF);
            res = getLastUsedIndex(0x7FFF);
            res = getLastUsedIndex(0x3FFF);
            res = getLastUsedIndex(0x1FFF);
            res = getLastUsedIndex(0xFFF);
            res = getLastUsedIndex(0x7FF);
            res = getLastUsedIndex(0x3FF);
            res = getLastUsedIndex(0x1FF);
            res = getLastUsedIndex(0xFF);
            res = getLastUsedIndex(0x7F);
            res = getLastUsedIndex(0x3F);
            res = getLastUsedIndex(0x1F);
            res = getLastUsedIndex(0xF);
            res = getLastUsedIndex(0x7);
            res = getLastUsedIndex(0x3);
            res = getLastUsedIndex(0x1);

        }, [](){});

        std::cout << "Latency of getLastUsedIndex processing:" << std::endl;
        perfTestCase.writeResult();

    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainer_LastUsedCLZ )
    {

        stick_this_thread_to_core(3);

        const size_t PACKAGE_COUNT = 100; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 1000000; //20000000

        int32_t val = 0;
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            char res = getLastUsedIndexCLZ(0xFFFFFFFF);
            res = getLastUsedIndexCLZ(0x7FFFFFFF);
            res = getLastUsedIndexCLZ(0x3FFFFFFF);
            res = getLastUsedIndexCLZ(0x1FFFFFFF);
            res = getLastUsedIndexCLZ(0xFFFFFFF);
            res = getLastUsedIndexCLZ(0x7FFFFFF);
            res = getLastUsedIndexCLZ(0x3FFFFFF);
            res = getLastUsedIndexCLZ(0x1FFFFFF);
            res = getLastUsedIndexCLZ(0xFFFFFF);
            res = getLastUsedIndexCLZ(0x7FFFFF);
            res = getLastUsedIndexCLZ(0x3FFFFF);
            res = getLastUsedIndexCLZ(0x1FFFFF);
            res = getLastUsedIndexCLZ(0xFFFFF);
            res = getLastUsedIndexCLZ(0x7FFFF);
            res = getLastUsedIndexCLZ(0x3FFFF);
            res = getLastUsedIndexCLZ(0x1FFFF);
            res = getLastUsedIndexCLZ(0xFFFF);
            res = getLastUsedIndexCLZ(0x7FFF);
            res = getLastUsedIndexCLZ(0x3FFF);
            res = getLastUsedIndexCLZ(0x1FFF);
            res = getLastUsedIndexCLZ(0xFFF);
            res = getLastUsedIndexCLZ(0x7FF);
            res = getLastUsedIndexCLZ(0x3FF);
            res = getLastUsedIndexCLZ(0x1FF);
            res = getLastUsedIndexCLZ(0xFF);
            res = getLastUsedIndexCLZ(0x7F);
            res = getLastUsedIndexCLZ(0x3F);
            res = getLastUsedIndexCLZ(0x1F);
            res = getLastUsedIndexCLZ(0xF);
            res = getLastUsedIndexCLZ(0x7);
            res = getLastUsedIndexCLZ(0x3);
            res = getLastUsedIndexCLZ(0x1);

        }, [](){});

        std::cout << "Latency of getLastUsedIndexCLZ processing:" << std::endl;
        perfTestCase.writeResult();

    }*/


    BOOST_AUTO_TEST_CASE( PriceLevelContainer_AddOrderSamePrice_10Package)
    {
        oli::idT startOrderId = 1234;
        oli::quantityT startOrderQty = 200;

        stick_this_thread_to_core(3);

        const size_t PACKAGE_COUNT = 10; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 10000; //20000000

        PriceLevelContainer testCont(PACKAGE_COUNT*SAMPLES_COUNT + 1);

        MDLevel2Record order2Add;
        order2Add.id_ = startOrderId;
        order2Add.price_ = 5000;
        order2Add.qty_ = startOrderQty;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;

        sleep(2);
        int32_t val = 0;
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            ++order2Add.id_;
            order2Add.qty_ += 10;
            testCont.add(order2Add);
        }, [&](){
        });

        std::cout << "Latency of PriceLevelContainer::AddOrderSamePrice_10Package processing:" << std::endl;
        perfTestCase.writeResult();

    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainer2Cmp_AddOrderSamePrice_10Package)
    {
        oli::idT startOrderId = 1234;
        oli::quantityT startOrderQty = 200;

        stick_this_thread_to_core(3);

        const size_t PACKAGE_COUNT = 10; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 10000; //20000000

        PriceLevelContainer2Cmp testCont(PACKAGE_COUNT*SAMPLES_COUNT + 1);

        MDLevel2Record order2Add;
        order2Add.id_ = startOrderId;
        order2Add.price_ = 5000;
        order2Add.qty_ = startOrderQty;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;

        sleep(2);
        int32_t val = 0;
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            ++order2Add.id_;
            order2Add.qty_ += 10;
            testCont.add(order2Add);
        }, [&](){
        });

        std::cout << "Latency of PriceLevelContainer2Cmp::AddOrderSamePrice_10Package processing:" << std::endl;
        perfTestCase.writeResult();

    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainer_AddOrderAtSamePriceRepeated_1000Package)
    {
        oli::idT startOrderId = 1234;
        oli::quantityT startOrderQty = 200;

        stick_this_thread_to_core(3);

        const size_t PACKAGE_COUNT = 1000; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 10000; //20000000

        PriceLevelContainer testCont(PACKAGE_COUNT + 1);

        MDLevel2Record order2Add;
        order2Add.id_ = startOrderId;
        order2Add.price_ = 5000;
        order2Add.qty_ = startOrderQty;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;

        MDLevel2Record order2Remove;
        order2Remove.id_ = startOrderId;
        order2Remove.price_ = 5000;
        order2Remove.qty_ = startOrderQty;
        order2Remove.side_ = oli::Side::buy_Side;
        order2Remove.type_ = MDUpdateType::remove_MDUpdateType;

        sleep(2);
        int32_t val = 0;
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            ++order2Add.id_;
            order2Add.qty_ += 10;
            testCont.add(order2Add);
        }, [&](){
            for(size_t i = 0; i < PACKAGE_COUNT; ++i){
                ++order2Remove.id_;
                testCont.remove(order2Remove);
            }
        });

        std::cout << "Latency of PriceLevelContainer::AddOrderAtSamePriceRepeated_1000Package processing:" << std::endl;
        perfTestCase.writeResult();

    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainer2Cmp_AddOrderAtSamePriceRepeated_1000Package)
    {
        oli::idT startOrderId = 1234;
        oli::quantityT startOrderQty = 200;

        stick_this_thread_to_core(3);

        const size_t PACKAGE_COUNT = 1000; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 10000; //20000000

        PriceLevelContainer2Cmp testCont(PACKAGE_COUNT + 1);

        MDLevel2Record order2Add;
        order2Add.id_ = startOrderId;
        order2Add.price_ = 5000;
        order2Add.qty_ = startOrderQty;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;

        MDLevel2Record order2Remove;
        order2Remove.id_ = startOrderId;
        order2Remove.price_ = 5000;
        order2Remove.qty_ = startOrderQty;
        order2Remove.side_ = oli::Side::buy_Side;
        order2Remove.type_ = MDUpdateType::remove_MDUpdateType;

        sleep(2);
        int32_t val = 0;
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            ++order2Add.id_;
            order2Add.qty_ += 10;
            testCont.add(order2Add);
        }, [&](){
            for(size_t i = 0; i < PACKAGE_COUNT; ++i){
                ++order2Remove.id_;
                testCont.remove(order2Remove);
            }
        });

        std::cout << "Latency of PriceLevelContainer2Cmp::AddOrderAtSamePriceRepeated_1000Package processing:" << std::endl;
        perfTestCase.writeResult();

    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainer_AddOrderDiffPrices_20Package)
    {
        oli::idT startOrderId = 1234;
        oli::quantityT startOrderQty = 200;
        oli::priceT startPrice = 5000;

        stick_this_thread_to_core(3);

        const size_t PACKAGE_COUNT = 20; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 10000; //20000000

        PriceLevelContainer testCont(PACKAGE_COUNT*SAMPLES_COUNT + 1);

        MDLevel2Record order2Add;
        order2Add.id_ = startOrderId;
        order2Add.price_ = startPrice;
        order2Add.qty_ = startOrderQty;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;

        sleep(2);
        int32_t val = 0;
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            ++order2Add.id_;
            order2Add.qty_ += 10;
            order2Add.price_ += 100;
            testCont.add(order2Add);
        }, [&](){
            order2Add.price_ = startPrice;
        });

        std::cout << "Latency of PriceLevelContainer::AddOrderDiffPrices_20Package processing:" << std::endl;
        perfTestCase.writeResult();

    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainer2Cmp_AddOrderDiffPrices_20Package)
    {
        oli::idT startOrderId = 1234;
        oli::quantityT startOrderQty = 200;
        oli::priceT startPrice = 5000;

        stick_this_thread_to_core(3);

        const size_t PACKAGE_COUNT = 20; //batch, because of the timer granularity
        const size_t SAMPLES_COUNT = 10000; //20000000

        PriceLevelContainer2Cmp testCont(PACKAGE_COUNT*SAMPLES_COUNT + 1);

        MDLevel2Record order2Add;
        order2Add.id_ = startOrderId;
        order2Add.price_ = startPrice;
        order2Add.qty_ = startOrderQty;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;

        sleep(2);
        int32_t val = 0;
        PerfTestScenario perfTestCase;
        perfTestCase.execute(PACKAGE_COUNT, SAMPLES_COUNT, [&](){
            ++order2Add.id_;
            order2Add.qty_ += 10;
            order2Add.price_ += 100;
            testCont.add(order2Add);
        }, [&](){
            order2Add.price_ = startPrice;
        });

        std::cout << "Latency of PriceLevelContainer2Cmp::AddOrderDiffPrices_20Package processing:" << std::endl;
        perfTestCase.writeResult();

    }

BOOST_AUTO_TEST_SUITE_END()

#endif


