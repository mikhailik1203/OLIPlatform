#include "InMessageWrapper.h"
#include "src/protobuf/oli_messageslib.pb.h"

using namespace oli::msg;

namespace{
    template<typename T, typename Proc>
    void processMessage(oli::ConnectionIdT connectionId, Proc &proc, const std::string &msgbody)
    {
        T msg;
        msg.ParseFromString(msgbody);
        proc.onMessage(connectionId, &msg);
    }

}

InMessageWrapper::InMessageWrapper(const char *msg, size_t size)
{
    MessageHeaderData data;
    data.ParseFromArray(msg, static_cast<int>(size));
    if(PROTOCOL_MAJOR_VERSION != data.protocolversionmajor())
        throw std::logic_error("InMessageWrapper(): incoming message has invalid major version of Protocol!");
    if(PROTOCOL_MINOR_VERSION != data.protocolversionminor())
        throw std::logic_error("InMessageWrapper(): incoming message has invalid minor version of Protocol!");
        
    msgType_ = data.type();
    msgSeqNum_ = data.msgsequenceid();
    channelId_ = data.channelid();
    msgBody_ = data.messagedata();
}

InMessageWrapper::~InMessageWrapper()
{
    
}

bool InMessageWrapper::isSessionLevelMsg()const
{
    return msgType_ < topSessionLevelMsg_messageType;
}

void InMessageWrapper::handleSsnMessage(ConnectionIdT connId, SsnMessageProcessor &proc)const
{
    switch(msgType_){
        case SsnLogonMsg_messageType:
            processMessage<SsnLogonMsg>(connId, proc, msgBody_);
            break;
        case SsnHeartBeatMsg_messageType:
            processMessage<SsnHeartBeatMsg>(connId, proc, msgBody_);
            break;
        case SsnResendRequestMsg_messageType:
            processMessage<SsnResendRequestMsg>(connId, proc, msgBody_);
            break;
        case SsnSequenceChangeMsg_messageType:
            processMessage<SsnSequenceChangeMsg>(connId, proc, msgBody_);
            break;
        case SsnLogoutMsg_messageType:
            processMessage<SsnLogoutMsg>(connId, proc, msgBody_);
            break;
        case SsnRejectMsg_messageType:
            processMessage<SsnRejectMsg>(connId, proc, msgBody_);
            break;
        case SsnRetransmittedMsg_messageType:
            processMessage<SsnRetransmittedMsg>(connId, proc, msgBody_);
            break;
        case invalid_messageType:
        default:
            throw std::logic_error("InMessageWrapper::handleSsnMessage: unable to decode message!");
    };
}

void InMessageWrapper::handleAppMessage(ConnectionIdT connId, AppMessageProcessor &proc)const
{
    switch(msgType_){
        /// client messages 100-599
    case clntNewOrderRequest_messageType:
        processMessage<ClntNewOrderRequestMsg>(connId, proc, msgBody_);
        break;
    case clntOrderReplaceRequest_messageType:
        processMessage<ClntCancelOrderRequestMsg>(connId, proc, msgBody_);
        break;
    case clntOrderCancelRequest_messageType:
        processMessage<ClntReplaceOrderRequestMsg>(connId, proc, msgBody_);
        break;
        
        /// orderBook messages 600-799
    case obCancelOrder_messageType:
        break;
    case obCancelAllOrders_messageType:
        break;
        
        /// limitChecker messages 800-999
    case lcOrderCancel_messageType:
        break;
    case lcCancelAllOrders_messageType:
        break;
        
        /// securityMaster messages 1000-1199
    case smSecurityData_messageType:
        break;
    case smSecGroupData_messageType:
        break;
    case smMarketData_messageType:
        break;
    case smMarketStatusUpdate_messageType:
        break;
        
        /// userManager messages 1200-1399
    case umAccountData_messageType:
        break;
    case umAccountStatusUpdate_messageType:
        break;
        
        /// matchingEngine messages 1400-1599
    case meOrderMatched_messageType:
        break;
        
        /// positionManager messages 1600-1799
    case pmNewOrderRequest_messageType:
        break;
        
        /// OMS messages 1800-1999
    case omsNewOrder_messageType:
        processMessage<OMSNewOrderMsg>(connId, proc, msgBody_);
        break;
    case omsOrderStatus_messageType:
        
        break;
    case omsCancelOrder_messageType:
        processMessage<OMSCancelOrderMsg>(connId, proc, msgBody_);
        break;
    case omsReplaceOrder_messageType:
        processMessage<OMSChangeOrderMsg>(connId, proc, msgBody_);
        break;
    case omsTradeReject_messageType:
        processMessage<OMSTradeRejectMsg>(connId, proc, msgBody_);
        break;
    case omsTradeCorrectReject_messageType:
        processMessage<OMSTradeCorrectRejectMsg>(connId, proc, msgBody_);
        break;

        /// OrderBlotter messages 2000-2199
    case obSubscription_messageType:
        break;
        
        /// MarketData messages 2200-2399
    case mdSubscription_messageType:
        break;
        
        /// MDRates messages 2400-2599
    case mdrSubscription_messageType:
        break;
        
        /// PlatformManagement messages 2600-2799
    case pmNotification_messageType:
        break;
        
        /// Orchestrator messages 2800-2999
    case oInstanceStop_messageType:
        break;
        
    case invalid_messageType:  
    default:
        throw std::logic_error("InMessageWrapper::handleMessage: unable to decode message!");
    };
}


