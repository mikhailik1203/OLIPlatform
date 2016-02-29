/*
 * TransportLayer.h
 *
 *  Created on: Aug 15, 2015
 *      Author: sam1203
 */

#ifndef TRANSPORT_LAYER_H_
#define TRANSPORT_LAYER_H_

#include "TransportLayerDef.h"

namespace oli{
    namespace tlimpl{
        struct TransportLayerImpl;
    }
 
    namespace tl{

       
class TransportLayer final{
public:
    /// constructs transport layer for this node using data from xml config file
	TransportLayer(const std::string &xmlConfigFile, const std::string &nodeName);
	~TransportLayer();
        
    /// returns names of channels defined for this node
    const ChannelNamesT &channels()const;
    
    /// returns channel by name
    Channel *channel(const std::string &name)const;
        
private:
    tlimpl::TransportLayerImpl *impl_;
    ChannelNamesT channelNames_;
    ChannelsT channels_;
};

    }
}

#endif /* TRANSPORT_LAYER_H_ */
