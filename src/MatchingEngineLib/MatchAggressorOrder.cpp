//
// Created by sam1203 on 9/24/17.
//

#include "MatchAggressorOrder.h"

using namespace oli::mengine;

namespace{

    template<class OBCont>
    struct OrderBookTraits{
        bool checkPrice(const oli::priceT &orderPx,
                        const oli::priceT &priceLevel);

        void addMatchedData(const oli::idT &aggrOrderId,
                            const oli::idT &matchedOrderId,
                            const oli::quantityT &matchQty,
                            const oli::priceT &matchPx,
                            MatchedOrdersT &matchedOrders);
    };

    template<>
    struct OrderBookTraits<BidOrderBookT>{
        bool checkPrice(const oli::priceT &orderPx,
                        const oli::priceT &priceLevel)
        {
            return priceLevel >= orderPx;
        }

        void addMatchedData(const oli::idT &aggrOrderId,
                                           const oli::idT &matchedOrderId,
                                           const oli::quantityT &matchQty,
                                           const oli::priceT &matchPx,
                                           MatchedOrdersT &matchedOrders)
        {
            matchedOrders.push_back(MatchedOrderData(matchedOrderId, aggrOrderId, matchQty, matchPx));
        }
    };

    template<>
    struct OrderBookTraits<AskOrderBookT>{
        bool checkPrice(const oli::priceT &orderPx,
                        const oli::priceT &priceLevel)
        {
            return priceLevel <= orderPx;
        }

        void addMatchedData(const oli::idT &aggrOrderId,
                                           const oli::idT &matchedOrderId,
                                           const oli::quantityT &matchQty,
                                           const oli::priceT &matchPx,
                                           MatchedOrdersT &matchedOrders) {
            matchedOrders.push_back(MatchedOrderData(aggrOrderId, matchedOrderId, matchQty, matchPx));
        }

    };

    template<class OBCont>
    MatchedOrdersT match(OBCont &orderBook, const AggressorOrderData &order)
    {
        MatchedOrdersT matchedOrders;
        oli::quantityT leftQty = order.qty_;
        OrderBookTraits<OBCont> traits;

        for(auto &pxLevel : orderBook)
        {
            OrderAtPriceLevelT &orderSeq =  pxLevel.second;
            if(!traits.checkPrice(order.price_, pxLevel.first))
                return matchedOrders;

            for(auto &ordId : orderSeq)
            {
                if(0 == ordId.leftQty_)
                    continue;
                oli::quantityT matchQty = std::min(leftQty, ordId.leftQty_);
                oli::priceT  matchPx = ordId.price_;
                traits.addMatchedData(order.id_, ordId.id_, matchQty, matchPx, matchedOrders);
                leftQty -= matchQty;
                ordId.leftQty_ -= matchQty;
                if(0 == leftQty)
                    return matchedOrders;
            }
        }
        return matchedOrders;
    }


}

#define LIKELY(condition) __builtin_expect((condition), 1)

namespace oli{
    namespace mengine {

        template<class OBCont>
        class MatchAggrHelper {
        private:
            MatchedOrdersT *matchOrders_ = nullptr;
            oli::idT marketOrderId_;
        public:
            void init(MatchedOrdersT *matchOrders)
            {
                assert(nullptr != matchOrders);
                matchOrders_ = matchOrders;
            }

            inline void __attribute__((always_inline)) addMatchedData(const oli::idT &matchedOrderId,
                                const oli::quantityT &matchQty,
                                const oli::priceT &matchPx);

            inline bool __attribute__((always_inline)) checkPrice(const oli::priceT &orderPx,
                            const oli::priceT &priceLevel);


            void match(const AggressorOrderData &order, OBCont &orderBook) {
                marketOrderId_ = order.id_;
                oli::quantityT leftQty = order.qty_;
                for (auto &pxLevel : orderBook) {
                    OrderAtPriceLevelT &orderSeq = pxLevel.second;
                    if(LIKELY(checkPrice(order.price_, pxLevel.first))){
                        for (auto &ordId : orderSeq) {
                            if (0 != ordId.leftQty_) {
                                oli::quantityT matchQty = std::min(leftQty, ordId.leftQty_);
                                leftQty -= matchQty;
                                ordId.leftQty_ -= matchQty;
                                addMatchedData(ordId.id_, matchQty, ordId.price_);
                                if (0 == leftQty)
                                    return;
                            }
                        }
                    }else
                        return;
                }
            }
        };

        template<>
        inline void __attribute__((always_inline)) MatchAggrHelper<BidOrderBookT>::addMatchedData(const oli::idT &matchedOrderId,
                                                        const oli::quantityT &matchQty,
                                                        const oli::priceT &matchPx) {
            matchOrders_->push_back(MatchedOrderData(matchedOrderId, marketOrderId_, matchQty, matchPx));
        }

        template<>
        inline void __attribute__((always_inline)) MatchAggrHelper<AskOrderBookT>::addMatchedData(const oli::idT &matchedOrderId,
                                                        const oli::quantityT &matchQty,
                                                        const oli::priceT &matchPx) {
            matchOrders_->push_back(MatchedOrderData(marketOrderId_, matchedOrderId, matchQty, matchPx));
        }

        template<>
        inline bool __attribute__((always_inline)) MatchAggrHelper<BidOrderBookT>::checkPrice(const oli::priceT &orderPx,
                        const oli::priceT &priceLevel)
        {
            return priceLevel >= orderPx;
        }


        template<>
        inline bool __attribute__((always_inline)) MatchAggrHelper<AskOrderBookT>::checkPrice(const oli::priceT &orderPx,
                            const oli::priceT &priceLevel)
        {
            return priceLevel <= orderPx;
        }


    }
}


MatchAggressorOrder::MatchAggressorOrder():
        order_(nullptr),
        askHelper_(std::make_unique<MatchAggrHelper<AskOrderBookT>>()),
        bidHelper_(std::make_unique<MatchAggrHelper<BidOrderBookT>>())
{
    askHelper_->init(&matchedOrders_);
    bidHelper_->init(&matchedOrders_);
}

MatchAggressorOrder::~MatchAggressorOrder()
{}

void MatchAggressorOrder::init(const AggressorOrderData *order)
{
    assert(nullptr != order);
    order_ = order;
    matchedOrders_.reset();
}

const MatchedOrdersT &MatchAggressorOrder::matchedOrders()const{
    return matchedOrders_;
}

void MatchAggressorOrder::handle(BidOrderBookT &bidBook, AskOrderBookT &askBook)
{
    if(oli::Side::buy_Side == order_->side_) {
        askHelper_->match(*order_, askBook);
    }else{
        bidHelper_->match(*order_, bidBook);
    }
}
