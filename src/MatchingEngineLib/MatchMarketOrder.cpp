//
// Created by sam1203 on 9/24/17.
//

#include "MatchMarketOrder.h"

using namespace oli::mengine;

namespace oli{
    namespace mengine {

        template<class OBCont>
        class MatchHelper {
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

            void match(const MarketOrderData &order, OBCont &orderBook) {
                marketOrderId_ = order.id_;
                oli::quantityT leftQty = order.qty_;
                for (auto &pxLevel : orderBook) {
                    OrderAtPriceLevelT &orderSeq = pxLevel.second;
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
                }
                return;
            }
        };

        template<>
        inline void __attribute__((always_inline)) MatchHelper<BidOrderBookT>::addMatchedData(const oli::idT &matchedOrderId,
                                                        const oli::quantityT &matchQty,
                                                        const oli::priceT &matchPx) {
            matchOrders_->push_back(MatchedOrderData(matchedOrderId, marketOrderId_, matchQty, matchPx));
        }

        template<>
        inline void __attribute__((always_inline)) MatchHelper<AskOrderBookT>::addMatchedData(const oli::idT &matchedOrderId,
                                                        const oli::quantityT &matchQty,
                                                        const oli::priceT &matchPx) {
            matchOrders_->push_back(MatchedOrderData(marketOrderId_, matchedOrderId, matchQty, matchPx));
        }

    }
}

MatchMarketOrder::MatchMarketOrder():
        order_(nullptr),
        askHelper_(std::make_unique<MatchHelper<AskOrderBookT>>()),
        bidHelper_(std::make_unique<MatchHelper<BidOrderBookT>>())
{
    askHelper_->init(&matchedOrders_);
    bidHelper_->init(&matchedOrders_);
}
MatchMarketOrder::~MatchMarketOrder()
{}

void MatchMarketOrder::init(const MarketOrderData *order)
{
    assert(nullptr != order);
    order_ = order;
    matchedOrders_.reset();
}

const MatchedOrdersT &MatchMarketOrder::matchedOrders()const
{
    return matchedOrders_;
}

void MatchMarketOrder::handle(BidOrderBookT &bidBook, AskOrderBookT &askBook)
{
    assert(nullptr != order_);
    if(oli::Side::buy_Side == order_->side_) {
        askHelper_->match(*order_, askBook);
    }else{
        bidHelper_->match(*order_, bidBook);
    }
}
