//
// Created by sam1203 on 9/18/17.
//

#ifndef MATCHINGENGINELIB_MATCHINGENGINE_H
#define MATCHINGENGINELIB_MATCHINGENGINE_H

#include "MatchingEngineDef.h"
#include <atomic>
#include "MatchMarketOrder.h"
#include "MatchAggressorOrder.h"

namespace oli {

    namespace mengine {

        class MatchingEngine: public IInMEOrderObserver{
        public:
            MatchingEngine();
            virtual ~MatchingEngine();

            void attach(IOutMEObserver *obs);
            void attach(IOrderBookProcessor *proc);

        public:
            /// Implementation of the IInMEOrderObserver
            virtual void onNewMarketOrder(const MarketOrderData &order) override;
            virtual void onNewOrderAggressor(const AggressorOrderData &order) override;

        private:
            std::atomic<IOutMEObserver *> obs_;
            std::atomic<IOrderBookProcessor *> proc_;
            MatchMarketOrder marketOrderMatcher_;
            MatchAggressorOrder aggrOrderMatcher_;
        };

    }
}

#endif //MATCHINGENGINELIB_MATCHINGENGINE_H
