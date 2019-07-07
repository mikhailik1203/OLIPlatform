#include "OBTransportAdaptor.h"

#include "src/protobuf/oli_messageslib.pb.h"
#include "src/MsgHelpers.h"

using namespace oli::obook;

namespace{
    oli::msg::OBookChangeType toChangeType(MDLevelChangeType type){
        switch(type){
        case add_MDLevelChangeType:
            return oli::msg::add_OBookChangeType;
        case change_MDLevelChangeType:
            return oli::msg::change_OBookChangeType;
        case delete_MDLevelChangeType:
            return oli::msg::delete_OBookChangeType;
        case invalid_MDLevelChangeType:
        default:
            return oli::msg::invalid_OBookChangeType;
        };
        return oli::msg::invalid_OBookChangeType;
    }
    
}

OBTransportAdaptor::OBTransportAdaptor(): 
    ordObs_(nullptr), instrObs_(nullptr), publisher_(nullptr)
{
}

OBTransportAdaptor::~OBTransportAdaptor()
{
}

void OBTransportAdaptor::attachPublisher(oli::tl::TlMessagePublisher *publ)
{
    publisher_ = publ;
}

void OBTransportAdaptor::detachPublisher()
{
    publisher_ = nullptr;
}


void OBTransportAdaptor::attachOBookObserver(IInOrderBookMessageObserver *obs)
{
    ordObs_ = obs;
}

void OBTransportAdaptor::detachOBookObserver()
{
    ordObs_ = nullptr;
}

void OBTransportAdaptor::attachInstrObserver(secmaster::IInstrumentMessageObserver *obs)
{
    instrObs_ = obs;
}

void OBTransportAdaptor::detachInstrObserver()
{
    instrObs_ = nullptr;
}


void OBTransportAdaptor::onMessage(ConnectionIdT connId, const msg::ClntNewOrderRequestMsg */*msg*/)
{
    /// OrderBook ingnores this message
}

void OBTransportAdaptor::onMessage(ConnectionIdT connId, const msg::ClntCancelOrderRequestMsg */*msg*/)
{
    /// OrderBook ingnores this message
}

void OBTransportAdaptor::onMessage(ConnectionIdT connId, const msg::ClntReplaceOrderRequestMsg */*msg*/)
{
    /// OrderBook ingnores this message
}

void OBTransportAdaptor::onMessage(ConnectionIdT connId, const msg::OMSNewOrderMsg *msg)
{
    OrderBookRecord rec(msg->orderid(), msg->instrumentid(),
                        msg->price(), msg->volumeleft(),
                        msg::MsgHelpers::toSide(msg->side()));
    ordObs_.load()->onNewOrder(rec);
}

void OBTransportAdaptor::onMessage(ConnectionIdT connId, const msg::OMSChangeOrderMsg *msg)
{
    OrderBookRecord oldRec(msg->oldorderid(), msg->instrumentid(),
                        msg->oldprice(), msg->oldvolumeleft(),
                        msg::MsgHelpers::toSide(msg->side()));
    OrderBookRecord newRec(msg->neworderid(), msg->instrumentid(),
                        msg->newprice(), msg->newvolumeleft(),
                        msg::MsgHelpers::toSide(msg->side()));
    ordObs_.load()->onChangeOrder(oldRec, newRec);
}

void OBTransportAdaptor::onMessage(ConnectionIdT connId, const msg::OMSCancelOrderMsg *msg)
{
    OrderBookRecord rec(msg->orderid(), msg->instrumentid(),
                        msg->price(), msg->volumeleft(),
                        msg::MsgHelpers::toSide(msg->side()));
    ordObs_.load()->onDeleteOrder(rec);
}


void OBTransportAdaptor::onOBookChanged(const MDLevel2Record &rec)
{
    msg::OBBookChangeMsg msg;
    
    msg.set_type(toChangeType(rec.type_));
    msg.set_side(msg::MsgHelpers::toSide(rec.side_));
    msg.set_instrumentid(rec.instrumId_);
    msg.set_price(rec.price_);
    msg.set_volume(rec.volume_);
    msg.set_prevprice(rec.prevPrice_);
    msg.set_prevvolume(rec.prevVolume_);
    
    oli::msg::OutMessageWrapper msg2Send(&msg);
    publisher_.load()->sendMessage(ConnectionIdT(), msg2Send);
}

