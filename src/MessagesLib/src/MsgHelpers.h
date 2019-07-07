/**
 * @class Message helpers functions
 * @author Sergey A Mikhailik
 * @date 20/Nov/16
 * @brief Helper functions for translating msg enums to the oli
 */

#ifndef MSGHELPERS_H
#define MSGHELPERS_H

#include "OLI_FinanceDomain.h"
#include "OLI_MsgDefines.h"

#include "protobuf/oli_messageslib.pb.h"

#include <string>

namespace oli
{
namespace msg
{
   
    /// Class is used to encapsulate ougoing message and encode it before send
    class MsgHelpers
    {
    public:
        static MessageType toMessageType(const std::string &val);
        static std::string to_string(MessageType val);

        static oli::Side toSide(const oli::msg::OrderSide &side);
        static oli::msg::OrderSide toSide(const oli::Side &side);
        
        static oli::InstrumentType toInstrument(const oli::msg::InstrumentType &type);
        static oli::msg::InstrumentType toInstrument(const oli::InstrumentType &type);

        static oli::OrderType toOrderType(const oli::msg::OrderType type);
        static oli::msg::OrderType toOrderType(const oli::OrderType type);

        static oli::OrderState toOrderStatus(const oli::msg::OrderStatus &status);
        static oli::msg::OrderStatus toOrderStatus(const oli::OrderState &status);
        
        static std::string serialise(const oli::msg::SsnLogonMsg &msg);
        static std::string serialise(const oli::msg::SsnHeartBeatMsg &msg);
        static std::string serialise(const oli::msg::SsnResendRequestMsg &msg);
        static std::string serialise(const oli::msg::SsnSequenceChangeMsg &msg);
        static std::string serialise(const oli::msg::SsnLogoutMsg &msg);
        static std::string serialise(const oli::msg::SsnRejectMsg &msg);
        static std::string serialise(const oli::msg::SsnRetransmittedMsg &msg);
        
        static std::string serialise(const oli::msg::ClntNewOrderRequestMsg &msg);
        static std::string serialise(const oli::msg::OMSNewOrderMsg &msg);
        static std::string serialise(const oli::msg::OMSOrderStatusMsg &msg);

        static std::string parseFromJson(MessageType type, const std::string &json);
        static void parseFromJson(const std::string &json, oli::msg::SsnLogonMsg &msg);
        static void parseFromJson(const std::string &json, oli::msg::ClntNewOrderRequestMsg &msg);
        static void parseFromJson(const std::string &json, oli::msg::OMSNewOrderMsg &msg);
        static std::string serialiseToJson(const oli::msg::SsnLogonMsg &msg);
        static std::string serialiseToJson(const oli::msg::ClntNewOrderRequestMsg &msg);
        static std::string serialiseToJson(const oli::msg::OMSNewOrderMsg &msg);
    };
   

}
}

#endif // MSGHELPERS_H
