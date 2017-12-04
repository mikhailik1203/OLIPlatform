//
// Created by sam1203 on 9/18/17.
//

#include "MEOrderBookCache.h"
#include <cassert>
#include <mutex>


using namespace oli::mengine;

namespace{
    const size_t ORDER_COUNT_RESERVE = 1000;

}

MEOrderBookCache::MEOrderBookCache(const oli::idT &instrId): instrId_(instrId)
{
    order2Position_.reserve(ORDER_COUNT_RESERVE);

}

MEOrderBookCache::~MEOrderBookCache()
{}

void MEOrderBookCache::onOBookChanged(const MDLevel2Record &order)
{
    /// update order book according to the event
    /// fire IMatchingEngine::onOrderBookCrossed() if bid&ask orderbooks are crossed
    switch(order.type_){
        case MDUpdateType::add_MDUpdateType:
        {
            assert(oli::INVALID_ID != order.id_);
            assert(0 != order.price_);
            assert(oli::Side::invalid_Side != order.side_);

            OrderData ordData = {order.id_, order.qty_, order.price_};
            OrderAtPriceLevelT &ordList =
                    (oli::Side::sell_Side == order.side_) ?
                    (askOrderBook_[order.price_]) : (bidOrderBook_[order.price_]);
            order2Position_[order.id_] = ordList.insert(ordList.end(), ordData);
        }
        break;
        case MDUpdateType::remove_MDUpdateType:
        {
            assert(oli::INVALID_ID != order.id_);
            assert(oli::Side::invalid_Side != order.side_);

            auto orderDataIt = order2Position_.find(order.id_);
            if(order2Position_.end() == orderDataIt)
                throw std::runtime_error("MEOrderBookCache::onOBookChanged: unable to process remove_MDUpdateType - no order located");
            oli::priceT px = orderDataIt->second->price_;
            OrderAtPriceLevelT &ordList =
                    (oli::Side::sell_Side == order.side_) ?
                    (askOrderBook_[px]) : (bidOrderBook_[px]);
            ordList.erase(orderDataIt->second);
            order2Position_.erase(orderDataIt);
        }
        break;
        case MDUpdateType::update_MDUpdateType:
        {
            auto orderDataIt = order2Position_.find(order.id_);
            if(order2Position_.end() == orderDataIt)
                throw std::runtime_error("MEOrderBookCache::onOBookChanged: unable to process update_MDUpdateType - no order located");
            if(order.price_ != orderDataIt->second->price_){
                /// remove price at old price level
                OrderAtPriceLevelT &ordListOld =
                        (oli::Side::sell_Side == order.side_) ?
                        (askOrderBook_[orderDataIt->second->price_]) : (bidOrderBook_[orderDataIt->second->price_]);
                ordListOld.erase(orderDataIt->second);

                /// add price at new price level
                OrderAtPriceLevelT &ordListNew =
                        (oli::Side::sell_Side == order.side_) ?
                        (askOrderBook_[order.price_]) : (bidOrderBook_[order.price_]);
                OrderData ordData = {order.id_, order.qty_, order.price_};
                *(orderDataIt->second) = ordData;
            }else if(order.qty_ != orderDataIt->second->leftQty_){
                /// update leftQty
                OrderAtPriceLevelT &ordList =
                        (oli::Side::sell_Side == order.side_) ?
                        (askOrderBook_[order.price_]) : (bidOrderBook_[order.price_]);
                orderDataIt->second->leftQty_ = order.qty_;
            }else
                throw std::runtime_error("MEOrderBookCache::onOBookChanged: unable to execute update_MDUpdateType - no data changed");
        }
        break;
        default:
            throw std::runtime_error("MEOrderBookCache::onOBookChanged: invalid order's MDUpdateType");
    };
}

void MEOrderBookCache::onOBooksChanged(const MDLevel2Record &askUpdate,
                             const MDLevel2Record &bidUpdate)
{
    /// update order book according to the event
    /// fire IMatchingEngine::onOrderBookCrossed() if bid&ask orderbooks are crossed
}

void MEOrderBookCache::onOBooksReset(const oli::idT &instrId)
{
    /// clear orderbooks
    BidOrderBookT tmpBidBook;
    AskOrderBookT tmpAskBook;
    OrderId2PositionT orders;
    {
        std::swap(bidOrderBook_, tmpBidBook);
        std::swap(askOrderBook_, tmpAskBook);
        std::swap(order2Position_, orders);
    }
}

void MEOrderBookCache::process(IOrderBookFunctor *func)
{
    assert(nullptr != func);
    func->handle(bidOrderBook_, askOrderBook_);
}
