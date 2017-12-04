//
// Created by sam1203 on 10/17/17.
//


#ifndef __PERF_TEST_ONLY___

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MatchingEngLibTest
// include this to get main() and UTF included by headers
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wterminate"
#include <boost/test/included/unit_test.hpp>
#pragma GCC diagnostic pop

#else

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

using namespace oli::mengine;
using namespace oli::msg;
using namespace std::chrono;

#include <sched.h>
#include <unistd.h>
#include "PriceLevelContainer.h"

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

int main()
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

    return 0;
}

#endif