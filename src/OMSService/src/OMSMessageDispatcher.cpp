#include "OMSMessageDispatcher.h"

using namespace oli::oms;
using namespace oli::tl;

OMSMessageDispatcher::OMSMessageDispatcher()
{
}

OMSMessageDispatcher::~OMSMessageDispatcher()
{
}

void OMSMessageDispatcher::onMessage(const TLDataSource &source, const msg::MessageBase &msg)
{
    /// switch by message type and decode required message
    /// process message
    
}

void OMSMessageDispatcher::process(const msg::ClntNewOrderRequestMsg &msg)
{
    /// prepare context and event
    /// initialise state machine
    /// push event to the state machine
    /// store state machine state
}