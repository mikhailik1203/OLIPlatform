/*
 * OrderBookDef.h
 *
 *  Created on: Oct 31, 2016
 *      Author: sam1203
 */
 
#ifndef ORDERBOOKDEF_H
#define ORDERBOOKDEF_H

#include "OLI_UtilsDefines.h"
#include "OLI_FinanceDomain.h"
#include "SecMasterDef.h"

#include <deque>
#include <vector>
#include <map>

namespace oli
{

    namespace msg{
        class MessageBase;
    }
    
namespace obook
{
    /// content of the NewOrder, UpdateOrder or DeleteOrder messages
    struct OrderBookRecord{
        oli::idT orderId_;
        oli::idT instrumentId_;
        oli::priceT price_;
        oli::quantityT volumeLeft_;
        oli::Side side_;
        
        OrderBookRecord(): 
            orderId_(oli::INVALID_ID), instrumentId_(oli::INVALID_ID), price_(0),
            volumeLeft_(0), side_(oli::Side::invalid_Side)
        {}
        
        OrderBookRecord(const oli::idT &orderId, const oli::idT &instrumentId, 
                        const oli::priceT &price, const oli::quantityT &volume,
                        const oli::Side &side): 
            orderId_(orderId), instrumentId_(instrumentId), price_(price),
            volumeLeft_(volume), side_(side)
        {}
        
    };
    
    /// content of the OrderMatch message
    struct TradeRecord{
        oli::idT bidOrderId_;
        oli::idT askOrderId_;
        oli::priceT bidPrice_;
        oli::priceT askPrice_;
        oli::quantityT matchedVolume_;
        
        TradeRecord(): 
            bidOrderId_(oli::INVALID_ID), askOrderId_(oli::INVALID_ID), bidPrice_(0),
            askPrice_(0), matchedVolume_(0)
        {}
        TradeRecord(const oli::idT &bidOrderId, const oli::priceT &bidPrice, 
                    const oli::idT &askOrderId, const oli::priceT &askPrice,
                    const oli::quantityT &matchedVolume): 
            bidOrderId_(bidOrderId), askOrderId_(askOrderId), bidPrice_(bidPrice),
            askPrice_(askPrice), matchedVolume_(matchedVolume)
        {}
        
    };
    
    /// entry of the order book
    struct OrderParams{
        OrderParams(): orderId_(oli::INVALID_ID), volume_(0){}
        OrderParams(const oli::idT &orderId, const oli::quantityT &volume): orderId_(orderId), volume_(volume){}
        explicit OrderParams(const OrderBookRecord &order): orderId_(order.orderId_), volume_(order.volumeLeft_){}
        
        oli::idT orderId_;
        oli::quantityT volume_;
        
        bool operator==(const OrderParams &ordPar){
            return orderId_ == ordPar.orderId_ && volume_ == ordPar.volume_;
        }
        
    };
    typedef std::deque<OrderParams> OrderParamsT;
    
    /// entry of the price level - contains all orders related to this level
    struct PriceLevel{
        oli::priceT price_;
        oli::quantityT volumeTotal_;
        OrderParamsT orders_;
        
        PriceLevel(): price_(0), volumeTotal_(0), orders_()
        {}
        
    };
    typedef std::vector<PriceLevel> PriceLevelsT;
    typedef std::map<oli::priceT, PriceLevel> PriceLevelsMapT;
    
    /// OrderBook parameters
    struct OrderBookForInstrument{
    public:
        oli::idT instrumentId_;
        oli::priceT topBidPrice_;
        oli::priceT topAskPrice_;
       
        PriceLevelsT bidLevels_;
        PriceLevelsT askLevels_;
        
        OrderBookForInstrument(): instrumentId_(oli::INVALID_ID), topBidPrice_(0), 
            topAskPrice_(0), bidLevels_(), askLevels_(0)
        {}
    };
    

    /// interface to process incoming OrderBook messages
    class IInOrderBookMessageObserver{
    public:
        virtual ~IInOrderBookMessageObserver(){}
        /// callback is called when NewOrder message received
        virtual void onNewOrder(const OrderBookRecord &order) = 0;
        
        /// callback is called when UpdateOrder message received
        virtual void onChangeOrder(const OrderBookRecord &oldOrder, const OrderBookRecord &newOrder) = 0;
        
        /// callback is called when DeleteOrder message received
        virtual void onDeleteOrder(const OrderBookRecord &order) = 0;
        
