//
// Created by sam1203 on 9/18/17.
//

#include "METransportAdaptor.h"
#include "oli_messageslib.pb.h"
#include "MsgHelpers.h"

using namespace oli::mengine;
using namespace oli::msg;

namespace{
    MDUpdateType toMDUpdateType(oli::msg::OBookChangeType val)
    {
        switch(val){
            case OBookChangeType::add_OBookChangeType:
                return MDUpdateType::add_MDUpdateType;
            case OBookChangeType::change_OBookChangeType:
                return MDUpdateType::update_MDUpdateType;
            case OBookChangeType::delete_OBookChangeType:
                return MDUpdateType::remove_MDUpdateType;
            default:
                ;
        };
        return MDUpdateType::invalid_MDUpdateType;
    }

}

METransportAdaptor::METransportAdaptor(IInMEOBookObserver *obookObs, IInMEOrderObserver *orderObs):
        obookObs_(obookObs), orderObs_(orderObs)
{}

METransportAdaptor::~METransportAdaptor()
{

}

void METransportAdaptor::onMessage(const msg::ClntNewOrderRequestMsg */*msg*/)
{
    /// matching engine ignores this message
}

void METransportAdaptor::onMessage(const msg::ClntCancelOrderRequestMsg */*msg*/)
{
    /// matching engine ignores this message
}

void METransportAdaptor::onMessage(const msg::ClntReplaceOrderRequestMsg */*msg*/)
{
    /// matching engine ignores this message
}

void METransportAdaptor::onMessage(const msg::OMSNewOrderMsg *msg)
{
    switch(msg->ordtype()){
        case oli::msg::marketOrderType:
        {
            MarketOrderData data = {msg->orderid(),
                                    msg::MsgHelpers::toSide(msg->side()),
                                    msg->volumeleft()};
            orderObs_->onNewMarketOrder(data);
        }
        break;
        case oli::msg::limitOrderType:
        {
            AggressorOrderData data = {msg->orderid(),
                                       msg::MsgHelpers::toSide(msg->side()),
                                       msg->volumeleft(),
                                       static_cast<oli::priceT>(msg->price())};
            orderObs_->onNewOrderAggressor(data);
        }
        break;
        default:
            ;
    };
}

void METransportAdaptor::onMessage(const msg::OMSChangeOrderMsg */*msg*/)
{
    /// matching engine ignores this message
}

void METransportAdaptor::onMessage(const msg::OMSCancelOrderMsg */*msg*/)
{
    /// matching engine ignores this message
}

void METransportAdaptor::onMessage(const msg::OMSOrderStatusMsg */*msg*/)
{
    /// matching engine ignores this message
}

void METransportAdaptor::onMessage(const msg::OBBookChangeMsg *msg)
{
    assert(nullptr != msg);
    obookObs_->onOBookChanged(MDLevel2Record(toMDUpdateType(msg->type()), msg->orderid(),
                              MsgHelpers::toSide(msg->side()), static_cast<oli::priceT>(msg->volume()),
                              msg->price()));
}

void METransportAdaptor::onMessage(const msg::OBBookChangesMsg *msg)
{
    MDLevel2Record data;
    obookObs_->onOBookChanged(data);
}

void METransportAdaptor::onMessage(const msg::MEOrdersMatchedMsg */*msg*/)
{
    /// matching engine ignores this message
}

void METransportAdaptor::onOrdersMatched(const OrderMatchEvent &)
{
    ///todo: send message to he transportLayer
}
