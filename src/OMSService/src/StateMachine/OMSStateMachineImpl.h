#ifndef OMSSTATEMACHINEIMPL_H
#define OMSSTATEMACHINEIMPL_H

#include <boost/msm/front/state_machine_def.hpp>
#include <boost/msm/back/state_machine.hpp>

#include "OMSDefinesInt.h"
#include "OMSStates.h"
#include "OMSEvents.h"

namespace oli
{

namespace oms
{

namespace sm
{

    
    
    class OMSStateMachineImpl: 
                public boost::msm::front::state_machine_def<OMSStateMachineImpl>
    {
    public:
        OMSStateMachineImpl();
        ~OMSStateMachineImpl();


    public:
    ///------------------------- Guard functions ------------------------------------------
        /// validates incoming NewOrder request
        bool checkNewOrderRequest(const NewOrderRequestEvent &);
        /// validates incoming OrderCancel request
        bool checkCancelOrderRequest(const CancelOrderRequestEvent &);
        /// validates incoming OrderReplace request
        bool checkReplaceOrderRequest(const ReplaceOrderRequestEvent &);
        /// check trade and return true, if order is partially filled after the trade
        bool isPartFillTrade(const TradeEvent &);
        /// check trade and return true, if order is fully filled after the trade
        bool isFullFillTrade(const TradeEvent &);
        /// check TradeCorrection and return true, if order isn't traded after the correction
        bool isNewAfterTradeCorrect(const TradeCorrectEvent &);
        /// check TradeCorrection and return true, if order is partially filled after the correction
        bool isPartFillAfterTradeCorrect(const TradeCorrectEvent &);
        /// check TradeCorrection and return true, if order is fully filled after the correction
        bool isFullFillAfterTradeCorrect(const TradeCorrectEvent &);
        
    ///------------------------- Action functions ------------------------------------------
        /// creates Order entity and sends requests to the LC and OB
        void registerNewOrderRequest(const NewOrderRequestEvent &);
        /// rejects incoming NewOrder request
        void rejectNewOrderRequest(const NewOrderRequestEvent &);
        /// creates Order entity and sends requests to the LC and OB
        void registerCancelOrderRequest(const CancelOrderRequestEvent &);
        /// rejects incoming OrderCancel request
        void rejectCancelOrderRequest(const CancelOrderRequestEvent &);
        /// creates Order entity and sends requests to the LC and OB
        void registerReplaceOrderRequest(const ReplaceOrderRequestEvent &);
        /// rejects incoming OrderReplace request
        void rejectReplaceOrderRequest(const ReplaceOrderRequestEvent &);
        
        /// updates Order entity with data from LC
        void checkedNewOrderRequest(const LimitCheckSucceedEvent &);
        /// rejects incoming NewOrder request, because LC fails
        void rejectNewOrderRequest(const LimitCheckFailsEvent &);
        /// updates Order entity with data from OB
        void createNewOrder(const OrderBookSucceedEvent &);
        /// rejects incoming NewOrder request, because OB fails
        void rejectNewOrderRequest(const OrderBookFailsEvent &);
        
        /// updates Order entity with data from LC
        void checkedCancelOrderRequest(const LimitCheckSucceedEvent &);
        /// rejects incoming OrderCancel request, because LC fails
        void rejectCancelOrderRequest(const LimitCheckFailsEvent &);
        /// updates Order entity with data from OB
        void createCancelOrder(const OrderBookSucceedEvent &);
        /// rejects incoming OrderCancel request, because OB fails
        void rejectCancelOrderRequest(const OrderBookFailsEvent &);
        
        /// updates Order entity with data from LC
        void checkedReplaceOrderRequest(const LimitCheckSucceedEvent &);
        /// rejects incoming OrderReplace request, because LC fails
        void rejectReplaceOrderRequest(const LimitCheckFailsEvent &);
        /// updates Order entity with data from OB
        void createReplaceOrder(const OrderBookSucceedEvent &);
        /// rejects incoming OrderReplace request, because OB fails
        void rejectReplaceOrderRequest(const OrderBookFailsEvent &);
        
        /// rejects order because of the concurrent TradeHalt event
        void rejectOrder(const TradeHaltEvent &);
        /// updates Order entity with Trade data
        void processPartFillTrade(const TradeEvent &);
        /// updates Order entity with Trade data
        void processFullFillTrade(const TradeEvent &);
        /// updates original Order - apply OrderCancel request
        void processOrderCancel(const OrderCanceledEvent &);
        /// updates original Order - apply OrderReplace request
        void processOrderReplace(const OrderReplacedEvent &);
        /// cancels order because of the expiration time
        void processOrderExpiration(const OrderExpireEvent &);
        /// updates Order entity with TradeCorrection data
        void processTradeCorrect2New(const TradeCorrectEvent &);
        /// updates Order entity with TradeCorrection data
        void processTradeCorrect2PartFill(const TradeCorrectEvent &);
        /// updates Order entity with TradeCorrection data
        void processTradeCorrect2FullFill(const TradeCorrectEvent &);
       
    };



    typedef OMSStateMachineImpl SMImplT;

