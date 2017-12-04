//
// Created by sam1203 on 9/24/17.
//

#ifndef MATCHINGENGINELIB_MATCHAGGRESSORORDER_H
#define MATCHINGENGINELIB_MATCHAGGRESSORORDER_H

#include "MatchingEngineDef.h"
#include <cassert>
#include <memory>

namespace oli {

    namespace mengine {

        template <class OBCont>
        class MatchAggrHelper;

        class MatchAggressorOrder: public IOrderBookFunctor{
        public:
            MatchAggressorOrder();
            virtual ~MatchAggressorOrder();

            const MatchedOrdersT &matchedOrders()const;

            void init(const AggressorOrderData *order);

        public:
            /// Implementation of the IOrderBookFunctor
            virtual void handle(BidOrderBookT &bidBook, AskOrderBookT &askBook) override;

        private:
            const AggressorOrderData *order_;
            MatchedOrdersT matchedOrders_;

            std::unique_ptr<MatchAggrHelper<AskOrderBookT>> askHelper_;
            std::unique_ptr<MatchAggrHelper<BidOrderBookT>> bidHelper_;
        };



    }
}


#endif //MATCHINGENGINELIB_MATCHAGGRESSORORDER_H
