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
    
        /// Interface for publishing messages to the TL channel
        class TlMessagePublisher{
        public:
            virtual ~TlMessagePublisher(){}
            
            virtual void sendMessage(ConnectionIdT connId, const oli::msg::OutMessageWrapper &msg) = 0;
        };
        
        
    }    
    
}

#endif // __OLI_TRANSPORTLAYER_DEF__H__