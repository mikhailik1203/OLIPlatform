#ifndef OBPROCESSOR_H
#define OBPROCESSOR_H

#include "OrderBookDef.h"

#include <mutex>
#include <atomic>

namespace oli
{

namespace obook
{

/// encapsulates queue of the order envets and process them 
class OBProcessor final : public IOBookEventProcessor 
{
public:
    OBProcessor();
    ~OBProcessor();

    void attach(IOrderBookEventObserver *obs);
    void detach();

public:
    /// implementation of IOBookEventProcessor
    virtual void onNewOrder(const OrderBookRecord &order) override;
    virtual void onChangeOrder(const OrderBookRecord &oldOrder, const OrderBookRecord &newOrder) override;
    virtual void onDeleteOrder(const OrderBookRecord &order) override;
    virtual void onDeleteOrders(const std::deque<OrderBookRecord> &orders) override;
    virtual void stopProcessing() override;
    virtual void startProcessing(const secmaster::InstrumentRecord &instr) override;
    virtual void onTrade(const TradeRecord &trade) override;
    virtual void getOBSnapshot(size_t maxLevels, size_t maxRecordsPerLevel, PriceLevelsT &bidPriceLevels, PriceLevelsT &askPriceLevels)const override;

private:
    mutable std::mutex lock_;
    
    std::atomic<IOrderBookEventObserver *> obs_;

    OrderBookForInstrument orderBook_;
    
    oli::priceT lowestPrice_;
    oli::priceT priceStep_;
    size_t priceLevelAmount_;
    
    std::atomic<bool> started_;
    
private:
    int calculatePriceLevelIdx_ThrdUnsafe(const oli::priceT &price)const;
    void addPriceLevels_ThrdUnsafe(int idx, PriceLevelsT *levels);
    void addOrder_ThrdUnsafe(const OrderBookRecord &order);
    void removeOrder_ThrdUnsafe(const OrderBookRecord &order);
    void changeOrderVolume_ThrdUnsafe(const OrderBookRecord &oldOrder, const OrderBookRecord &newOrder);
    void removeMatchedOrders_ThrdUnsafe(const TradeRecord &trade, MDLevel2Record &askUpdate, MDLevel2Record &bidUpdate);
    
};

}

}

#endif // OBPROCESSOR_H
