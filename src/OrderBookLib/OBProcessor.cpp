#include "OBProcessor.h"

using namespace oli::obook;

namespace{
    const size_t PRICE_LEVEL_AMOUNT_LIMIT = 10000;
}

OBProcessor::OBProcessor(): 
    obs_(nullptr), lowestPrice_(0), priceStep_(0), 
    priceLevelAmount_(0), started_(false)
{
    orderBook_.instrumentId_ = oli::INVALID_ID;
    orderBook_.topAskPrice_ = 0;
    orderBook_.topBidPrice_ = 0;
}

OBProcessor::~OBProcessor()
{
}

void OBProcessor::attach(IOrderBookEventObserver *obs)
{
    obs_ = obs;
}

void OBProcessor::detach()
{
    obs_ = nullptr;
}

void OBProcessor::stopProcessing()
{
    started_ = false;
    
    oli::idT instrId = oli::INVALID_ID;
    {
        std::lock_guard<std::mutex> grd(lock_);
        instrId = orderBook_.instrumentId_;
        
        orderBook_.topAskPrice_ = lowestPrice_ + priceStep_*priceLevelAmount_;
        orderBook_.topBidPrice_ = lowestPrice_;
        
        for(size_t idx = 0; idx < orderBook_.askLevels_.size(); ++idx){
            orderBook_.askLevels_[idx].orders_.clear();
            orderBook_.bidLevels_[idx].orders_.clear();
        }
    }
    obs_.load()->onOBooksReset(instrId);
}

void OBProcessor::startProcessing(const secmaster::InstrumentRecord &instr)
{
    if(!started_){
        started_ = true;        
 
        lowestPrice_ = instr.lowPrice_;
        priceStep_ = instr.priceStep_; 
       
        orderBook_.instrumentId_ = instr.instrId_;
        orderBook_.topAskPrice_ = instr.highPrice_;
        orderBook_.topBidPrice_ = instr.lowPrice_;
        
        priceLevelAmount_ = (instr.highPrice_ - lowestPrice_)/priceStep_ + 1;
        if(PRICE_LEVEL_AMOUNT_LIMIT < priceLevelAmount_)
            throw std::logic_error("OBProcessor::ctor: too many price levels for instrument!");
        
        orderBook_.askLevels_.reserve(priceLevelAmount_);
        orderBook_.bidLevels_.reserve(priceLevelAmount_);
        for(oli::priceT px = lowestPrice_; px < instr.highPrice_; px += priceStep_){
            PriceLevel pl;
            pl.price_ = px;
            orderBook_.askLevels_.push_back(pl);
            orderBook_.bidLevels_.push_back(pl);
        }
        
    }
}


int OBProcessor::calculatePriceLevelIdx_ThrdUnsafe(const oli::priceT &price)const
{
    int idx = (price - lowestPrice_)/priceStep_;
    if(0 > idx)
        throw std::logic_error("OBProcessor::calculatePriceLevelIdx: invalid price of order - below lowest price!");
    if(priceLevelAmount_ < static_cast<size_t>(idx))
        throw std::logic_error("OBProcessor::calculatePriceLevelIdx: invalid price of order - greater then highest price!");
    return idx;
}

void OBProcessor::addOrder_ThrdUnsafe(const OrderBookRecord &order)
{
    int priceLevelIdx = calculatePriceLevelIdx_ThrdUnsafe(order.price_);
    
    oli::priceT *topPrice = nullptr;
    PriceLevelsT *priceLevels = nullptr;
    if(Side::buy_Side == order.side_){
        priceLevels = &orderBook_.bidLevels_;
        topPrice = &orderBook_.topBidPrice_;
        if(*topPrice < order.price_)
            *topPrice = order.price_;
    }else{
        priceLevels = &orderBook_.askLevels_;
        topPrice = &orderBook_.topAskPrice_;
        if(*topPrice > order.price_)
            *topPrice = order.price_;
    }
    
    (*priceLevels)[priceLevelIdx].orders_.push_back(OrderParams(order));
    (*priceLevels)[priceLevelIdx].volumeTotal_ += order.volumeLeft_;
}

