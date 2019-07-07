#include "OutMessageWrapper.h"
#include "src/protobuf/oli_messageslib.pb.h"

#include <google/protobuf/util/json_util.h>

using namespace oli::msg;

namespace{
   
}

OutMessageWrapper::OutMessageWrapper(int type, const std::string &data)
{
    data_ = data;
    type_ = type;
    connId_ = 0;
}

void OutMessageWrapper::setDestinationId(ConnectionIdT connId)const
{
    connId_ = connId;
}

oli::ConnectionIdT OutMessageWrapper::destinationId()const
{
    return connId_;
}


OutMessageWrapper::OutMessageWrapper(const SsnResendRequestMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(SsnResendRequestMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(SsnResendRequestMsg): message is not fully initialised!");
    msg->SerializeToString(&data_);
    type_ = SsnResendRequestMsg_messageType;
}

OutMessageWrapper::OutMessageWrapper(const SsnLogonMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(SsnLogonMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(SsnLogonMsg): message is not fully initialised!");
    msg->SerializeToString(&data_);
    type_ = SsnLogonMsg_messageType;
}


OutMessageWrapper::OutMessageWrapper(const SsnHeartBeatMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(SsnHeartBeatMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(SsnHeartBeatMsg): message is not fully initialised!");
    msg->SerializeToString(&data_);
    type_ = SsnHeartBeatMsg_messageType;
}

OutMessageWrapper::OutMessageWrapper(const SsnRejectMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(SsnRejectMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(SsnRejectMsg): message is not fully initialised!");
    msg->SerializeToString(&data_);
    type_ = SsnRejectMsg_messageType;
}

OutMessageWrapper::OutMessageWrapper(const SsnRetransmittedMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(SsnRetransmittedMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(SsnRetransmittedMsg): message is not fully initialised!");
    msg->SerializeToString(&data_);
    type_ = SsnRetransmittedMsg_messageType;
}

OutMessageWrapper::OutMessageWrapper(const ClntNewOrderRequestMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(ClntNewOrderRequestMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(ClntNewOrderRequestMsg): message is not fully initialised!");
    msg->SerializeToString(&data_);
    type_ = clntNewOrderRequest_messageType;
}
    
OutMessageWrapper::OutMessageWrapper(const ClntCancelOrderRequestMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(ClntCancelOrderRequestMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(ClntCancelOrderRequestMsg): message is not fully initialised!");

    msg->SerializeToString(&data_);
    type_ = clntOrderCancelRequest_messageType;
}

OutMessageWrapper::OutMessageWrapper(const ClntReplaceOrderRequestMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(ClntReplaceOrderRequestMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(ClntReplaceOrderRequestMsg): message is not fully initialised!");
    
    msg->SerializeToString(&data_);
    type_ = clntOrderReplaceRequest_messageType;
}

OutMessageWrapper::OutMessageWrapper(const OMSNewOrderMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(OMSNewOrderMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(OMSNewOrderMsg): message is not fully initialised!");
    
    msg->SerializeToString(&data_);
    type_ = omsNewOrder_messageType;
}

OutMessageWrapper::OutMessageWrapper(const OMSChangeOrderMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(OMSChangeOrderMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(OMSChangeOrderMsg): message is not fully initialised!");
    
    msg->SerializeToString(&data_);
    type_ = omsReplaceOrder_messageType;
}

OutMessageWrapper::OutMessageWrapper(const OMSCancelOrderMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(OMSCancelOrderMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(OMSCancelOrderMsg): message is not fully initialised!");
    
    msg->SerializeToString(&data_);
    type_ = omsCancelOrder_messageType;
}

OutMessageWrapper::OutMessageWrapper(const OMSOrderStatusMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(OMSOrderStatusMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(OMSOrderStatusMsg): message is not fully initialised!");
    
    msg->SerializeToString(&data_);
    type_ = omsOrderStatus_messageType;
}

OutMessageWrapper::OutMessageWrapper(const OMSTradeRejectMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(OMSTradeRejectMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(OMSTradeRejectMsg): message is not fully initialised!");

    msg->SerializeToString(&data_);
    type_ = omsTradeReject_messageType;
}

OutMessageWrapper::OutMessageWrapper(const OMSTradeCorrectRejectMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(OMSTradeCorrectRejectMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(OMSTradeCorrectRejectMsg): message is not fully initialised!");

    msg->SerializeToString(&data_);
    type_ = omsTradeCorrectReject_messageType;
}

OutMessageWrapper::OutMessageWrapper(const OBBookChangeMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(OBBookChangeMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(OBBookChangeMsg): message is not fully initialised!");
    
    msg->SerializeToString(&data_);
    type_ = obBookChange_messageType;
}

OutMessageWrapper::OutMessageWrapper(const OBBookChangesMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(OBBookChangesMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(OBBookChangesMsg): message is not fully initialised!");
    
    msg->SerializeToString(&data_);
    type_ = obBookChanges_messageType;
}

OutMessageWrapper::OutMessageWrapper(const MEOrdersMatchedMsg *msg)
{
    if(nullptr == msg)
        throw std::logic_error("OutMessageWrapper(MEOrdersMatchedMsg): message is NULL!");
    if(!msg->IsInitialized())
        throw std::logic_error("OutMessageWrapper(MEOrdersMatchedMsg): message is not fully initialised!");
    
    msg->SerializeToString(&data_);
    type_ = meOrderMatched_messageType;
}

OutMessageWrapper::~OutMessageWrapper()
{
    
}

int OutMessageWrapper::messageType()const
{
    return type_;
}

const std::string &OutMessageWrapper::serialise(const std::string &clientName, 
            const std::string &ssnName, const std::string &channelName, oli::int64T seqId)const
{
    MessageHeaderData header;
    header.set_protocolversionmajor(PROTOCOL_MAJOR_VERSION);
    header.set_protocolversionminor(PROTOCOL_MINOR_VERSION);
    header.set_type(static_cast<MessageType>(type_));
    header.set_messagedata(data_);
    header.set_clientname(clientName);
    header.set_sessionname(ssnName);
    header.set_channelname(channelName);
    header.set_msgsequenceid(seqId);
    
    if(!header.IsInitialized())
        throw std::logic_error("OutMessageWrapper(MessageHeaderData): message is not fully initialised!");
    
    header.SerializeToString(&data_);
    return data_;
}

std::string OutMessageWrapper::serialiseToJson(const std::string &clientName,
                                   const std::string &ssnName, const std::string &channelName,
                                   oli::int64T seqId)const
{
    MessageHeaderData header;
    header.set_protocolversionmajor(PROTOCOL_MAJOR_VERSION);
    header.set_protocolversionminor(PROTOCOL_MINOR_VERSION);
    header.set_type(static_cast<MessageType>(type_));
    header.set_messagedata(data_);
    header.set_clientname(clientName);
    header.set_sessionname(ssnName);
    header.set_channelname(channelName);
    header.set_msgsequenceid(seqId);

    if(!header.IsInitialized())
        throw std::logic_error("OutMessageWrapper(MessageHeaderData): message is not fully initialised!");

    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;

    std::string jsonResult;
    MessageToJsonString(header, &jsonResult, options);
    return jsonResult;
}
