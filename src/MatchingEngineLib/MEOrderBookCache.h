//
// Created by sam1203 on 9/18/17.
//

#ifndef MATCHINGENGINELIB_MEORDERBOOKCACHE_H
#define MATCHINGENGINELIB_MEORDERBOOKCACHE_H

#include "MatchingEngineDef.h"
#include <list>
#include <map>
#include <algorithm>

namespace oli {

    namespace mengine {

        class MEOrderBookCache: public IInMEOBookObserver, public IOrderBookProcessor {
        public:
            MEOrderBookCache(const oli::idT &instrId);
            virtual ~MEOrderBookCache();

        public:
            /// implementation of the IInMEOBookObserver
            virtual void onOBookChanged(const MDLevel2Record &order) override;
            virtual void onOBooksChanged(const MDLevel2Record &askUpdate,
                                         const MDLevel2Record &bidUpdate) override;
            virtual void onOBooksReset(const oli::idT &instrId) override;
        public:
            /// implementation of the IOrderBookProcessor
            virtual void process(IOrderBookFunctor *func) override;

        private:
            OrderAtPriceLevelT *topBidPriceLevel_ = nullptr;
            OrderAtPriceLevelT *topAskPriceLevel_ = nullptr;
            BidOrderBookT bidOrderBook_;
            AskOrderBookT askOrderBook_;
            OrderId2PositionT order2Position_;
            const oli::idT instrId_;



        };

    }
}

#endif //MATCHINGENGINELIB_MEORDERBOOKCACHE_H
