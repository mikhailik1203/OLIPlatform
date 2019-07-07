#include "OBProcessorFactory.h"
#include "OBProcessor.h"

using namespace oli::obook;

OBProcessorFactory::OBProcessorFactory()
{
}

OBProcessorFactory::~OBProcessorFactory()
{
}

IOBookEventProcessor *OBProcessorFactory::create()const 
{
    return new OBProcessor;
}
