#ifndef OMSSTATES_H
#define OMSSTATES_H

#include <boost/msm/front/state_machine_def.hpp>

namespace oli
{
namespace oms
{
namespace sm
{
    // The list of FSM states
    struct InitialState : public boost::msm::front::state<> 
    {
    };
    
    struct PendingNewState : public boost::msm::front::state<> 
    {
    };

    struct RejectedState : public boost::msm::front::state<> 
    {
    };
    
    struct NewState : public boost::msm::front::state<> 
    {
    };

    struct PartFilledState : public boost::msm::front::state<> 
    {
    };

    struct FilledState : public boost::msm::front::state<> 
    {
    };

    struct ExpiredState : public boost::msm::front::state<> 
    {
    };

    struct PendingCancelState : public boost::msm::front::state<> 
    {
    };
    
    struct CanceledState : public boost::msm::front::state<> 
    {
    };
   
    struct PendingReplaceState : public boost::msm::front::state<> 
    {
    };

    struct ReplacedState : public boost::msm::front::state<> 
    {
    };

    
}
}
}

#endif