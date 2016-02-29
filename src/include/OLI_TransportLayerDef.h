/*
 * OLI_TransportLayerDef.h
 *
 *  Created on: Feb 26, 2016
 *      Author: sam1203
 */
 
#ifndef __OLI_TRANSPORTLAYER_DEF__H__
#define __OLI_TRANSPORTLAYER_DEF__H__

#include <string>
#include <vector>

#include "OLI_UtilsDefines.h"
#include "OLI_MsgDefines.h"

namespace oli{
    namespace tl{
        
        struct TLDataSource{
            uint64T sourceId_;
        };
    
        class TLAppObserver{
        public:    
            virtual ~TLAppObserver(){}
            
            virtual void onMessage(const TLDataSource &source, const msg::MessageBase &msg) = 0;
        };

        class TLSsnObserver{
        public:    
            virtual ~TLSsnObserver(){}
            
            virtual void onMessage(int64_t topic, const std::vector<char> &msg) = 0;
            virtual void onResendRequest() = 0;
        };
        
    }    
    
}

#endif // __OLI_TRANSPORTLAYER_DEF__H__