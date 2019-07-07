#include "MsgHelpers.h"
#include "src/protobuf/oli_messageslib.pb.h"

#include <sstream>
#include <google/protobuf/util/json_util.h>


using namespace oli::msg;

namespace{
   std::unordered_map<MessageType, std::string> messageTypeToName = {
           {SsnLogonMsg_messageType, "SsnLogonMsg_messageType"},
           {SsnHeartBeatMsg_messageType, "SsnHeartBeatMsg_messageType"},
           {SsnHeartBeatMsg_messageType, "SsnHeartBeatMsg_messageType"},
           {SsnResendRequestMsg_messageType, "SsnResendRequestMsg_messageType"},
           {SsnSequenceChangeMsg_messageType, "SsnSequenceChangeMsg_messageType"},
           {SsnLogoutMsg_messageType, "SsnLogoutMsg_messageType"},
           {SsnRejectMsg_messageType, "SsnRejectMsg_messageType"},
           {SsnRetransmittedMsg_messageType, "SsnRetransmittedMsg_messageType"},
           {clntNewOrderRequest_messageType, "clntNewOrderRequest_messageType"},
           {clntOrderReplaceRequest_messageType, "clntOrderReplaceRequest_messageType"},
           {clntOrderCancelRequest_messageType, "clntOrderCancelRequest_messageType"},
           {obCancelOrder_messageType, "obCancelOrder_messageType"},
           {obCancelAllOrders_messageType, "obCancelAllOrders_messageType"},
           {obBookChanges_messageType, "obBookChanges_messageType"},
           {obBookChange_messageType, "obBookChange_messageType"},
           {lcOrderCancel_messageType, "lcOrderCancel_messageType"},
           {lcCancelAllOrders_messageType, "lcCancelAllOrders_messageType"},
           {smSecurityData_messageType, "smSecurityData_messageType"},
           {smSecGroupData_messageType, "smSecGroupData_messageType"},
           {smMarketData_messageType, "smMarketData_messageType"},
           {smMarketStatusUpdate_messageType, "smMarketStatusUpdate_messageType"},
           {umAccountData_messageType, "umAccountData_messageType"},
           {umAccountStatusUpdate_messageType, "umAccountStatusUpdate_messageType"},
           {meOrderMatched_messageType, "meOrderMatched_messageType"},
           {pmNewOrderRequest_messageType, "pmNewOrderRequest_messageType"},
           {omsNewOrder_messageType, "omsNewOrder_messageType"},
           {omsOrderStatus_messageType, "omsOrderStatus_messageType"},
           {omsCancelOrder_messageType, "omsCancelOrder_messageType"},
           {omsReplaceOrder_messageType, "omsReplaceOrder_messageType"},
           {omsTradeReject_messageType, "omsTradeReject_messageType"},
           {omsTradeCorrectReject_messageType, "omsTradeCorrectReject_messageType"},
           {obSubscription_messageType, "obSubscription_messageType"},
           {mdSubscription_messageType, "mdSubscription_messageType"},
           {mdrSubscription_messageType, "mdrSubscription_messageType"},
           {pmNotification_messageType, "pmNotification_messageType"},
           {oInstanceStop_messageType, "oInstanceStop_messageType"}
   };