void OBProcessor::removeOrder_ThrdUnsafe(const OrderBookRecord &order)
{
    int priceLevelIdx = calculatePriceLevelIdx_ThrdUnsafe(order.price_);
    
    oli::priceT *topPrice = nullptr;
    PriceLevelsT *priceLevels = nullptr;
    if(Side::buy_Side == order.side_){
        priceLevels = &orderBook_.bidLevels_;
        topPrice = &orderBook_.topBidPrice_;
    }else{
        priceLevels = &orderBook_.askLevels_;
        topPrice = &orderBook_.topAskPrice_;
    }

    OrderParamsT &orders = (*priceLevels)[priceLevelIdx].orders_;
    OrderParamsT::iterator itEnd = orders.end();
    OrderParamsT::iterator it = orders.begin();
    for(; it != itEnd; ++it){
        if(order.orderId_ == (*it).orderId_){
            orders.erase(it);
            break;
        }
    }
    if(itEnd == it)
        throw std::logic_error("OBProcessor::removeOrder_ThrdUnsafe: no order found at price level!");
    (*priceLevels)[priceLevelIdx].volumeTotal_ -= order.volumeLeft_;
    
    if((0 == (*priceLevels)[priceLevelIdx].volumeTotal_) && 
       (*topPrice == order.price_)){
        /// change top price level - find first level with available Qty
        if(Side::buy_Side == order.side_){
            for(size_t i = priceLevelIdx + 1; i < priceLevels->size(); ++i){
                if(0 < (*priceLevels)[i].volumeTotal_){
                    *topPrice = (*priceLevels)[i].price_;
                    break;
                }
            }
        }else{
            for(size_t i = priceLevelIdx - 1; i > 0; --i){
                if(0 < (*priceLevels)[i].volumeTotal_){
                    *topPrice = (*priceLevels)[i].price_;
                    break;
                }
            }
        }
    }
    
}

void OBProcessor::changeOrderVolume_ThrdUnsafe(const OrderBookRecord &order, const OrderBookRecord &newOrder)
{
    int priceLevelIdx = calculatePriceLevelIdx_ThrdUnsafe(order.price_);
    
    PriceLevelsT *priceLevels = nullptr;
    if(Side::buy_Side == order.side_){
        priceLevels = &orderBook_.bidLevels_;
    }else{
        priceLevels = &orderBook_.askLevels_;
    }

    if(priceLevelIdx < 0 || static_cast<size_t>(priceLevelIdx) >= priceLevels->size())
        throw std::logic_error("OBProcessor::changeOrderVolume_ThrdUnsafe: no price level to process delete!");
    
    OrderParamsT &orders = (*priceLevels)[priceLevelIdx].orders_;
    OrderParamsT::iterator itEnd = orders.end();
    OrderParamsT::iterator it = orders.begin();
    for(; it != itEnd; ++it){
        if(order.orderId_ == (*it).orderId_){
            orders.erase(it);
            orders.push_back(OrderParams(newOrder));
            break;
        }
    }
    if(itEnd == it)
        throw std::logic_error("OBProcessor::changeOrderVolume_ThrdUnsafe: no order found at price level!");
    (*priceLevels)[priceLevelIdx].volumeTotal_ += newOrder.volumeLeft_ - order.volumeLeft_;
}

