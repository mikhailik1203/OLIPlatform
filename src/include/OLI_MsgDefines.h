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
#include <memory>

#include "OLI_UtilsDefines.h"

namespace oli
{
namespace msg
{
    const int PROTOCOL_MAJOR_VERSION = 1;
    const int PROTOCOL_MINOR_VERSION = 1;

    
    class MessageLib{
    public:
        static void init();
        static void deinit();
    };
    
    class ClntNewOrderRequestMsg;
    class ClntCancelOrderRequestMsg;
    class ClntReplaceOrderRequestMsg;
    class OMSNewOrderMsg;
    class OMSChangeOrderMsg;
    class OMSCancelOrderMsg;
    class OMSOrderStatusMsg;
    class OMSTradeRejectMsg;
    class OMSTradeCorrectRejectMsg;
    class MEOrdersMatchedMsg;
    class OBBookChangeMsg;
    class OBBookChangesMsg;

    class AppMessageProcessor{
    public: 
        virtual ~AppMessageProcessor(){}
        
        virtual void onMessage(ConnectionIdT connId, const msg::ClntNewOrderRequestMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const msg::ClntCancelOrderRequestMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const msg::ClntReplaceOrderRequestMsg *msg) = 0;
        
        virtual void onMessage(ConnectionIdT connId, const msg::OMSNewOrderMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const msg::OMSChangeOrderMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const msg::OMSCancelOrderMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const msg::OMSOrderStatusMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const msg::OMSTradeRejectMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const msg::OMSTradeCorrectRejectMsg *msg) = 0;
        
        virtual void onMessage(ConnectionIdT connId, const msg::OBBookChangeMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const msg::OBBookChangesMsg *msg) = 0;
        
        virtual void onMessage(ConnectionIdT connId, const msg::MEOrdersMatchedMsg *msg) = 0;
        
    };

    class SsnLogonMsg;
    class SsnHeartBeatMsg;
    class SsnResendRequestMsg;
    class SsnSequenceChangeMsg;
    class SsnLogoutMsg;
    class SsnRejectMsg;
    class SsnRetransmittedMsg;
    
    class SsnMessageProcessor{
    public: 
        virtual ~SsnMessageProcessor(){}
      
        virtual void onMessage(ConnectionIdT connId, const SsnLogonMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const SsnHeartBeatMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const SsnResendRequestMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const SsnSequenceChangeMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const SsnLogoutMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const SsnRejectMsg *msg) = 0;
        virtual void onMessage(ConnectionIdT connId, const SsnRetransmittedMsg *msg) = 0;
    };
    
    struct ApplicationMsg{
    public:
        int type_; /// msg::MessageType
        //std::string body_;
        const char *buffer_;
        size_t bufferLen_;
        
        ApplicationMsg();
        //ApplicationMsg(int type, const std::string &body);
        ApplicationMsg(int type, const char *buffer, size_t bufferLen);
        ApplicationMsg(const ApplicationMsg &msg);
        ~ApplicationMsg();
    };
    
    /// Class is used to encapsulate outgoing message and encode it before send
    class OutMessageWrapper
    {
    public:
        OutMessageWrapper(int type, const std::string &data);
        explicit OutMessageWrapper(const msg::ClntNewOrderRequestMsg *msg);
        explicit OutMessageWrapper(const msg::ClntCancelOrderRequestMsg *msg);
        explicit OutMessageWrapper(const msg::ClntReplaceOrderRequestMsg *msg);
        
        explicit OutMessageWrapper(const msg::OMSNewOrderMsg *msg);
        explicit OutMessageWrapper(const msg::OMSChangeOrderMsg *msg);
        explicit OutMessageWrapper(const msg::OMSCancelOrderMsg *msg);
        explicit OutMessageWrapper(const msg::OMSOrderStatusMsg *msg);
        explicit OutMessageWrapper(const msg::OMSTradeRejectMsg *msg);
        explicit OutMessageWrapper(const msg::OMSTradeCorrectRejectMsg *msg);
        
        explicit OutMessageWrapper(const msg::OBBookChangeMsg *msg);
        explicit OutMessageWrapper(const msg::OBBookChangesMsg *msg);

        explicit OutMessageWrapper(const msg::MEOrdersMatchedMsg *msg);
        
        explicit OutMessageWrapper(const msg::SsnHeartBeatMsg *msg);
        explicit OutMessageWrapper(const msg::SsnLogonMsg *msg);
        explicit OutMessageWrapper(const msg::SsnLogoutMsg *msg);
        explicit OutMessageWrapper(const msg::SsnResendRequestMsg *msg);
        explicit OutMessageWrapper(const msg::SsnSequenceChangeMsg *msg);
        explicit OutMessageWrapper(const msg::SsnRejectMsg *msg);
        explicit OutMessageWrapper(const msg::SsnRetransmittedMsg *msg);
        
        ~OutMessageWrapper();

        const std::string &serialise(const std::string &clientName, 
                const std::string &ssnName, const std::string &channelName, 
                oli::int64T seqId)const;

        std::string serialiseToJson(const std::string &clientName,
                                     const std::string &ssnName, const std::string &channelName,
                                     oli::int64T seqId)const;

        int messageType()const;

        void setDestinationId(ConnectionIdT connId)const;
        ConnectionIdT destinationId()const;
    private:
        mutable std::string data_;
        mutable ConnectionIdT connId_;
        int type_;
    };
    
    class InMessageWrapper
    {
    public:
        InMessageWrapper(const char *data, size_t size);
        ~InMessageWrapper();

        bool isSessionLevelMsg()const;
        
        void handleSsnMessage(ConnectionIdT connId, SsnMessageProcessor &proc)const;
        void handleAppMessage(ConnectionIdT connId, AppMessageProcessor &proc)const;
        
        int channelId()const{return channelId_;}
        int64_t msgSeqNum()const{return msgSeqNum_;}
        const std::string &msgBody()const{return msgBody_;}
    private:
        int msgType_;
        int channelId_;
        int64_t msgSeqNum_;
        std::string msgBody_;
    };
    
    
}
}

#endif // OLI_MSGDEFINES_H