    std::unordered_map<std::string, MessageType> nameToMessageType = {
            {"SsnLogonMsg_messageType", SsnLogonMsg_messageType},
            {"SsnHeartBeatMsg_messageType", SsnHeartBeatMsg_messageType},
            {"SsnHeartBeatMsg_messageType", SsnHeartBeatMsg_messageType},
            {"SsnResendRequestMsg_messageType", SsnResendRequestMsg_messageType},
            {"SsnSequenceChangeMsg_messageType", SsnSequenceChangeMsg_messageType},
            {"SsnLogoutMsg_messageType", SsnLogoutMsg_messageType},
            {"SsnRejectMsg_messageType", SsnRejectMsg_messageType},
            {"SsnRetransmittedMsg_messageType", SsnRetransmittedMsg_messageType},
            {"clntNewOrderRequest_messageType", clntNewOrderRequest_messageType},
            {"clntOrderReplaceRequest_messageType", clntOrderReplaceRequest_messageType},
            {"clntOrderCancelRequest_messageType", clntOrderCancelRequest_messageType},
            {"obCancelOrder_messageType_messageType", obCancelOrder_messageType},
            {"obCancelAllOrders_messageType", obCancelAllOrders_messageType},
            {"obBookChanges_messageType", obBookChanges_messageType},
            {"obBookChange_messageType", obBookChange_messageType},
            {"lcOrderCancel_messageType", lcOrderCancel_messageType},
            {"lcCancelAllOrders_messageType", lcCancelAllOrders_messageType},
            {"smSecurityData_messageType", smSecurityData_messageType},
            {"smSecGroupData_messageType", smSecGroupData_messageType},
            {"smMarketData_messageType", smMarketData_messageType},
            {"smMarketStatusUpdate_messageType", smMarketStatusUpdate_messageType},
            {"umAccountData_messageType", umAccountData_messageType},
            {"umAccountStatusUpdate_messageType", umAccountStatusUpdate_messageType},
            {"meOrderMatched_messageType", meOrderMatched_messageType},
            {"pmNewOrderRequest_messageType", pmNewOrderRequest_messageType},
            {"omsNewOrder_messageType", omsNewOrder_messageType},
            {"omsOrderStatus_messageType", omsOrderStatus_messageType},
            {"omsCancelOrder_messageType", omsCancelOrder_messageType},
            {"omsReplaceOrder_messageType", omsReplaceOrder_messageType},
            {"omsTradeReject_messageType", omsTradeReject_messageType},
            {"omsTradeCorrectReject_messageType", omsTradeCorrectReject_messageType},
            {"obSubscription_messageType", obSubscription_messageType},
            {"mdSubscription_messageType", mdSubscription_messageType},
            {"mdrSubscription_messageType", mdrSubscription_messageType},
            {"pmNotification_messageType", pmNotification_messageType},
            {"oInstanceStop_messageType", oInstanceStop_messageType}
    };


    template<typename T>
    std::string serialiseMessageToJson(const T &msg)
    {
        google::protobuf::util::JsonPrintOptions options;
        options.add_whitespace = true;
        options.always_print_primitive_fields = true;

        std::string jsonResult;
        MessageToJsonString(msg, &jsonResult, options);
        return jsonResult;
    }

    template<typename T>
    std::string parseMessageFromJson(const std::string &json)
    {
        google::protobuf::util::JsonParseOptions options2;
        T msg;
        JsonStringToMessage(json, &msg, options2);
        std::string data;
        msg.SerializeToString(&data);
        return data;
    }

}

MessageType MsgHelpers::toMessageType(const std::string &val)
{
    auto it = nameToMessageType.find(val);
    if(nameToMessageType.end() != it)
        return it->second;
    return invalid_messageType;
}

std::string MsgHelpers::to_string(MessageType val)
{
    auto it = messageTypeToName.find(val);
    if(messageTypeToName.end() != it)
        return it->second;
    return "";
}

oli::Side MsgHelpers::toSide(const oli::msg::OrderSide &side)
{
    return static_cast<oli::Side>(side);
    /*switch (side){
        case buySide:
            return oli::Side::buy_Side;
        case sellSide:
            return oli::Side::sell_Side;
        case invalidSide:
        default:
            return oli::Side::invalid_Side;
    };
    return oli::Side::invalid_Side;*/
}

oli::msg::OrderSide MsgHelpers::toSide(const oli::Side &side)
{
    return static_cast<oli::msg::OrderSide>(side);
    /*switch (side){
        case oli::Side::buy_Side:
            return buySide;
        case oli::Side::sell_Side:
            return sellSide;
        case oli::Side::invalid_Side:
        default:
            return invalidSide;
    };
    return invalidSide;*/
}

oli::InstrumentType MsgHelpers::toInstrument(const oli::msg::InstrumentType &type)
{
    switch (type){
        case stockInstrumentType:
            return oli::InstrumentType::stock_instrumentType;
        case invalidInstrumentType:
        default:
            return oli::InstrumentType::invalid_instrumentType;
    };
    return oli::InstrumentType::invalid_instrumentType;
}

oli::msg::InstrumentType MsgHelpers::toInstrument(const oli::InstrumentType &type)
{
    switch (type){
        case oli::InstrumentType::stock_instrumentType:
            return stockInstrumentType;
        case oli::InstrumentType::invalid_instrumentType:
        default:
            return invalidInstrumentType;
    };
    return invalidInstrumentType;
}

