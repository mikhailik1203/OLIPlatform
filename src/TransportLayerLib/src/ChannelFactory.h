/*
 * TLChannel.h
 *
 *  Created on: Aug 18, 2015
 *      Author: sam1203
 */
 
#ifndef CHANNELFACTORY_H
#define CHANNELFACTORY_H

#include "TransportLayerDef.h"
#include "TransportLayerImplDef.h"

namespace oli
{

namespace tlimpl
{

class ChannelFactory
{
public:
    static tl::Channel *create(const ChannelConfig &cfg);
};

}

}

#endif // CHANNELFACTORY_H
