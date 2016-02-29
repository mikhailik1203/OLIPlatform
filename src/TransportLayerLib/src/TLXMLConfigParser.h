/*
 * TLXMLConfigParser.h
 *
 *  Created on: Aug 15, 2015
 *      Author: sam1203
 */
 
#ifndef TLXMLCONFIGPARSER_H
#define TLXMLCONFIGPARSER_H

#include "TransportLayerImplDef.h"

namespace oli{
    namespace tlimpl{

class TLXMLConfigParser
{
public:
    TLXMLConfigParser();
    ~TLXMLConfigParser();

    static NodesConfigT parse(const std::string &xmlFile);
};

    }
}

#endif // TLXMLCONFIGPARSER_H
