#ifndef ORDERBOOKDISPATCHER_H
#define ORDERBOOKDISPATCHER_H

#include "OrderBookDef.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <boost/thread/shared_mutex.hpp>
#pragma GCC diagnostic pop

#include <map>

namespace oli
{

namespace obook
{
   
/// encapsulates orderbooks per instrument and dispatches 
/// incoming order events to the corresponding OrderBookProcessor
class OrderBookDispatcher : public IInOrderBookMessageObserver, 
            public secmaster::IInstrumentEventObserver
{
public:
    OrderBookDispatcher(IOBookProcessorFactory *fact, 
                secmaster::IInstrumentCache *instrCache);
    ~OrderBookDispatcher();
    
    void attach(IOrderBookEventObserver *obs);
    void detach();

public:
    /// implementation of IInOrderBookMessageObserver
    virtual void onNewOrder(const OrderBookRecord &order) override;
    virtual void onChangeOrder(const OrderBookRecord &oldOrder, 
                               const OrderBookRecord &newOrder) override;
    virtual void onDeleteOrder(const OrderBookRecord &order) override;
    virtual void onDeleteOrders(const std::deque<OrderBookRecord> &orders) override;
    virtual void onGetOrderBookSnapshot(const oli::idT &instrId, 
                    size_t maxLevels, size_t maxRecordsPerLevel, 
                    PriceLevelsT &bidPriceLevels, PriceLevelsT &askPriceLevels) override;
    virtual void onTrade(const oli::idT &instrId, const TradeRecord &trade) override;

public:
    /// implementation of secmaster::IInstrumentEventObserver
    virtual void onAddInstrument(const secmaster::InstrumentRecord &instr) override;
    virtual void onRemoveInstrument(const secmaster::InstrumentRecord &instr) override;
    
private:
    mutable boost::shared_mutex lock_;
    typedef std::map<oli::idT, IOBookEventProcessor*> OBProcessorsByIntrumentT;
    OBProcessorsByIntrumentT obProcessors_;
    
    IOrderBookEventObserver *obs_;
    
    IOBookProcessorFactory *fact_;
    
    secmaster::IInstrumentCache *instrCache_;
    
protected: /// protected to call from tests
    IOBookEventProcessor* findProcessor(const oli::idT &instrId)const;
};

}

}

#endif // ORDERBOOKDISPATCHER_H
