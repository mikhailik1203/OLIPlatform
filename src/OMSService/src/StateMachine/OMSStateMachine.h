/*
 * OMSStateMachine.h
 *
 *  Created on: Feb 23, 2016
 *      Author: sam1203
 */
 
#ifndef OMSSTATEMACHINE_H
#define OMSSTATEMACHINE_H

#include "OMSEvents.h"

namespace oli
{
namespace oms
{
    
    namespace sm{
        class OMSStateMachineImpl;
    }

class OMSStateMachine: public sm::OMSEventProcessor
{
public:
    OMSStateMachine();
    ~OMSStateMachine();

public:
    virtual void onEvent(const sm::NewOrderRequestEvent &event);
    virtual void onEvent(const sm::CancelOrderRequestEvent &event);
    virtual void onEvent(const sm::ReplaceOrderRequestEvent &event);
    virtual void onEvent(const sm::ValidationFailsEvent &event);
    virtual void onEvent(const sm::LimitCheckSucceedEvent &event);
    virtual void onEvent(const sm::LimitCheckFailsEvent &event);
    virtual void onEvent(const sm::OrderBookSucceedEvent &event);
    virtual void onEvent(const sm::OrderBookFailsEvent &event);
    virtual void onEvent(const sm::OrderCanceledEvent &event);
    virtual void onEvent(const sm::OrderReplacedEvent &event);
    virtual void onEvent(const sm::TradeHaltEvent &event);
    virtual void onEvent(const sm::TradeEvent &event);
    virtual void onEvent(const sm::OrderExpireEvent &event);
    virtual void onEvent(const sm::TradeCorrectEvent &event);

private:
    sm::OMSStateMachineImpl *impl_;
};

}
}
#endif // OMSSTATEMACHINE_H