    // Transition table for player
    struct transition_table : boost::mpl::vector<
        //              Start         Event                      Next                  Action				                   Guard
        //            +-------------+--------------------------+---------------------+---------------------------------------+----------------------+
      SMImplT::  row < InitialState , NewOrderRequestEvent     , PendingNewState     , &SMImplT::registerNewOrderRequest     , &SMImplT::checkNewOrderRequest   >,
      SMImplT::a_row < InitialState , NewOrderRequestEvent     , RejectedState       , &SMImplT::rejectNewOrderRequest       >,
      SMImplT::  row < InitialState , CancelOrderRequestEvent  , PendingCancelState  , &SMImplT::registerCancelOrderRequest  , &SMImplT::checkCancelOrderRequest        >,
      SMImplT::a_row < InitialState , CancelOrderRequestEvent  , RejectedState       , &SMImplT::rejectCancelOrderRequest    >,
      SMImplT::  row < InitialState , ReplaceOrderRequestEvent , PendingReplaceState , &SMImplT::registerReplaceOrderRequest , &SMImplT::checkReplaceOrderRequest        >,
      SMImplT::a_row < InitialState , ReplaceOrderRequestEvent , RejectedState       , &SMImplT::rejectReplaceOrderRequest   >,
      
      SMImplT::a_row < PendingNewState , LimitCheckSucceedEvent , PendingNewState     , &SMImplT::checkedNewOrderRequest     >,
      SMImplT::a_row < PendingNewState , LimitCheckFailsEvent   , RejectedState       , &SMImplT::rejectNewOrderRequest      >,
      SMImplT::a_row < PendingNewState , OrderBookSucceedEvent  , NewState            , &SMImplT::createNewOrder             >,
      SMImplT::a_row < PendingNewState , OrderBookFailsEvent    , RejectedState       , &SMImplT::rejectNewOrderRequest      >,

      SMImplT::a_row < PendingCancelState , LimitCheckSucceedEvent , PendingCancelState  , &SMImplT::checkedCancelOrderRequest  >,
      SMImplT::a_row < PendingCancelState , LimitCheckFailsEvent   , RejectedState       , &SMImplT::rejectCancelOrderRequest   >,
      SMImplT::a_row < PendingCancelState , OrderBookSucceedEvent  , NewState            , &SMImplT::createCancelOrder          >,
      SMImplT::a_row < PendingCancelState , OrderBookFailsEvent    , RejectedState       , &SMImplT::rejectCancelOrderRequest   >,

      SMImplT::a_row < PendingReplaceState , LimitCheckSucceedEvent , PendingCancelState  , &SMImplT::checkedReplaceOrderRequest >,
      SMImplT::a_row < PendingReplaceState , LimitCheckFailsEvent   , RejectedState       , &SMImplT::rejectReplaceOrderRequest  >,
      SMImplT::a_row < PendingReplaceState , OrderBookSucceedEvent  , NewState            , &SMImplT::createReplaceOrder        >,
      SMImplT::a_row < PendingReplaceState , OrderBookFailsEvent    , RejectedState       , &SMImplT::rejectReplaceOrderRequest  >,

      SMImplT::a_row < NewState , TradeHaltEvent      , RejectedState    , &SMImplT::rejectOrder             >,
      SMImplT::  row < NewState , TradeEvent          , PartFilledState  , &SMImplT::processPartFillTrade    , &SMImplT::isPartFillTrade   >,
      SMImplT::  row < NewState , TradeEvent          , FilledState      , &SMImplT::processFullFillTrade    , &SMImplT::isFullFillTrade   >,
      SMImplT::a_row < NewState , OrderCanceledEvent  , CanceledState    , &SMImplT::processOrderCancel      >,
      SMImplT::a_row < NewState , OrderReplacedEvent  , ReplacedState    , &SMImplT::processOrderReplace     >,
      SMImplT::a_row < NewState , OrderExpireEvent    , ExpiredState     , &SMImplT::processOrderExpiration  >,
    
      SMImplT::  row < PartFilledState , TradeCorrectEvent  , NewState        , &SMImplT::processTradeCorrect2New      , &SMImplT::isNewAfterTradeCorrect   >,
      SMImplT::  row < PartFilledState , TradeCorrectEvent  , PartFilledState , &SMImplT::processTradeCorrect2PartFill , &SMImplT::isPartFillAfterTradeCorrect   >,
      SMImplT::  row < PartFilledState , TradeCorrectEvent  , FilledState     , &SMImplT::processTradeCorrect2FullFill , &SMImplT::isFullFillAfterTradeCorrect   >,
      SMImplT::  row < PartFilledState , TradeEvent         , PartFilledState , &SMImplT::processPartFillTrade         , &SMImplT::isPartFillTrade   >,
      SMImplT::  row < PartFilledState , TradeEvent         , FilledState     , &SMImplT::processFullFillTrade         , &SMImplT::isFullFillTrade   >,
      SMImplT::a_row < PartFilledState , OrderCanceledEvent , CanceledState   , &SMImplT::processOrderCancel      >,
      SMImplT::a_row < PartFilledState , OrderReplacedEvent , ReplacedState   , &SMImplT::processOrderReplace     >,
      SMImplT::a_row < PartFilledState , OrderExpireEvent   , ExpiredState    , &SMImplT::processOrderExpiration  >,
      
      SMImplT::  row < FilledState , TradeCorrectEvent  , NewState        , &SMImplT::processTradeCorrect2New       , &SMImplT::isNewAfterTradeCorrect   >,
      SMImplT::  row < FilledState , TradeCorrectEvent  , PartFilledState , &SMImplT::processTradeCorrect2PartFill  , &SMImplT::isPartFillAfterTradeCorrect   >,
      SMImplT::  row < FilledState , TradeCorrectEvent  , FilledState     , &SMImplT::processTradeCorrect2FullFill  , &SMImplT::isFullFillAfterTradeCorrect   >
        //            +------------+--------------------+-----------------+-----------------------------------+----------------------+      
    > {};

}

}

}

#endif // OMSSTATEMACHINEIMPL_H
