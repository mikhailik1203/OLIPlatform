#ifndef OMSMESSAGEDISPATCHER_H
#define OMSMESSAGEDISPATCHER_H

#include "OLI_TransportLayerDef.h"
#include "MessagesLib/src/ClntMessages/ClntNewOrderRequestMsg.h"


namespace oli
{
namespace oms
{

    
/** Handles incoming messages from TL and dispatch processing to the corresponding component*/
class OMSMessageDispatcher: public tl::TLAppObserver
{
public:
    OMSMessageDispatcher();
    ~OMSMessageDispatcher();
    
public:
    /// implementation of the tl::TLAppObserver
    virtual void onMessage(const tl::TLDataSource &source, const msg::MessageBase &msg);

public:
    void process(const oli::msg::ClntNewOrderRequestMsg &msg);
};

}
}
#endif // OMSMESSAGEDISPATCHER_H