void OBProcessor::removeMatchedOrders_ThrdUnsafe(const TradeRecord &trade, 
                MDLevel2Record &askUpdate, MDLevel2Record &bidUpdate)
{
    /// find bid order
    int bidPriceLevelIdx = calculatePriceLevelIdx_ThrdUnsafe(trade.bidPrice_);
    PriceLevelsT *bidPriceLevels = &orderBook_.bidLevels_;    
    if((*bidPriceLevels)[bidPriceLevelIdx].volumeTotal_ < trade.matchedVolume_)
        throw std::logic_error("OBProcessor::removeMatchedOrders_ThrdUnsafe: bid price level hasn't enough volume!");

    OrderParamsT &bidOrders = (*bidPriceLevels)[bidPriceLevelIdx].orders_;
    OrderParamsT::iterator bidItEnd = bidOrders.end();
    OrderParamsT::iterator bidIt = bidOrders.begin();
    for(; bidIt != bidItEnd; ++bidIt){
        if(trade.bidOrderId_ == (*bidIt).orderId_)
            break;
    }
    if(bidItEnd == bidIt)
        throw std::logic_error("OBProcessor::removeMatchedOrders_ThrdUnsafe: no bid order found at price level!");
    if(trade.matchedVolume_ > (*bidIt).volume_)
        throw std::logic_error("OBProcessor::removeMatchedOrders_ThrdUnsafe: bid order has lower volume than matched!");
        
    /// find ask order
    int askPriceLevelIdx = calculatePriceLevelIdx_ThrdUnsafe(trade.askPrice_);
    PriceLevelsT *askPriceLevels = &orderBook_.askLevels_;    
    if((*askPriceLevels)[askPriceLevelIdx].volumeTotal_ < trade.matchedVolume_)
        throw std::logic_error("OBProcessor::removeMatchedOrders_ThrdUnsafe: ask price level hasn't enough volume!");
    
    OrderParamsT &askOrders = (*askPriceLevels)[askPriceLevelIdx].orders_;
    OrderParamsT::iterator askItEnd = askOrders.end();
    OrderParamsT::iterator askIt = askOrders.begin();
    for(; askIt != askItEnd; ++askIt){
        if(trade.askOrderId_ == (*askIt).orderId_)
            break;
    }
    if(askItEnd == askIt)
        throw std::logic_error("OBProcessor::removeMatchedOrders_ThrdUnsafe: no ask order found at price level!");
    if(trade.matchedVolume_ > (*askIt).volume_)
        throw std::logic_error("OBProcessor::removeMatchedOrders_ThrdUnsafe: ask order has lower volume than matched!");
        
    /// update or erase bid/ask orders
    if(trade.matchedVolume_ < (*askIt).volume_){
        askUpdate.prevVolume_ = (*askIt).volume_;
        askUpdate.prevPrice_ = trade.askPrice_;
        
        (*askIt).volume_ -= trade.matchedVolume_;
        askUpdate.volume_ = (*askIt).volume_;
        askUpdate.type_ = change_MDLevelChangeType;
    }else{
        askOrders.erase(askIt);
        askUpdate.type_ = delete_MDLevelChangeType;        
    }
    if(trade.matchedVolume_ < (*bidIt).volume_){
        bidUpdate.prevVolume_ = (*bidIt).volume_;
        bidUpdate.prevPrice_ = trade.bidPrice_;
        
        (*bidIt).volume_ -= trade.matchedVolume_;
        bidUpdate.volume_ = (*bidIt).volume_;
        bidUpdate.type_ = change_MDLevelChangeType;
    }else{
        bidOrders.erase(bidIt);
        bidUpdate.type_ = delete_MDLevelChangeType;        
    }
    
    (*askPriceLevels)[askPriceLevelIdx].volumeTotal_ -= trade.matchedVolume_;
    (*bidPriceLevels)[bidPriceLevelIdx].volumeTotal_ -= trade.matchedVolume_;
    
    /// update top ask price, if required
    if((orderBook_.topAskPrice_ == trade.askPrice_) && 
       (0 == (*askPriceLevels)[askPriceLevelIdx].volumeTotal_)){
        for(size_t i = askPriceLevelIdx - 1; i > 0; --i){
            if(0 < (*askPriceLevels)[i].volumeTotal_){
                orderBook_.topAskPrice_ = (*askPriceLevels)[i].price_;
                break;
            }
        }
    }
    
    /// update top bid price, if required
    if((orderBook_.topBidPrice_ == trade.bidPrice_) && 
       (0 == (*bidPriceLevels)[bidPriceLevelIdx].volumeTotal_)){
        for(size_t i = bidPriceLevelIdx + 1; i < bidPriceLevels->size(); ++i){
            if(0 < (*bidPriceLevels)[i].volumeTotal_){
                orderBook_.topBidPrice_ = (*bidPriceLevels)[i].price_;
                break;
            }
        }
    }
   
}


void OBProcessor::onNewOrder(const OrderBookRecord &order)
{
    if(!started_)
        throw std::logic_error("OBProcessor::onNewOrder: processor is stopped!");
    if(order.instrumentId_ != orderBook_.instrumentId_)
        throw std::logic_error("OBProcessor::onNewOrder: order was dispatched to the invalid processor(according to the instrumentId)!");
        
    MDLevel2Record l2Update(add_MDLevelChangeType, order.instrumentId_, 
                    order.price_, order.volumeLeft_, order.side_);
    {
        std::lock_guard<std::mutex> grd(lock_);
        addOrder_ThrdUnsafe(order);
    }
    obs_.load()->onOBookChanged(l2Update);
}

void OBProcessor::onChangeOrder(const OrderBookRecord &oldOrder, const OrderBookRecord &newOrder)
{
    if(!started_)
        throw std::logic_error("OBProcessor::onChangeOrder: processor is stopped!");
    
    if(oldOrder.instrumentId_ != orderBook_.instrumentId_)
        throw std::logic_error("OBProcessor::onChangeOrder: order was dispatched to the invalid processor(according to the instrumentId)!");
        
    MDLevel2Record l2Update(change_MDLevelChangeType, newOrder.instrumentId_, 
                    newOrder.price_, oldOrder.price_, newOrder.volumeLeft_, 
                    oldOrder.volumeLeft_, oldOrder.side_);
    {
        std::lock_guard<std::mutex> grd(lock_);
        
        if(newOrder.price_ != oldOrder.price_){
            /// delete order at old price level and add to new price level
            /// it incude changes in order's volume
            removeOrder_ThrdUnsafe(oldOrder);
            addOrder_ThrdUnsafe(newOrder);
        }else if(newOrder.volumeLeft_ != oldOrder.volumeLeft_){
            /// order's volume changed for the same price level
            changeOrderVolume_ThrdUnsafe(oldOrder, newOrder);
        }else
            throw std::logic_error("OBProcessor::onChangeOrder: this change of the order is not supported (price and volume only)!");
    }
    obs_.load()->onOBookChanged(l2Update);
}

