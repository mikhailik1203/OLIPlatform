/*
 * OLI_UtilsDefines.h
 *
 *  Created on: Feb 23, 2016
 *      Author: sam1203
 */
 
#ifndef __OLI_UTILS_DEF__H__
#define __OLI_UTILS_DEF__H__

#include <string>
#include <cstdint>
#include <deque>

namespace oli{
        
    typedef uint64_t uint64T;
    typedef uint64T idT;
    const idT INVALID_ID = 0;
    typedef int64_t int64T;
    typedef int64_t datetimeUTCT;

    typedef uint32_t uint32T;
    typedef uint32_t quantityT;
    typedef int32_t priceT;
    typedef int32_t int32T;

    using ConnectionIdT = int32T;

    typedef uint8_t uint8T;
    typedef int8_t int8T;

    typedef std::deque<idT> IdsDequeT;
   
   
    class IScheduledEventProcessor{
    public:
        virtual ~IScheduledEventProcessor(){}
        
        virtual void onTimer(int64_t eventId, int64_t contextId) = 0;
    };

    class IEventScheduler{
    public:
        virtual ~IEventScheduler(){}
        
        virtual oli::idT addEvent(IScheduledEventProcessor *proc, oli::idT context, 
                                  int timeoutMSec, bool repeating) = 0;
        virtual void removeEvent(oli::idT id) = 0;
    };
   
}

#endif // __OLI_UTILS_DEF__H__