#include "OrderBookDispatcher.h"

using namespace oli::obook;

OrderBookDispatcher::OrderBookDispatcher(IOBookProcessorFactory *fact, 
            secmaster::IInstrumentCache *instrCache): 
    obs_(nullptr), fact_(fact), instrCache_(instrCache)
{
    instrCache_->attach(this);
}

OrderBookDispatcher::~OrderBookDispatcher()
{
    instrCache_->dettach(this);
    
    OBProcessorsByIntrumentT tmpProcessors;
    {
        boost::unique_lock<boost::shared_mutex> lock(lock_);
        std::swap(tmpProcessors, obProcessors_);
    }
    for(auto it : tmpProcessors){
        std::unique_ptr<IOBookEventProcessor> proc(it.second);
        proc->stopProcessing();
    }
    
    instrCache_ = nullptr;
    fact_ = nullptr;
    obs_ = nullptr;
}

void OrderBookDispatcher::attach(IOrderBookEventObserver *obs)
{
    obs_ = obs;
}

void OrderBookDispatcher::detach()
{
    obs_ = nullptr;
}

void OrderBookDispatcher::onAddInstrument(const secmaster::InstrumentRecord &instr)
{
    boost::unique_lock<boost::shared_mutex> lock(lock_);
    OBProcessorsByIntrumentT::iterator instOBIt = obProcessors_.find(instr.instrId_);
    if(obProcessors_.end() == instOBIt){
        std::unique_ptr<IOBookEventProcessor> proc(fact_->create());
        instOBIt = obProcessors_.insert(
                    OBProcessorsByIntrumentT::value_type(
                            instr.instrId_, proc.get())).first;
        proc.release();
    }
    instOBIt->second->startProcessing(instr);
}

void OrderBookDispatcher::onRemoveInstrument(const secmaster::InstrumentRecord &instr)
{
    IOBookEventProcessor* proc = nullptr;
    {
        boost::shared_lock<boost::shared_mutex> lock(lock_);
        OBProcessorsByIntrumentT::const_iterator instOBIt = obProcessors_.find(instr.instrId_);
        if(obProcessors_.end() != instOBIt){
            proc = instOBIt->second;
        }
    }
    if(nullptr != proc)
        proc->stopProcessing();
}

IOBookEventProcessor* OrderBookDispatcher::findProcessor(const oli::idT &instrId)const
{
    boost::shared_lock<boost::shared_mutex> lock(lock_);
    OBProcessorsByIntrumentT::const_iterator instOBIt = obProcessors_.find(instrId);
    if(obProcessors_.end() == instOBIt)
        throw std::logic_error("OrderBookDispatcher::findProcessor: unable to find processor by instrument!");
    return instOBIt->second;
}

void OrderBookDispatcher::onNewOrder(const OrderBookRecord &order)
{
    IOBookEventProcessor* processor = findProcessor(order.instrumentId_);
    processor->onNewOrder(order);
}

void OrderBookDispatcher::onChangeOrder(const OrderBookRecord &oldOrder, const OrderBookRecord &updatedOrder)
{
    IOBookEventProcessor* processor = findProcessor(oldOrder.instrumentId_);
    processor->onChangeOrder(oldOrder, updatedOrder);
}

void OrderBookDispatcher::onDeleteOrder(const OrderBookRecord &order)
{
    IOBookEventProcessor* processor = findProcessor(order.instrumentId_);
    processor->onDeleteOrder(order);
}

void OrderBookDispatcher::onDeleteOrders(const std::deque<OrderBookRecord> &orders)
{
    throw std::logic_error("OrderBookDispatcher::onDeleteOrders: not implemented!");
}

void OrderBookDispatcher::onGetOrderBookSnapshot(const oli::idT &instrId, 
            size_t maxLevels, size_t maxRecordsPerLevel, PriceLevelsT &bidPriceLevels, 
            PriceLevelsT &askPriceLevels)
{
    IOBookEventProcessor* processor = findProcessor(instrId);
    processor->getOBSnapshot(maxLevels, maxRecordsPerLevel, bidPriceLevels, 
                        askPriceLevels);
} 

void OrderBookDispatcher::onTrade(const oli::idT &instrId, const TradeRecord &trade) 
{
    IOBookEventProcessor* processor = findProcessor(instrId);
    processor->onTrade(trade);
}