oli::OrderType MsgHelpers::toOrderType(const oli::msg::OrderType type)
{
    switch (type){
        case marketOrderType:
            return oli::OrderType::market_orderType;
        case limitOrderType:
            return oli::OrderType::limit_orderType;
        case stopOrderType:
            return oli::OrderType::stop_orderType;
        case invalidOrderType:
        default:
            return oli::OrderType::invalid_orderType;
    };
    return oli::OrderType::invalid_orderType;
}

oli::msg::OrderType MsgHelpers::toOrderType(const oli::OrderType type)
{
    switch (type){
        case oli::OrderType::market_orderType:
            return marketOrderType;
        case oli::OrderType::limit_orderType:
            return limitOrderType;
        case oli::OrderType::stop_orderType:
            return stopOrderType;
        case oli::OrderType::invalid_orderType:
        default:
            return invalidOrderType;
    };
    return OrderType::invalidOrderType;

}

oli::OrderState MsgHelpers::toOrderStatus(const oli::msg::OrderStatus &status)
{
    switch (status){
        case newOrderStatus:
            return oli::OrderState::new_orderState;
        case canceledOrderStatus:
            return oli::OrderState::canceled_orderState;
        case filledOrderStatus:
            return oli::OrderState::filled_orderState;
        case pendingNewOrderStatus:
            return oli::OrderState::pendingNew_orderState;
        case invalidOrderStatus:
        default:
            return oli::OrderState::invalid_orderState;
    };
    return oli::OrderState::invalid_orderState;
}

oli::msg::OrderStatus MsgHelpers::toOrderStatus(const oli::OrderState &status)
{
    switch (status){
        case oli::OrderState::new_orderState:
            return newOrderStatus;
        case oli::OrderState::canceled_orderState:
            return canceledOrderStatus;
        case oli::OrderState::filled_orderState:
            return filledOrderStatus;
            
        case oli::OrderState::pendingNew_orderState:
            return pendingNewOrderStatus;
        case oli::OrderState::pendingCancel_orderState:
        case oli::OrderState::pendingReplace_orderState:
        case oli::OrderState::partFill_orderState:
        case oli::OrderState::rejected_orderState:
        case oli::OrderState::replaced_orderState:
        case oli::OrderState::expired_orderState:
            return invalidOrderStatus;
            
        case oli::OrderState::invalid_orderState:
        default:
            return invalidOrderStatus;
    };
    return invalidOrderStatus;
}

std::string MsgHelpers::serialise(const oli::msg::SsnLogonMsg &msg)
{
    std::stringstream ostr;
    ostr << "<SsnLogonMsg: lastReceivedSeqId["<< msg.lastreceivedseqid()
         << "], nextExpectedSeqId["<< msg.nextexpectedseqid()<< "], heartBeatInterval["
         << msg.heartbeatinterval()<< "]>";
    return ostr.str();
}

std::string MsgHelpers::serialise(const oli::msg::SsnHeartBeatMsg &msg)
{
    std::stringstream ostr;
    ostr << "<SsnHeartBeatMsg: tst["<< msg.tst()<< "]>";
    return ostr.str();
}

std::string MsgHelpers::serialise(const oli::msg::SsnResendRequestMsg &msg)
{
    std::stringstream ostr;
    ostr << "<SsnResendRequestMsg: fromSequenceId["<< msg.fromsequenceid()
         << "], toSequenceId["<< msg.tosequenceid()<< "]>";
    return ostr.str();
}

std::string MsgHelpers::serialise(const oli::msg::SsnSequenceChangeMsg &msg)
{
    std::stringstream ostr;
    ostr << "<SsnSequenceChangeMsg: nextExpectedId["<< msg.nextexpectedid()<< "]>";
    return ostr.str();
}

std::string MsgHelpers::serialise(const oli::msg::SsnLogoutMsg &msg)
{
    std::stringstream ostr;
    ostr << "<SsnLogonMsg: reason["<< msg.reason()<< "]>";
    return ostr.str();
}

std::string MsgHelpers::serialise(const oli::msg::SsnRejectMsg &msg)
{
    std::stringstream ostr;
    ostr << "<SsnRejectMsg: reason["<< msg.rejectreason()<< "]>";
    return ostr.str();
}

std::string MsgHelpers::serialise(const oli::msg::SsnRetransmittedMsg &msg)
{
    std::stringstream ostr;
    ostr << "<SsnRetransmittedMsg: reason["<< msg.msg()<< "]>";
    return ostr.str();
}

