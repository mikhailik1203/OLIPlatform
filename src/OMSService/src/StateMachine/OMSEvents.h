/*
 * OMSEvents.h
 *
 *  Created on: Feb 24, 2016
 *      Author: sam1203
 */


#ifndef OMSEVENTS_H
#define OMSEVENTS_H

namespace oli
{
namespace oms
{
namespace sm
{
    /// event fired when new order request received
    struct NewOrderRequestEvent
    {
    };
    
    /// event fired when new order cancel request received
    struct CancelOrderRequestEvent
    {
    };

    /// event fired when new order replace request received
    struct ReplaceOrderRequestEvent
    {
    };

    /// event fired when OMS fails to validate order - one or more errors found
    struct ValidationFailsEvent
    {
    };

    /// event fired when order succeed all LimitChecker check rules
    struct LimitCheckSucceedEvent
    {
    };

    /// event fired when order fails one or more LimitChecker check rules
    struct LimitCheckFailsEvent
    {
    };

    /// event fired, when OrderBook was succeesfully updated
    struct OrderBookSucceedEvent
    {
    };

    /// event fired, when OrderBook couldn't be updated
    struct OrderBookFailsEvent
    {
    };

    /// event is fired by OMS, when order could be canceled
    struct OrderCanceledEvent
    {
    };

    /// event is fired by OMS, when order could be replaced
    struct OrderReplacedEvent
    {
    };
    
    /// event is fired when trading is halted
    struct TradeHaltEvent
    {
    };

    /// event is fired by MatchingEngine, when 2 orders are matched
    struct TradeEvent
    {
    };

    /// event is fired by OMS, when order expiration date&time reached
    struct OrderExpireEvent
    {
    };

    /// event is fired, when executed trade should be changed
    struct TradeCorrectEvent
    {
    };


    /// interface for the events prcessing
    class OMSEventProcessor{
    public:
        virtual ~OMSEventProcessor(){}
        virtual void onEvent(const NewOrderRequestEvent &event) = 0;
        virtual void onEvent(const CancelOrderRequestEvent &event) = 0;
        virtual void onEvent(const ReplaceOrderRequestEvent &event) = 0;
        virtual void onEvent(const ValidationFailsEvent &event) = 0;
        virtual void onEvent(const LimitCheckSucceedEvent &event) = 0;
        virtual void onEvent(const LimitCheckFailsEvent &event) = 0;
        virtual void onEvent(const OrderBookSucceedEvent &event) = 0;
        virtual void onEvent(const OrderBookFailsEvent &event) = 0;
        virtual void onEvent(const OrderCanceledEvent &event) = 0;
        virtual void onEvent(const OrderReplacedEvent &event) = 0;
        virtual void onEvent(const TradeHaltEvent &event) = 0;
        virtual void onEvent(const TradeEvent &event) = 0;
        virtual void onEvent(const OrderExpireEvent &event) = 0;
        virtual void onEvent(const TradeCorrectEvent &event) = 0;
    };

}
}
}

#endif