void OBProcessor::onDeleteOrder(const OrderBookRecord &order)
{
    if(!started_)
        throw std::logic_error("OBProcessor::onDeleteOrder: processor is stopped!");
    
    if(order.instrumentId_ != orderBook_.instrumentId_)
        throw std::logic_error("OBProcessor::onDeleteOrder: order was dispatched to the invalid processor(according to the instrumentId)!");
        
    MDLevel2Record l2Update(delete_MDLevelChangeType, order.instrumentId_, 
                order.price_, order.volumeLeft_, order.side_);
    {
        std::lock_guard<std::mutex> grd(lock_);
        removeOrder_ThrdUnsafe(order);
    }
    obs_.load()->onOBookChanged(l2Update);
}

void OBProcessor::onDeleteOrders(const std::deque<OrderBookRecord> &orders)
{
    if(!started_)
        throw std::logic_error("OBProcessor::onDeleteOrders: processor is stopped!");
    
    {
        std::lock_guard<std::mutex> grd(lock_);
        for(auto it: orders){
            removeOrder_ThrdUnsafe(it);
        }
    }
}

void OBProcessor::onTrade(const TradeRecord &trade)
{
    if(!started_)
        throw std::logic_error("OBProcessor::onTrade: processor is stopped!");
    
    MDLevel2Record askL2Update;
    askL2Update.price_ = trade.askPrice_;
    askL2Update.side_ = Side::sell_Side;
    
    MDLevel2Record bidL2Update;
    bidL2Update.price_ = trade.bidPrice_;
    bidL2Update.side_ = Side::buy_Side;
    
    {
        std::lock_guard<std::mutex> grd(lock_);
        removeMatchedOrders_ThrdUnsafe(trade, askL2Update, bidL2Update);
    }
    obs_.load()->onOBooksChanged(askL2Update, bidL2Update);
}

void OBProcessor::getOBSnapshot(size_t maxLevels, size_t maxRecordsPerLevel, PriceLevelsT &bidPriceLevels, 
        PriceLevelsT &askPriceLevels)const
{
    if(!started_)
        throw std::logic_error("OBProcessor::getOBSnapshot: processor is stopped!");
    
    bidPriceLevels.clear();
    askPriceLevels.clear();
    {
        std::lock_guard<std::mutex> grd(lock_);    
        
        int bidPriceLevelIdx = calculatePriceLevelIdx_ThrdUnsafe(orderBook_.topBidPrice_);

        size_t count = orderBook_.bidLevels_.size();
        for(size_t i = bidPriceLevelIdx; i > 0; --i){
            if(0 < orderBook_.bidLevels_[i].volumeTotal_){
                PriceLevel lvl;
                lvl.price_ = orderBook_.bidLevels_[i].price_;
                lvl.volumeTotal_ = orderBook_.bidLevels_[i].volumeTotal_;
                OrderParamsT::const_iterator fstIt = orderBook_.bidLevels_[i].orders_.begin();
                OrderParamsT::const_iterator lastIt = 
                            (maxRecordsPerLevel < orderBook_.bidLevels_[i].orders_.size())?
                                    (fstIt + maxRecordsPerLevel):
                                    (orderBook_.bidLevels_[i].orders_.end());
                lvl.orders_.assign(fstIt, lastIt);
                bidPriceLevels.push_back(std::move(lvl));
                
                if(maxLevels <= bidPriceLevels.size())
                    break;
            }
        }

        int askPriceLevelIdx = calculatePriceLevelIdx_ThrdUnsafe(orderBook_.topAskPrice_);
        count = orderBook_.askLevels_.size();
        for(size_t i = askPriceLevelIdx; i < count; ++i){
            if(0 < orderBook_.askLevels_[i].volumeTotal_){
                PriceLevel lvl;
                lvl.price_ = orderBook_.askLevels_[i].price_;
                lvl.volumeTotal_ = orderBook_.askLevels_[i].volumeTotal_;
                OrderParamsT::const_iterator fstIt = orderBook_.askLevels_[i].orders_.begin();
                OrderParamsT::const_iterator lastIt = 
                            (maxRecordsPerLevel < orderBook_.askLevels_[i].orders_.size())?
                                    (fstIt + maxRecordsPerLevel):
                                    (orderBook_.askLevels_[i].orders_.end());
                lvl.orders_.assign(fstIt, lastIt);
                askPriceLevels.push_back(std::move(lvl));
                
                if(maxLevels <= askPriceLevels.size())
                    break;
            }
        }

    }
}