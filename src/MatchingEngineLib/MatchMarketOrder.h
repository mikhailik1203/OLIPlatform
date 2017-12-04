//
// Created by sam1203 on 9/24/17.
//

#ifndef MATCHINGENGINELIB_MATCHMARKETORDER_H
#define MATCHINGENGINELIB_MATCHMARKETORDER_H

#include "MatchingEngineDef.h"
#include <cassert>
#include <memory>

namespace oli {

    namespace mengine {

        template <class OBCont>
        class MatchHelper;

        class MatchMarketOrder: public IOrderBookFunctor{
        public:
            MatchMarketOrder();
            virtual ~MatchMarketOrder();

            const MatchedOrdersT &matchedOrders()const;

            void init(const MarketOrderData *order);
        public:
            /// Implementation of the IOrderBookFunctor
            virtual void handle(BidOrderBookT &bidBook, AskOrderBookT &askBook) override;

        private:
            MatchedOrdersT matchedOrders_;
            const MarketOrderData *order_;
            std::unique_ptr<MatchHelper<AskOrderBookT>> askHelper_;
            std::unique_ptr<MatchHelper<BidOrderBookT>> bidHelper_;
        };




    }
}

#endif //MATCHINGENGINELIB_MATCHMARKETORDER_H
