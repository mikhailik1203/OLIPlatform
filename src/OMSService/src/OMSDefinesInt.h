/*
 * OMSDefinesInt.h
 *
 *  Created on: Feb 24, 2016
 *      Author: sam1203
 */
 
#ifndef OMSDEFINESINT_H
#define OMSDEFINESINT_H

#include "OLI_UtilsDefines.h"

namespace oli
{
    
    namespace tl{
        class TransportLayer;
    }

namespace oms
{
    class OrderExpirationController;
    class OMSDataCache;
    class OMSStateMachine;
    
    /// contains entities required for event processing
    struct EventContext{
        OrderExpirationController *expireCtrlr_;
        OMSDataCache *dataCache_;
        tl::TransportLayer *transportLayer_;
        OMSStateMachine *stateMachine_;
    };
    
    
    enum Side{
        invalid_Side = 0,
        buy_Side,
        sell_Side
    };
    
    enum OrderType{
        invalid_orderType = 0,
        market_orderType,
        limit_orderType,
        stop_orderType
    };
    
    /// parameters of the event source
    struct RequestSource{
        /// name of the client, used to simplify Bridge functionality 
        std::string clientId_; 
        /// name of the client's session (if exists), used to simplify Bridge functionality
        std::string sessionId_; 
        /// channel name between Bridge and OMS
        std::string channelId_; 
    };
    
    struct Instrument{
        uint64T id_;
        std::string name_;        
    };
    
    /// contains order parameters
    struct OrderData{
        uint64T orderId_;
        std::string clientOrderId_;
        Side side_;
        OrderType type_;
        
        uint8T qtyDigits_; /// number of float digits for *Qty fields
        quantityT orderQty_; /// total qty of the order
        quantityT filledQty_; /// total filled qty 
        quantityT leftQty_; /// total open qty 
        uint8T priceDigits_; /// number of float digits for *Price fields
        priceT orderPrice_; /// for market is 0, for limit/stop - contains limit/stop price
        
        RequestSource source_; /// source of the original request
        Instrument instrument_;
        
        OrderData();
    };

    /// contains parameters of the order, used to cancel original active order
    struct OrderCancelData{
        uint64T orderId_;
        std::string clientOrderId_;

        uint64T orderIdOriginal_; // order to cancel
        std::string clientOrderIdOriginal_;
        
        RequestSource source_; /// source of the original request
        
        OrderCancelData();
    };

    /// contains paramters of the order, used to change original order
    struct OrderReplaceData{
        uint64T orderId_;
        std::string clientOrderId_;

        uint64T orderIdOriginal_; /// order to replace
        std::string clientOrderIdOriginal_;

        uint8T qtyDigits_; /// number of float digits for *Qty fields
        quantityT orderQtyOld_;
        quantityT orderQtyNew_;
        uint8T priceDigits_; /// number of float digits for *Price fields
        priceT orderPriceOld_;
        priceT orderPriceNew_;
        
        RequestSource source_; /// source of the original request
        
        OrderReplaceData();
    };
    
}
}

#endif