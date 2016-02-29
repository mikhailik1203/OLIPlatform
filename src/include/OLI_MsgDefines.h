/*
 * OLI_MsgDefines.h
 *
 *  Created on: Feb 24, 2016
 *      Author: sam1203
 */
 
#ifndef OLI_MSGDEFINES_H
#define OLI_MSGDEFINES_H

#include <string>
#include <vector>

#include "OLI_UtilsDefines.h"

namespace oli
{
namespace msg
{
    const std::string MSG_PROTOCOL = "0.0.0.1";
    
    /// types of the transport messages
    enum MessageType{
        invalid_messageType = 0,
        /// system messages 1-99
        /// client messages 100-599
        clntNewOrderRequest_messageType = 100,
        clntOrderReplaceRequest_messageType = 101,
        clntOrderCancelRequest_messageType = 102,
        /// orderBook messages 600-799
        obCancelOrder_messageType = 600,
        obCancelAllOrders_messageType = 601,
        /// limitChecker messages 800-999
        lcOrderCancel_messageType = 800,
        lcCancelAllOrders_messageType = 801,
        /// securityMaster messages 1000-1199
        smSecurityData_messageType = 1000,
        smSecGroupData_messageType = 1001,
        smMarketData_messageType = 1002,
        smMarketStatusUpdate_messageType = 1003,
        /// userManager messages 1200-1399
        umAccountData_messageType = 1200,
        umAccountStatusUpdate_messageType = 1201,
        /// matchingEngine messages 1400-1599
        meOrderMatched_messageType = 1400,
        /// positionManager messages 1600-1799
        pmNewOrderRequest_messageType = 1600,
        /// OMS messages 1800-1999
        omsNewOrder_messageType = 1800,
        omsOrderStatus_messageType = 1801,
        omsCancelOrder_messageType = 1802,
        omsReplaceOrder_messageType = 1803,
        /// OrderBlotter messages 2000-2199
        obSubscription_messageType = 2000,
        /// MarketData messages 2200-2399
        mdSubscription_messageType = 2200,
        /// MDRates messages 2400-2599
        mdrSubscription_messageType = 2400,
        /// PlatformManagement messages 2600-2799
        pmNotification_messageType = 2600,
        /// Orchestrator messages 2800-2999
        oInstanceStop_messageType = 2800
    };
    
    /// base class for transport messages
    class MessageBase
    {
    public:
        MessageBase();
        ~MessageBase();

        /// returns message type
        MessageType msgType()const;
        
        /// returns version of the message
        std::string msgVersion()const;
    
        /// returns message data
        const std::vector<int8T> &message()const;
    private:
        MessageType type_;
        std::string version_;
        std::vector<int8T> message_;
    };
    
    
}
}

#endif // OLI_MSGDEFINES_H