        /// callback is called when MassDeleteOrders message received
        virtual void onDeleteOrders(const std::deque<OrderBookRecord> &orders) = 0;
        
        /// callback is called when getOBSnapshot message received
        virtual void onGetOrderBookSnapshot(const oli::idT &instrId, size_t maxLevels, 
                        size_t maxRecordsPerLevel, PriceLevelsT &bidPriceLevels, 
                        PriceLevelsT &askPriceLevels) = 0;
        
        /// callback is called when new OrderMatch message is received
        virtual void onTrade(const oli::idT &instrId, const TradeRecord &trade) = 0;
    };
   
    /// interface to process incoming Order changes
    class IOBookEventProcessor{
    public:
        virtual ~IOBookEventProcessor(){}
        /// callback is called when NewOrder message received
        virtual void onNewOrder(const OrderBookRecord &order) = 0;
        
        /// callback is called when UpdateOrder message received
        virtual void onChangeOrder(const OrderBookRecord &oldOrder, const OrderBookRecord &newOrder) = 0;
        
        /// callback is called when DeleteOrder message received
        virtual void onDeleteOrder(const OrderBookRecord &order) = 0;
        
        /// callback is called when MassDeleteOrders message received
        virtual void onDeleteOrders(const std::deque<OrderBookRecord> &orders) = 0;
        
        /// callback is called when this processor needs to be stopped (Instrument halted)
        virtual void stopProcessing() = 0;

        /// callback is called when this processor needs to be started (Instrument restored)
        virtual void startProcessing(const secmaster::InstrumentRecord &instr) = 0;
        
        /// callback is called when new OrderMatch message is received
        virtual void onTrade(const TradeRecord &trade) = 0;
        
        /// method returns snpshot of the OrderBook (bid and ask)
        virtual void getOBSnapshot(size_t maxLevels, size_t maxRecordsPerLevel, PriceLevelsT &bidPriceLevels, PriceLevelsT &askPriceLevels)const = 0;
        
    };

    /// factory interface for creation IOBookEventProcessor
    class IOBookProcessorFactory{
    public:
        virtual ~IOBookProcessorFactory(){}
        
        virtual IOBookEventProcessor *create()const = 0;
    };
    
    enum MDLevelChangeType{
        invalid_MDLevelChangeType = 0,
        add_MDLevelChangeType,
        change_MDLevelChangeType,
        delete_MDLevelChangeType
    };
    
    struct MDLevel2Record{
    public:
        MDLevel2Record():
            instrumId_(oli::INVALID_ID), price_(0), volume_(0), 
            side_(oli::Side::invalid_Side), type_(invalid_MDLevelChangeType),
            prevPrice_(0), prevVolume_(0)
        {}
        
        MDLevel2Record(MDLevelChangeType type, oli::idT instrumId, oli::priceT price, 
                       oli::quantityT volume, oli::Side side):
            instrumId_(instrumId), price_(price), volume_(volume), 
            side_(side), type_(type), prevPrice_(0), prevVolume_(0)
        {}
        
        MDLevel2Record(MDLevelChangeType type, oli::idT instrumId, oli::priceT newPrice, oli::priceT oldPrice, 
                       oli::quantityT newVolume, oli::quantityT oldVolume, 
                       oli::Side side):
            instrumId_(instrumId), price_(newPrice), volume_(newVolume), 
            side_(side), type_(type), prevPrice_(oldPrice), prevVolume_(oldVolume)
        {}
        
        oli::idT instrumId_;
        oli::priceT price_; /// price level
        oli::quantityT volume_; /// volume
        oli::Side side_; /// side of L2
        MDLevelChangeType type_;
        oli::priceT prevPrice_; /// prev price level, in case of change
        oli::quantityT prevVolume_; /// prev volume, in case of change
        oli::OrderType ordType_;
    };
    
    /// interface for outgoing OBook events
    class IOrderBookEventObserver{
    public:
        virtual ~IOrderBookEventObserver(){}
        
        /// callback is called by orderbook when it changed
        virtual void onOrderToMatch(const MDLevel2Record &order) = 0;
        virtual void onOBookChanged(const MDLevel2Record &order) = 0;
        virtual void onOBooksChanged(const MDLevel2Record &askUpdate, 
                                     const MDLevel2Record &bidUpdate) = 0;
        virtual void onOBooksReset(const oli::idT &instrId) = 0;                                     
    };    
    
    /// interface for OrderBook container
    class IOrderBook{
    public:
        virtual ~IOrderBook(){}
        
    };    
    
}
}

#endif    