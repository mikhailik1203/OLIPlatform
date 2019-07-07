#ifndef OBTRANSPORTADAPTOR_H
#define OBTRANSPORTADAPTOR_H

#include "OLI_TransportLayerDef.h"
#include "OrderBookDef.h"

#include <atomic>

namespace oli
{

namespace obook
{

    
/// Encapsulates transport layer and transfers OBook types to the TL messages and back
class OBTransportAdaptor final: public IOrderBookEventObserver, 
                                public msg::AppMessageProcessor
{
public:
    OBTransportAdaptor();
    ~OBTransportAdaptor();
    
    void attachOBookObserver(IInOrderBookMessageObserver *obs);
    void detachOBookObserver();
    void attachInstrObserver(secmaster::IInstrumentMessageObserver *obs);
    void detachInstrObserver();

    void attachPublisher(oli::tl::TlMessagePublisher *publ);
    void detachPublisher();
public:
    /// implementation of the msg::AppMessageProcessor
    virtual void onMessage(ConnectionIdT connId, const msg::ClntNewOrderRequestMsg *msg) override;
    virtual void onMessage(ConnectionIdT connId, const msg::ClntCancelOrderRequestMsg *msg) override;
    virtual void onMessage(ConnectionIdT connId, const msg::ClntReplaceOrderRequestMsg *msg) override;
    
    virtual void onMessage(ConnectionIdT connId, const msg::OMSNewOrderMsg *msg) override;
    virtual void onMessage(ConnectionIdT connId, const msg::OMSChangeOrderMsg *msg) override;
    virtual void onMessage(ConnectionIdT connId, const msg::OMSCancelOrderMsg *msg) override;
    virtual void onMessage(ConnectionIdT connId, const msg::OMSOrderStatusMsg *msg) override;
        
    virtual void onMessage(ConnectionIdT connId, const msg::OBBookChangeMsg *msg) override;
    virtual void onMessage(ConnectionIdT connId, const msg::OBBookChangesMsg *msg) override;
    
    virtual void onMessage(ConnectionIdT connId, const msg::MEOrdersMatchedMsg *msg) override;
    
public:
    /// implementation of IOrderBookEventObserver
    virtual void onOBookChanged(const MDLevel2Record &rec) override;
    
private:
    std::atomic<IInOrderBookMessageObserver *> ordObs_;
    std::atomic<secmaster::IInstrumentMessageObserver *> instrObs_;
    std::atomic<oli::tl::TlMessagePublisher *> publisher_;
};

}

}

#endif // OBTRANSPORTADAPTOR_H
