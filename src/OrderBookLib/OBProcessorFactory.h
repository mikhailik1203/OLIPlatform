#ifndef OBPROCESSORFACTORY_H
#define OBPROCESSORFACTORY_H

#include "OrderBookDef.h"

namespace oli
{

namespace obook
{

/// factory for OBProcessor 
class OBProcessorFactory final: public IOBookProcessorFactory
{
public:
    OBProcessorFactory();
    virtual ~OBProcessorFactory();

public:
    virtual IOBookEventProcessor *create()const override;
};

}

}

#endif // OBPROCESSORFACTORY_H
