//
// Created by sam1203 on 9/18/17.
//

#ifndef MATCHINGENGINELIB_METRANSPORTADAPTOR_H
#define MATCHINGENGINELIB_METRANSPORTADAPTOR_H

#include "OLI_TransportLayerDef.h"
#include "OLI_MsgDefines.h"
#include "MatchingEngineDef.h"

namespace oli {

    namespace mengine {
        class METransportAdaptor: public msg::AppMessageProcessor,
                    public  IOutMEObserver
        {
        public:
            METransportAdaptor(IInMEOBookObserver *obookObs, IInMEOrderObserver *orderObs);
            virtual ~METransportAdaptor();

        public:
            /// implementation of the msg::AppMessageProcessor
            virtual void onMessage(ConnectionIdT connId, const msg::ClntNewOrderRequestMsg *msg) override;
            virtual void onMessage(ConnectionIdT connId, const msg::ClntCancelOrderRequestMsg *msg) override;
            virtual void onMessage(ConnectionIdT connId, const msg::ClntReplaceOrderRequestMsg *msg) override;

            virtual void onMessage(ConnectionIdT connId, const msg::OMSNewOrderMsg *msg) override;
            virtual void onMessage(ConnectionIdT connId, const msg::OMSChangeOrderMsg *msg) override;
            virtual void onMessage(ConnectionIdT connId, const msg::OMSCancelOrderMsg *msg) override;
            virtual void onMessage(ConnectionIdT connId, const msg::OMSOrderStatusMsg *msg) override;
            virtual void onMessage(ConnectionIdT connId, const msg::OMSTradeRejectMsg *msg) override;
            virtual void onMessage(ConnectionIdT connId, const msg::OMSTradeCorrectRejectMsg *msg) override;

            virtual void onMessage(ConnectionIdT connId, const msg::OBBookChangeMsg *msg) override;
            virtual void onMessage(ConnectionIdT connId, const msg::OBBookChangesMsg *msg) override;

            virtual void onMessage(ConnectionIdT connId, const msg::MEOrdersMatchedMsg *msg) override;

        public:
            /// implementation of IOutMEObserver
            virtual void onOrdersMatched(const OrderMatchEvent &) override;

        private:
            IInMEOBookObserver *obookObs_;
            IInMEOrderObserver *orderObs_;
        };

    }
}

#endif //MATCHINGENGINELIB_METRANSPORTADAPTOR_H
