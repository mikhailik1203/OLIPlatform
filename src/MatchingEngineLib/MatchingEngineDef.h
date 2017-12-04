//
// Created by sam1203 on 9/18/17.
//

#ifndef MATCHINGENGINELIB_MATCHINGENGINEDEF_H
#define MATCHINGENGINELIB_MATCHINGENGINEDEF_H

#include "OLI_UtilsDefines.h"
#include "OLI_FinanceDomain.h"
#include <list>
#include <map>
#include <unordered_map>
#include <cassert>

namespace oli
{

    namespace mengine {

        struct OrderData{
            oli::idT  id_;
            oli::quantityT leftQty_;
            oli::priceT price_;
        };

        struct MarketOrderData{
            oli::idT  id_;
            oli::Side side_;
            oli::quantityT qty_;
            //oli::priceT price_;
        };

        struct AggressorOrderData{
            oli::idT  id_;
            oli::Side side_;
            oli::quantityT qty_;
            oli::priceT price_;
        };

        enum class MDUpdateType{
            invalid_MDUpdateType = 0,
            add_MDUpdateType,
            remove_MDUpdateType,
            update_MDUpdateType
        };
        struct MDLevel2Record{
            MDUpdateType type_;
            oli::idT  id_;
            oli::Side side_;
            oli::quantityT qty_;
            oli::priceT price_;
            //oli::OrderType ordType_;

            MDLevel2Record():
                    type_(MDUpdateType::invalid_MDUpdateType), id_(oli::INVALID_ID), side_(oli::Side::invalid_Side), qty_(0), price_(0)
            {}

            MDLevel2Record(MDUpdateType type, oli::idT  id, oli::Side side,
                           oli::quantityT qty, oli::priceT price):
                    type_(type), id_(id), side_(side), qty_(qty), price_(price)
            {}

        };

        struct MatchedOrderData{
            oli::idT bidId_;
            oli::idT askId_;
            oli::quantityT matchQty_;
            oli::priceT matchPx_;

            MatchedOrderData():
                    bidId_(oli::INVALID_ID), askId_(oli::INVALID_ID), matchQty_(0), matchPx_(0)
            {}

            MatchedOrderData(oli::idT bidId, oli::idT askId, oli::quantityT qty, oli::priceT px):
                    bidId_(bidId), askId_(askId), matchQty_(qty), matchPx_(px)
            {}
        };
        //typedef std::list<MatchedOrderData> MatchedOrdersT;
        class MatchedOrders{
            static const size_t MATCHED_ORDER_COUNT = 3;
            MatchedOrderData matched_[MATCHED_ORDER_COUNT];
            size_t matchedCount_ = 0;
            std::deque<MatchedOrderData> matchedOrders_;
        public:
            void push_back(const MatchedOrderData &orderdata){
                if(matchedCount_ < MATCHED_ORDER_COUNT){
                    matched_[matchedCount_] = orderdata;
                }else
                    matchedOrders_.push_back(orderdata);
                ++matchedCount_;
            }

            size_t size()const{return matchedCount_;}

            void reset(){
                matchedCount_ = 0;
                matchedOrders_.clear();
            }

            const MatchedOrderData &at(size_t idx)const{
                assert(idx < matchedCount_);
                if(idx < MATCHED_ORDER_COUNT){
                    return matched_[idx];
                }else
                    return matchedOrders_[idx - MATCHED_ORDER_COUNT];
            }
        };
        typedef MatchedOrders MatchedOrdersT;

        struct OrderMatchEvent{
            const MatchedOrdersT &matchData_;

            explicit OrderMatchEvent(const MatchedOrdersT &data): matchData_(data){}
        };

        typedef std::list<OrderData> OrderAtPriceLevelT;
        typedef std::map<priceT, OrderAtPriceLevelT, std::greater<priceT>> BidOrderBookT;
        typedef std::map<priceT, OrderAtPriceLevelT> AskOrderBookT;
        typedef std::unordered_map<oli::idT, OrderAtPriceLevelT::iterator> OrderId2PositionT; //for replace/delete operations

        /// interface for incoming MatchingEngine events related to OrderBook
        class IInMEOBookObserver{
        public:
            virtual ~IInMEOBookObserver(){}

            virtual void onOBookChanged(const MDLevel2Record &obdata) = 0;
            virtual void onOBooksChanged(const MDLevel2Record &askUpdate,
                                         const MDLevel2Record &bidUpdate) = 0;
            virtual void onOBooksReset(const oli::idT &instrId) = 0;
        };

        /// interface for incoming MatchingEngine events related to Order management
        class IInMEOrderObserver{
        public:
            virtual ~IInMEOrderObserver(){}

            virtual void onNewMarketOrder(const MarketOrderData &order) = 0;
            virtual void onNewOrderAggressor(const AggressorOrderData &order) = 0;
        };

        // interface for outgoing MatchingEngine events
        class IOutMEObserver{
        public:
            virtual ~IOutMEObserver(){}

            virtual void onOrdersMatched(const OrderMatchEvent &) = 0;
        };

        class IOrderBookFunctor{
        public:
            virtual ~IOrderBookFunctor(){}

            virtual void handle(BidOrderBookT &bidBook, AskOrderBookT &askBook) = 0;
        };

        class IOrderBookProcessor {
        public:
            virtual ~IOrderBookProcessor(){}

            virtual void process(IOrderBookFunctor *func)= 0;
        };
    }
}

#endif //MATCHINGENGINELIB_MATCHINGENGINEDEF_H
