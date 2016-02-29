#include "OMSStateMachine.h"
#include "OMSStateMachineImpl.h"

using namespace oli::oms;
using namespace oli::oms::sm;

OMSStateMachine::OMSStateMachine(): impl_(new OMSStateMachineImpl)
{
}

OMSStateMachine::~OMSStateMachine()
{
    delete impl_;
}

void OMSStateMachine::onEvent(const NewOrderRequestEvent &event)
{
    
}

void OMSStateMachine::onEvent(const CancelOrderRequestEvent &event)
{}
void OMSStateMachine::onEvent(const ReplaceOrderRequestEvent &event)
{}
void OMSStateMachine::onEvent(const ValidationFailsEvent &event)
{}
void OMSStateMachine::onEvent(const LimitCheckSucceedEvent &event)
{}
void OMSStateMachine::onEvent(const LimitCheckFailsEvent &event)
{}
void OMSStateMachine::onEvent(const OrderBookSucceedEvent &event)
{}
void OMSStateMachine::onEvent(const OrderBookFailsEvent &event)
{}
void OMSStateMachine::onEvent(const OrderCanceledEvent &event)
{}
void OMSStateMachine::onEvent(const OrderReplacedEvent &event)
{}
void OMSStateMachine::onEvent(const TradeHaltEvent &event)
{}
void OMSStateMachine::onEvent(const TradeEvent &event)
{}
void OMSStateMachine::onEvent(const OrderExpireEvent &event)
{}
void OMSStateMachine::onEvent(const TradeCorrectEvent &event)
{}

