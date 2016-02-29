/*
 * TransportLayer.cpp
 *
 *  Created on: Aug 15, 2015
 *      Author: sam1203
 */

#include "TransportLayer.h"
#include "zmq.hpp"
#include "TLXMLConfigParser.h"

using namespace oli;
using namespace oli::tl;
using namespace oli::tlimpl;

namespace oli{
    namespace tlimpl{
        
struct TransportLayerImpl{
public:
    TransportLayerImpl(): context_(1){}
    ~TransportLayerImpl(){}
public:
    zmq::context_t context_;
};

    }
}

TransportLayer::TransportLayer(const std::string &xmlConfigFile, const std::string &nodeName): 
    impl_(new tlimpl::TransportLayerImpl) 
{
    NodesConfigT nodesConfig = TLXMLConfigParser::parse(xmlConfigFile);
    
    NodesConfigT::const_iterator nodeCfgIt = nodesConfig.find(nodeName);
    if(nodesConfig.end() == nodeCfgIt)
        return;
        
    /// process all channels for nodeName and create channels
}

TransportLayer::~TransportLayer() 
{
    delete impl_;
    impl_ = nullptr;
}

const ChannelNamesT &TransportLayer::channels()const
{
    return channelNames_;
}

Channel *TransportLayer::channel(const std::string &name)const
{
    ChannelsT::const_iterator it = channels_.find(name);
    if(channels_.end() != it)
        return it->second;
    return nullptr;
}