std::string MsgHelpers::serialise(const oli::msg::ClntNewOrderRequestMsg &msg)
{
    std::stringstream ostr;
    ostr << "<ClntNewOrderRequestMsg: clientRequestId["<< msg.clientrequestid()
         << "], side["<< msg.side()<< "], type["<< msg.type()<< "]>";
    return ostr.str();
}

std::string MsgHelpers::serialise(const oli::msg::OMSNewOrderMsg &msg)
{
    std::stringstream ostr;
    ostr << "<OMSNewOrderMsg: orderId["<< msg.orderid()
         << "], instrumentId["<< msg.instrumentid()<< "], price["
         << msg.price()<< "]>";
    return ostr.str();
}

std::string MsgHelpers::serialise(const oli::msg::OMSOrderStatusMsg &msg)
{
    std::stringstream ostr;
    ostr << "<OMSOrderStatusMsg: orderId["<< msg.orderid()
         << "], status["<< msg.status()<< "]>";
    return ostr.str();
}

std::string MsgHelpers::parseFromJson(MessageType type, const std::string &json)
{
    google::protobuf::util::JsonParseOptions options2;
    switch(type){
        case SsnLogonMsg_messageType:
            return parseMessageFromJson<oli::msg::SsnLogonMsg>(json);
        case SsnHeartBeatMsg_messageType:
            return parseMessageFromJson<oli::msg::SsnHeartBeatMsg>(json);
        case SsnResendRequestMsg_messageType:
            return parseMessageFromJson<oli::msg::SsnResendRequestMsg>(json);
        case SsnSequenceChangeMsg_messageType:
            return parseMessageFromJson<oli::msg::SsnSequenceChangeMsg>(json);
        case SsnLogoutMsg_messageType:
            return parseMessageFromJson<oli::msg::SsnLogoutMsg>(json);
        case SsnRejectMsg_messageType:
            return parseMessageFromJson<oli::msg::SsnRejectMsg>(json);
        case SsnRetransmittedMsg_messageType:
            return parseMessageFromJson<oli::msg::SsnRetransmittedMsg>(json);
        case clntNewOrderRequest_messageType:
            return parseMessageFromJson<oli::msg::ClntNewOrderRequestMsg>(json);
        case clntOrderReplaceRequest_messageType:
            break;
        case clntOrderCancelRequest_messageType:
            break;
        case obCancelOrder_messageType:
            break;
        case obCancelAllOrders_messageType:
            break;
        case obBookChanges_messageType:
            break;
        case obBookChange_messageType:
            break;
        case lcOrderCancel_messageType:
            break;
        case lcCancelAllOrders_messageType:
            break;
        case smSecurityData_messageType:
            break;
        case smSecGroupData_messageType:
            break;
        case smMarketData_messageType:
            break;
        case smMarketStatusUpdate_messageType:
            break;
        case umAccountData_messageType:
            break;
        case umAccountStatusUpdate_messageType:
            break;
        case meOrderMatched_messageType:
            break;
        case pmNewOrderRequest_messageType:
            break;
        case omsNewOrder_messageType:
            return parseMessageFromJson<oli::msg::OMSNewOrderMsg>(json);
        case omsOrderStatus_messageType:
            break;
        case omsCancelOrder_messageType:
            break;
        case omsReplaceOrder_messageType:
            break;
        case omsTradeReject_messageType:
            break;
        case omsTradeCorrectReject_messageType:
            break;
        case obSubscription_messageType:
            break;
        case mdSubscription_messageType:
            break;
        case mdrSubscription_messageType:
            break;
        case pmNotification_messageType:
            break;
        case oInstanceStop_messageType:
            break;
        default:
            throw std::runtime_error("MsgHelpers::parseFromJson: unknown message type!");
    };
    return "";
}

std::string MsgHelpers::serialiseToJson(const oli::msg::SsnLogonMsg &msg)
{
    return serialiseMessageToJson(msg);
}

std::string MsgHelpers::serialiseToJson(const oli::msg::ClntNewOrderRequestMsg &msg)
{
    return serialiseMessageToJson(msg);
}

std::string MsgHelpers::serialiseToJson(const oli::msg::OMSNewOrderMsg &msg)
{
    return serialiseMessageToJson(msg);
}
