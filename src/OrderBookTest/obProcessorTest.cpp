#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <boost/test/unit_test.hpp> 
#pragma GCC diagnostic pop

#include "OLI_Logger.h"
#include "OBProcessor.h"

#include <iostream>
#include <map>
#include <atomic>
#include <memory>


using namespace oli::obook;

namespace{

    class TestLogger final: public oli::Logger
    {
    public:
        TestLogger(){}
        virtual ~TestLogger(){}
        
        virtual void trace(const std::string &msg){
            BOOST_TEST_MESSAGE("[TRACE] "<< msg);
        }
        virtual void debug(const std::string &msg){
            BOOST_TEST_MESSAGE("[DEBUG] "<< msg);
        }
        virtual void note(const std::string &msg){
            BOOST_TEST_MESSAGE("[NOTE] "<< msg);
        }
        virtual void warn(const std::string &msg){
            BOOST_TEST_MESSAGE("[WARN] "<< msg);
        }
        virtual void error(const std::string &msg){
            BOOST_TEST_MESSAGE("[ERROR] "<< msg);
        }
        virtual void fatal(const std::string &msg){
            BOOST_TEST_MESSAGE("[FATAL] "<< msg);
        }
    };
    
   
    class TestOrderBookEventObserver final: public IOrderBookEventObserver{
    public:
        TestOrderBookEventObserver(){}
        virtual ~TestOrderBookEventObserver(){}

        virtual void onOrderToMatch(const MDLevel2Record &order)override
        {
            mdUpdates_.push_back(order);
        }

        virtual void onOBookChanged(const MDLevel2Record &upd)override
        {
            mdUpdates_.push_back(upd);
        }

        virtual void onOBooksChanged(const MDLevel2Record &askUpdate, 
                                     const MDLevel2Record &bidUpdate)override
        {
            mdUpdates_.push_back(askUpdate);
            mdUpdates_.push_back(bidUpdate);
        }
        
        virtual void onOBooksReset(const oli::idT &instrId) override
        {
            mdUpdates_.clear();
        }                                     
        
        void clearPublished(){mdUpdates_.clear();}
        
        size_t updatesPublished()const{return mdUpdates_.size();}
        
        MDLevel2Record popFrontUpdate(){
            if(mdUpdates_.empty())
                throw std::logic_error("TestOrderBookEventObserver::popFrontUpdate: no updates available!");
            MDLevel2Record val = mdUpdates_.front(); 
            mdUpdates_.pop_front();
            return val;
        }
    private:
        typedef std::deque<MDLevel2Record> MDLevelUpdatesT;
        MDLevelUpdatesT mdUpdates_;
    };    

    typedef std::deque<OrderBookRecord> OrdersT;
    OrdersT generateOrders(size_t count, oli::priceT lowPrice, oli::priceT highPrice, 
            oli::priceT priceStep, oli::quantityT lowQty, oli::quantityT highQty, 
            oli::quantityT qtyStep, oli::idT instrumentId)
    {
        std::srand(time(nullptr));

        OrdersT orders;
        OrderBookRecord orderData;
        orderData.instrumentId_ = instrumentId;

        oli::idT orderId = 1;
        size_t priceLevels = (highPrice - lowPrice)/priceStep + 1;
        size_t qtyLevels = (highQty - lowQty)/qtyStep + 1;
        for(size_t i = 0; i < count; ++i){
            orderData.orderId_ = ++orderId;
            
            int rvSide = rand()%100;
            orderData.side_ = (rvSide < 50)?(oli::Side::buy_Side):(oli::Side::sell_Side);

            size_t rvPrice = rand()%priceLevels;
            orderData.price_ = lowPrice + priceStep*rvPrice;

            size_t rvQty = rand()%qtyLevels;
            orderData.volumeLeft_ = lowQty + qtyStep*rvQty;            
            
            orders.push_back(orderData);
        }
        return orders;
    }
    
    void prepareOrderBooks(const OrdersT &orders, PriceLevelsMapT &bidPrices, PriceLevelsMapT &askPrices)
    {
        for(auto ord: orders){
            PriceLevelsMapT *prices = nullptr;
            if(oli::Side::buy_Side == ord.side_){
                prices = &bidPrices;
            }else{
                prices = &askPrices;
            }
            auto priceLvl = prices->find(ord.price_);
            if(prices->end() == priceLvl){
                PriceLevel lvl;
                lvl.price_ = ord.price_;
                priceLvl = prices->insert(PriceLevelsMapT::value_type(ord.price_, lvl)).first;
            }
            priceLvl->second.orders_.push_back(OrderParams(ord));
            priceLvl->second.volumeTotal_ += ord.volumeLeft_;
        }
    }
   
    void replaceOrder(OrdersT &ordersAfterChange, 
            const OrderBookRecord &ordBefore, const OrderBookRecord &ordChanged)
    {
        for(auto it = ordersAfterChange.begin(); it != ordersAfterChange.end(); ++it){
            if(it->orderId_ == ordBefore.orderId_){
                ordersAfterChange.erase(it);
                ordersAfterChange.push_back(ordChanged);
                return;
            }
        }
        throw std::logic_error("replaceOrder: Unable to change order - initial order wasn't found!");
    }
    
}

BOOST_AUTO_TEST_SUITE( OB_Processor )

BOOST_AUTO_TEST_CASE( AddOrder_VanillaScenario )
{
    //TestLogger logger;
    const oli::idT INSTRUMENTID = 5;
    TestOrderBookEventObserver updObserver;
    
    oli::priceT lowestPrice =   100000;
    oli::priceT highestPrice = 3000000;
    oli::priceT priceStep =       5000;
    oli::secmaster::InstrumentRecord instrument(INSTRUMENTID, "TestInstrument", 
                lowestPrice, highestPrice, priceStep);
    OBProcessor proc;
    proc.startProcessing(instrument);
                
    proc.attach(&updObserver);
    
    {
        OrderBookRecord orderData;
        orderData.orderId_ = 7;
        orderData.instrumentId_ = INSTRUMENTID;
        orderData.price_ = 125000;
        orderData.side_ = oli::Side::buy_Side;
        orderData.volumeLeft_ = 10000;
        
        /// add first order
        proc.onNewOrder(orderData);
        
        BOOST_REQUIRE(1 == updObserver.updatesPublished());
        MDLevel2Record updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(add_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderData.side_ == updRec.side_);
        BOOST_CHECK(orderData.price_ == updRec.price_);
        BOOST_CHECK(orderData.volumeLeft_ == updRec.volume_);
        
        OrderBookRecord sellOrderData;
        sellOrderData.orderId_ = 8;
        sellOrderData.instrumentId_ = INSTRUMENTID;
        sellOrderData.price_ = 160000;
        sellOrderData.side_ = oli::Side::sell_Side;
        sellOrderData.volumeLeft_ = 70000;
        
        /// add first sell order
        proc.onNewOrder(sellOrderData);
        
        BOOST_REQUIRE(1 == updObserver.updatesPublished());
        updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(add_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(sellOrderData.side_ == updRec.side_);
        BOOST_CHECK(sellOrderData.price_ == updRec.price_);
        BOOST_CHECK(sellOrderData.volumeLeft_ == updRec.volume_);

        OrderBookRecord orderData2;
        orderData2.orderId_ = 9;
        orderData2.instrumentId_ = INSTRUMENTID;
        orderData2.price_ = 125000;
        orderData2.side_ = oli::Side::buy_Side;
        orderData2.volumeLeft_ = 10000;
        
        /// add another buy order with same price level
        proc.onNewOrder(orderData2);
        
        BOOST_REQUIRE(1 == updObserver.updatesPublished());
        updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(add_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderData2.side_ == updRec.side_);
        BOOST_CHECK(orderData2.price_ == updRec.price_);
        BOOST_CHECK(orderData2.volumeLeft_ == updRec.volume_);

        OrderBookRecord orderData3;
        orderData3.orderId_ = 10;
        orderData3.instrumentId_ = INSTRUMENTID;
        orderData3.price_ = 145000;
        orderData3.side_ = oli::Side::buy_Side;
        orderData3.volumeLeft_ = 10000;
        
        /// add another buy order with same price level
        proc.onNewOrder(orderData3);
        
        BOOST_REQUIRE(1 == updObserver.updatesPublished());
        updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(add_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderData3.side_ == updRec.side_);
        BOOST_CHECK(orderData3.price_ == updRec.price_);
        BOOST_CHECK(orderData3.volumeLeft_ == updRec.volume_);
    }
}

BOOST_AUTO_TEST_CASE( AddOrder_randomOrder )
{
    TestLogger logger;
    const oli::idT INSTRUMENTID = 5;
    TestOrderBookEventObserver updObserver;
    
    oli::idT testInstrumentId = INSTRUMENTID;
    oli::priceT lowestPrice =   100000;
    oli::priceT highestPrice = 3000000;
    oli::priceT priceStep =       5000;

    oli::secmaster::InstrumentRecord instrument(testInstrumentId, "TestInstrument", 
                lowestPrice, highestPrice, priceStep);
    
    OBProcessor proc;
    proc.startProcessing(instrument);
                
    proc.attach(&updObserver);
    
    { 
        ///generate orders and push to the orderBook
        const oli::priceT LOWER_PRICE_SEQ = 140000;
        const oli::priceT HIGHER_PRICE_SEQ = 160000;
        const oli::priceT PRICE_STEP_SEQ = 5000;
        const oli::quantityT LOWER_QTY_SEQ = 1000;
        const oli::quantityT HIGHER_QTY_SEQ = 10000;
        const oli::quantityT QTY_STEP_SEQ = 500;
        const size_t ORDER_AMOUNT_SEQ = 200;
        
        OrdersT ordersSequence = generateOrders(ORDER_AMOUNT_SEQ, LOWER_PRICE_SEQ, HIGHER_PRICE_SEQ, 
                PRICE_STEP_SEQ, LOWER_QTY_SEQ, HIGHER_QTY_SEQ, QTY_STEP_SEQ, INSTRUMENTID);
        
        /// add orders in random order
        OrdersT addedOrders;
        while(!ordersSequence.empty()){
            size_t idx = rand()%ordersSequence.size();
            /// remove order from generated sequence and push back to added
            OrderBookRecord ord = ordersSequence[idx];
            OrdersT::iterator it2Del = ordersSequence.begin() + idx;
            ordersSequence.erase(it2Del);
            addedOrders.push_back(ord);
            
            proc.onNewOrder(ord);

            BOOST_REQUIRE(1 == updObserver.updatesPublished());
            MDLevel2Record updRec = updObserver.popFrontUpdate();
            BOOST_CHECK(add_MDLevelChangeType == updRec.type_);
            BOOST_CHECK(ord.side_ == updRec.side_);
            BOOST_CHECK(ord.price_ == updRec.price_);
            BOOST_CHECK(ord.volumeLeft_ == updRec.volume_);

            /// compare orderBooks
            PriceLevelsMapT bidPrices2Check, askPrices2Check;
            prepareOrderBooks(addedOrders, bidPrices2Check, askPrices2Check);

            // get results
            PriceLevelsT bidPriceLevels, askPriceLevels;
            proc.getOBSnapshot(-1, -1, bidPriceLevels, askPriceLevels);
            
            /// check results
            BOOST_REQUIRE(bidPrices2Check.size() == bidPriceLevels.size());
            for(auto bidOrd : bidPriceLevels){
                //std::cout<< "Search for price = "<< bidOrd.price_<< std::endl;
                auto bidOrdChk = bidPrices2Check.find(bidOrd.price_);
                BOOST_REQUIRE(bidOrdChk != bidPrices2Check.end());
                BOOST_REQUIRE(bidOrd.price_ == bidOrdChk->second.price_);
                BOOST_REQUIRE(bidOrd.volumeTotal_ == bidOrdChk->second.volumeTotal_);
                BOOST_REQUIRE(bidOrd.orders_.size() == bidOrdChk->second.orders_.size());
                BOOST_REQUIRE(true == equal(bidOrd.orders_.begin(), bidOrd.orders_.end(), bidOrdChk->second.orders_.begin()));
            }
            BOOST_REQUIRE(askPrices2Check.size() == askPriceLevels.size());
            for(auto askOrd : askPriceLevels){
                //std::cout<< "Search for ask price = "<< askOrd.price_<< std::endl;
                auto askOrdChk = askPrices2Check.find(askOrd.price_);
                BOOST_REQUIRE(askOrdChk != askPrices2Check.end());
                BOOST_REQUIRE(askOrd.price_ == askOrdChk->second.price_);
                BOOST_REQUIRE(askOrd.volumeTotal_ == askOrdChk->second.volumeTotal_);
                BOOST_REQUIRE(askOrd.orders_.size() == askOrdChk->second.orders_.size());
                BOOST_REQUIRE(true == equal(askOrd.orders_.begin(), askOrd.orders_.end(), askOrdChk->second.orders_.begin()));
            }

        }
    }
}


BOOST_AUTO_TEST_CASE( getOBSnapshot_EmptyBook )
{
    TestLogger logger;
    const oli::idT INSTRUMENTID = 5;
    TestOrderBookEventObserver updObserver;
    
    oli::idT testInstrumentId = INSTRUMENTID;
    oli::priceT lowestPrice =   100000;
    oli::priceT highestPrice = 3000000;
    oli::priceT priceStep =       5000;
    
    oli::secmaster::InstrumentRecord instrument(testInstrumentId, "TestInstrument", 
                lowestPrice, highestPrice, priceStep);
    
    OBProcessor proc;
    proc.startProcessing(instrument);                
    proc.attach(&updObserver);
    
    { // check getOBook for empty books
        PriceLevelsT bidPriceLevels, askPriceLevels;
        proc.getOBSnapshot(5, 5, bidPriceLevels, askPriceLevels);
        BOOST_REQUIRE(0 == bidPriceLevels.size());
        BOOST_REQUIRE(0 == askPriceLevels.size());
    }
}

BOOST_AUTO_TEST_CASE( getOBSnapshot_AddedGenerated )
{
    TestLogger logger;
    const oli::idT INSTRUMENTID = 5;
    TestOrderBookEventObserver updObserver;
    
    oli::idT testInstrumentId = INSTRUMENTID;
    oli::priceT lowestPrice =   100000;
    oli::priceT highestPrice = 3000000;
    oli::priceT priceStep =       5000;
    
    oli::secmaster::InstrumentRecord instrument(testInstrumentId, "TestInstrument", 
                lowestPrice, highestPrice, priceStep);
    
    OBProcessor proc;
    proc.startProcessing(instrument);                
    proc.attach(&updObserver);
    
    { 
        /// add several orders
        const oli::priceT LOWER_PRICE_SEQ = 120000;
        const oli::priceT HIGHER_PRICE_SEQ = 160000;
        const oli::priceT PRICE_STEP_SEQ = 5000;
        const oli::quantityT LOWER_QTY_SEQ = 1000;
        const oli::quantityT HIGHER_QTY_SEQ = 10000;
        const oli::quantityT QTY_STEP_SEQ = 500;
        const size_t ORDER_AMOUNT_SEQ = 1000;
        
        OrdersT ordersSequence = generateOrders(ORDER_AMOUNT_SEQ, LOWER_PRICE_SEQ, HIGHER_PRICE_SEQ, 
                PRICE_STEP_SEQ, LOWER_QTY_SEQ, HIGHER_QTY_SEQ, QTY_STEP_SEQ, INSTRUMENTID);
        
        PriceLevelsMapT bidPrices2Check, askPrices2Check;
        prepareOrderBooks(ordersSequence, bidPrices2Check, askPrices2Check);
        
        /// push orders to the OrderBook
        for(auto ord: ordersSequence){
            proc.onNewOrder(ord);
        }

        // get results
        PriceLevelsT bidPriceLevels, askPriceLevels;
        proc.getOBSnapshot(-1, -1, bidPriceLevels, askPriceLevels);
        
        /// check results
        BOOST_REQUIRE(bidPrices2Check.size() == bidPriceLevels.size());
        for(auto bidOrd : bidPriceLevels){
            //std::cout<< "Search for price = "<< bidOrd.price_<< std::endl;
            auto bidOrdChk = bidPrices2Check.find(bidOrd.price_);
            BOOST_REQUIRE(bidOrdChk != bidPrices2Check.end());
            BOOST_REQUIRE(bidOrd.price_ == bidOrdChk->second.price_);
            BOOST_REQUIRE(bidOrd.volumeTotal_ == bidOrdChk->second.volumeTotal_);
            BOOST_REQUIRE(bidOrd.orders_.size() == bidOrdChk->second.orders_.size());
            BOOST_REQUIRE(true == equal(bidOrd.orders_.begin(), bidOrd.orders_.end(), bidOrdChk->second.orders_.begin()));
        }
        BOOST_REQUIRE(askPrices2Check.size() == askPriceLevels.size());
        for(auto askOrd : askPriceLevels){
            //std::cout<< "Search for ask price = "<< askOrd.price_<< std::endl;
            auto askOrdChk = askPrices2Check.find(askOrd.price_);
            BOOST_REQUIRE(askOrdChk != askPrices2Check.end());
            BOOST_REQUIRE(askOrd.price_ == askOrdChk->second.price_);
            BOOST_REQUIRE(askOrd.volumeTotal_ == askOrdChk->second.volumeTotal_);
            BOOST_REQUIRE(askOrd.orders_.size() == askOrdChk->second.orders_.size());
            BOOST_REQUIRE(true == equal(askOrd.orders_.begin(), askOrd.orders_.end(), askOrdChk->second.orders_.begin()));
        }
        
    }
    
}

BOOST_AUTO_TEST_CASE( RemoveOrder_FIFOOrder )
{
    TestLogger logger;
    const oli::idT INSTRUMENTID = 5;
    TestOrderBookEventObserver updObserver;
    
    oli::idT testInstrumentId = INSTRUMENTID;
    oli::priceT lowestPrice =   100000;
    oli::priceT highestPrice = 3000000;
    oli::priceT priceStep =       5000;
    
    oli::secmaster::InstrumentRecord instrument(testInstrumentId, "TestInstrument", 
                lowestPrice, highestPrice, priceStep);
    
    OBProcessor proc;
    proc.startProcessing(instrument);
    proc.attach(&updObserver);
    
    { 
        ///generate orders and push to the orderBook
        const oli::priceT LOWER_PRICE_SEQ = 140000;
        const oli::priceT HIGHER_PRICE_SEQ = 160000;
        const oli::priceT PRICE_STEP_SEQ = 5000;
        const oli::quantityT LOWER_QTY_SEQ = 1000;
        const oli::quantityT HIGHER_QTY_SEQ = 10000;
        const oli::quantityT QTY_STEP_SEQ = 500;
        const size_t ORDER_AMOUNT_SEQ = 200;
        
        OrdersT ordersSequence = generateOrders(ORDER_AMOUNT_SEQ, LOWER_PRICE_SEQ, HIGHER_PRICE_SEQ, 
                PRICE_STEP_SEQ, LOWER_QTY_SEQ, HIGHER_QTY_SEQ, QTY_STEP_SEQ, INSTRUMENTID);
        
        PriceLevelsMapT bidPrices2Check, askPrices2Check;
        prepareOrderBooks(ordersSequence, bidPrices2Check, askPrices2Check);
        
        for(auto ord: ordersSequence){
            proc.onNewOrder(ord);
        }

        BOOST_REQUIRE(ordersSequence.size() == updObserver.updatesPublished());
        updObserver.clearPublished();

        /// remove orders from first till last
        while(!ordersSequence.empty()){
            OrderBookRecord ord = ordersSequence.front();
            ordersSequence.erase(ordersSequence.begin());
            proc.onDeleteOrder(ord);

            BOOST_REQUIRE(1 == updObserver.updatesPublished());
            MDLevel2Record updRec = updObserver.popFrontUpdate();
            BOOST_CHECK(delete_MDLevelChangeType == updRec.type_);
            BOOST_CHECK(ord.side_ == updRec.side_);
            BOOST_CHECK(ord.price_ == updRec.price_);
            BOOST_CHECK(ord.volumeLeft_ == updRec.volume_);

            /// compare orderBooks
            PriceLevelsMapT bidPrices2Check, askPrices2Check;
            prepareOrderBooks(ordersSequence, bidPrices2Check, askPrices2Check);

            // get results
            PriceLevelsT bidPriceLevels, askPriceLevels;
            proc.getOBSnapshot(-1, -1, bidPriceLevels, askPriceLevels);
            
            /// check results
            BOOST_REQUIRE(bidPrices2Check.size() == bidPriceLevels.size());
            for(auto bidOrd : bidPriceLevels){
                //std::cout<< "Search for price = "<< bidOrd.price_<< std::endl;
                auto bidOrdChk = bidPrices2Check.find(bidOrd.price_);
                BOOST_REQUIRE(bidOrdChk != bidPrices2Check.end());
                BOOST_REQUIRE(bidOrd.price_ == bidOrdChk->second.price_);
                BOOST_REQUIRE(bidOrd.volumeTotal_ == bidOrdChk->second.volumeTotal_);
                BOOST_REQUIRE(bidOrd.orders_.size() == bidOrdChk->second.orders_.size());
                BOOST_REQUIRE(true == equal(bidOrd.orders_.begin(), bidOrd.orders_.end(), bidOrdChk->second.orders_.begin()));
            }
            BOOST_REQUIRE(askPrices2Check.size() == askPriceLevels.size());
            for(auto askOrd : askPriceLevels){
                //std::cout<< "Search for ask price = "<< askOrd.price_<< std::endl;
                auto askOrdChk = askPrices2Check.find(askOrd.price_);
                BOOST_REQUIRE(askOrdChk != askPrices2Check.end());
                BOOST_REQUIRE(askOrd.price_ == askOrdChk->second.price_);
                BOOST_REQUIRE(askOrd.volumeTotal_ == askOrdChk->second.volumeTotal_);
                BOOST_REQUIRE(askOrd.orders_.size() == askOrdChk->second.orders_.size());
                BOOST_REQUIRE(true == equal(askOrd.orders_.begin(), askOrd.orders_.end(), askOrdChk->second.orders_.begin()));
            }

        }
        
        
        
    }
    
}

BOOST_AUTO_TEST_CASE( RemoveOrder_FILOOrder )
{
    TestLogger logger;
    const oli::idT INSTRUMENTID = 5;
    TestOrderBookEventObserver updObserver;
    
    oli::idT testInstrumentId = INSTRUMENTID;
    oli::priceT lowestPrice =   100000;
    oli::priceT highestPrice = 3000000;
    oli::priceT priceStep =       5000;
    
    oli::secmaster::InstrumentRecord instrument(testInstrumentId, "TestInstrument", 
                lowestPrice, highestPrice, priceStep);
    
    OBProcessor proc;
    proc.startProcessing(instrument); 
    proc.attach(&updObserver);
    
    { 
        ///generate orders and push to the orderBook
        const oli::priceT LOWER_PRICE_SEQ = 140000;
        const oli::priceT HIGHER_PRICE_SEQ = 160000;
        const oli::priceT PRICE_STEP_SEQ = 5000;
        const oli::quantityT LOWER_QTY_SEQ = 1000;
        const oli::quantityT HIGHER_QTY_SEQ = 10000;
        const oli::quantityT QTY_STEP_SEQ = 500;
        const size_t ORDER_AMOUNT_SEQ = 200;
        
        OrdersT ordersSequence = generateOrders(ORDER_AMOUNT_SEQ, LOWER_PRICE_SEQ, HIGHER_PRICE_SEQ, 
                PRICE_STEP_SEQ, LOWER_QTY_SEQ, HIGHER_QTY_SEQ, QTY_STEP_SEQ, INSTRUMENTID);
        
        PriceLevelsMapT bidPrices2Check, askPrices2Check;
        prepareOrderBooks(ordersSequence, bidPrices2Check, askPrices2Check);
        
        for(auto ord: ordersSequence){
            proc.onNewOrder(ord);
        }

        BOOST_REQUIRE(ordersSequence.size() == updObserver.updatesPublished());
        updObserver.clearPublished();

        /// remove orders from last till first
        while(!ordersSequence.empty()){
            OrderBookRecord ord = *ordersSequence.rbegin();
            ordersSequence.erase(ordersSequence.begin() + (ordersSequence.size() - 1));
            proc.onDeleteOrder(ord);

            BOOST_REQUIRE(1 == updObserver.updatesPublished());
            MDLevel2Record updRec = updObserver.popFrontUpdate();
            BOOST_CHECK(delete_MDLevelChangeType == updRec.type_);
            BOOST_CHECK(ord.side_ == updRec.side_);
            BOOST_CHECK(ord.price_ == updRec.price_);
            BOOST_CHECK(ord.volumeLeft_ == updRec.volume_);

            /// compare orderBooks
            PriceLevelsMapT bidPrices2Check, askPrices2Check;
            prepareOrderBooks(ordersSequence, bidPrices2Check, askPrices2Check);

            // get results
            PriceLevelsT bidPriceLevels, askPriceLevels;
            proc.getOBSnapshot(-1, -1, bidPriceLevels, askPriceLevels);
            
            /// check results
            BOOST_REQUIRE(bidPrices2Check.size() == bidPriceLevels.size());
            for(auto bidOrd : bidPriceLevels){
                //std::cout<< "Search for price = "<< bidOrd.price_<< std::endl;
                auto bidOrdChk = bidPrices2Check.find(bidOrd.price_);
                BOOST_REQUIRE(bidOrdChk != bidPrices2Check.end());
                BOOST_REQUIRE(bidOrd.price_ == bidOrdChk->second.price_);
                BOOST_REQUIRE(bidOrd.volumeTotal_ == bidOrdChk->second.volumeTotal_);
                BOOST_REQUIRE(bidOrd.orders_.size() == bidOrdChk->second.orders_.size());
                BOOST_REQUIRE(true == equal(bidOrd.orders_.begin(), bidOrd.orders_.end(), bidOrdChk->second.orders_.begin()));
            }
            BOOST_REQUIRE(askPrices2Check.size() == askPriceLevels.size());
            for(auto askOrd : askPriceLevels){
                //std::cout<< "Search for ask price = "<< askOrd.price_<< std::endl;
                auto askOrdChk = askPrices2Check.find(askOrd.price_);
                BOOST_REQUIRE(askOrdChk != askPrices2Check.end());
                BOOST_REQUIRE(askOrd.price_ == askOrdChk->second.price_);
                BOOST_REQUIRE(askOrd.volumeTotal_ == askOrdChk->second.volumeTotal_);
                BOOST_REQUIRE(askOrd.orders_.size() == askOrdChk->second.orders_.size());
                BOOST_REQUIRE(true == equal(askOrd.orders_.begin(), askOrd.orders_.end(), askOrdChk->second.orders_.begin()));
            }

        }
        
        
        
    }
    
}

BOOST_AUTO_TEST_CASE( RemoveOrder_randomOrder )
{
    TestLogger logger;
    const oli::idT INSTRUMENTID = 5;
    TestOrderBookEventObserver updObserver;
    
    oli::idT testInstrumentId = INSTRUMENTID;
    oli::priceT lowestPrice =   100000;
    oli::priceT highestPrice = 3000000;
    oli::priceT priceStep =       5000;
    
    oli::secmaster::InstrumentRecord instrument(testInstrumentId, "TestInstrument", 
                lowestPrice, highestPrice, priceStep);
    
    OBProcessor proc;
    proc.startProcessing(instrument);                
    proc.attach(&updObserver);
    
    { 
        ///generate orders and push to the orderBook
        const oli::priceT LOWER_PRICE_SEQ = 140000;
        const oli::priceT HIGHER_PRICE_SEQ = 160000;
        const oli::priceT PRICE_STEP_SEQ = 5000;
        const oli::quantityT LOWER_QTY_SEQ = 1000;
        const oli::quantityT HIGHER_QTY_SEQ = 10000;
        const oli::quantityT QTY_STEP_SEQ = 500;
        const size_t ORDER_AMOUNT_SEQ = 200;
        
        OrdersT ordersSequence = generateOrders(ORDER_AMOUNT_SEQ, LOWER_PRICE_SEQ, HIGHER_PRICE_SEQ, 
                PRICE_STEP_SEQ, LOWER_QTY_SEQ, HIGHER_QTY_SEQ, QTY_STEP_SEQ, INSTRUMENTID);
        
        PriceLevelsMapT bidPrices2Check, askPrices2Check;
        prepareOrderBooks(ordersSequence, bidPrices2Check, askPrices2Check);
        
        for(auto ord: ordersSequence){
            proc.onNewOrder(ord);
        }

        BOOST_REQUIRE(ordersSequence.size() == updObserver.updatesPublished());
        updObserver.clearPublished();


        /// remove orders in random order
        while(!ordersSequence.empty()){
            size_t idx = rand()%ordersSequence.size();
            OrderBookRecord ord = ordersSequence[idx];
            OrdersT::iterator it2Del = ordersSequence.begin() + idx;
            ordersSequence.erase(it2Del);
            proc.onDeleteOrder(ord);

            BOOST_REQUIRE(1 == updObserver.updatesPublished());
            MDLevel2Record updRec = updObserver.popFrontUpdate();
            BOOST_CHECK(delete_MDLevelChangeType == updRec.type_);
            BOOST_CHECK(ord.side_ == updRec.side_);
            BOOST_CHECK(ord.price_ == updRec.price_);
            BOOST_CHECK(ord.volumeLeft_ == updRec.volume_);

            /// compare orderBooks
            PriceLevelsMapT bidPrices2Check, askPrices2Check;
            prepareOrderBooks(ordersSequence, bidPrices2Check, askPrices2Check);

            // get results
            PriceLevelsT bidPriceLevels, askPriceLevels;
            proc.getOBSnapshot(-1, -1, bidPriceLevels, askPriceLevels);
            
            /// check results
            BOOST_REQUIRE(bidPrices2Check.size() == bidPriceLevels.size());
            for(auto bidOrd : bidPriceLevels){
                //std::cout<< "Search for price = "<< bidOrd.price_<< std::endl;
                auto bidOrdChk = bidPrices2Check.find(bidOrd.price_);
                BOOST_REQUIRE(bidOrdChk != bidPrices2Check.end());
                BOOST_REQUIRE(bidOrd.price_ == bidOrdChk->second.price_);
                BOOST_REQUIRE(bidOrd.volumeTotal_ == bidOrdChk->second.volumeTotal_);
                BOOST_REQUIRE(bidOrd.orders_.size() == bidOrdChk->second.orders_.size());
                BOOST_REQUIRE(true == equal(bidOrd.orders_.begin(), bidOrd.orders_.end(), bidOrdChk->second.orders_.begin()));
            }
            BOOST_REQUIRE(askPrices2Check.size() == askPriceLevels.size());
            for(auto askOrd : askPriceLevels){
                //std::cout<< "Search for ask price = "<< askOrd.price_<< std::endl;
                auto askOrdChk = askPrices2Check.find(askOrd.price_);
                BOOST_REQUIRE(askOrdChk != askPrices2Check.end());
                BOOST_REQUIRE(askOrd.price_ == askOrdChk->second.price_);
                BOOST_REQUIRE(askOrd.volumeTotal_ == askOrdChk->second.volumeTotal_);
                BOOST_REQUIRE(askOrd.orders_.size() == askOrdChk->second.orders_.size());
                BOOST_REQUIRE(true == equal(askOrd.orders_.begin(), askOrd.orders_.end(), askOrdChk->second.orders_.begin()));
            }
        }
    }
}

BOOST_AUTO_TEST_CASE( ChangeOrder_vanilla )
{
    TestLogger logger;
    const oli::idT INSTRUMENTID = 5;
    TestOrderBookEventObserver updObserver;
    
    oli::idT testInstrumentId = INSTRUMENTID;
    oli::priceT lowestPrice =   100000;
    oli::priceT highestPrice = 3000000;
    oli::priceT priceStep =       5000;
    
    oli::secmaster::InstrumentRecord instrument(testInstrumentId, "TestInstrument", 
                lowestPrice, highestPrice, priceStep);
    
    OBProcessor proc;
    proc.startProcessing(instrument);
    proc.attach(&updObserver);
    
    { // change Qty
        OrderBookRecord orderDataBefore;
        orderDataBefore.orderId_ = 7;
        orderDataBefore.instrumentId_ = INSTRUMENTID;
        orderDataBefore.price_ = 125000;
        orderDataBefore.side_ = oli::Side::buy_Side;
        orderDataBefore.volumeLeft_ = 10000;

        proc.onNewOrder(orderDataBefore);
        BOOST_REQUIRE(1 == updObserver.updatesPublished());
        updObserver.clearPublished();
        
        OrderBookRecord orderDataAfter = orderDataBefore;
        orderDataAfter.orderId_ = 8;
        orderDataAfter.volumeLeft_ = 15000;
        proc.onChangeOrder(orderDataBefore, orderDataAfter);
        BOOST_REQUIRE(1 == updObserver.updatesPublished());
        MDLevel2Record updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(change_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderDataAfter.side_ == updRec.side_);
        BOOST_CHECK(orderDataAfter.price_ == updRec.price_);
        BOOST_CHECK(orderDataAfter.volumeLeft_ == updRec.volume_);
        BOOST_CHECK(orderDataBefore.price_ == updRec.prevPrice_);
        BOOST_CHECK(orderDataBefore.volumeLeft_ == updRec.prevVolume_);

    }

    { // change Price
        OrderBookRecord orderDataBefore;
        orderDataBefore.orderId_ = 9;
        orderDataBefore.instrumentId_ = INSTRUMENTID;
        orderDataBefore.price_ = 125000;
        orderDataBefore.side_ = oli::Side::buy_Side;
        orderDataBefore.volumeLeft_ = 10000;

        proc.onNewOrder(orderDataBefore);
        BOOST_REQUIRE(1 == updObserver.updatesPublished());
        updObserver.clearPublished();
        
        OrderBookRecord orderDataAfter = orderDataBefore;
        orderDataAfter.orderId_ = 10;
        orderDataAfter.price_ = 127000;
        proc.onChangeOrder(orderDataBefore, orderDataAfter);
        BOOST_REQUIRE(1 == updObserver.updatesPublished());
        MDLevel2Record updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(change_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderDataAfter.side_ == updRec.side_);
        BOOST_CHECK(orderDataAfter.price_ == updRec.price_);
        BOOST_CHECK(orderDataAfter.volumeLeft_ == updRec.volume_);
        BOOST_CHECK(orderDataBefore.price_ == updRec.prevPrice_);
        BOOST_CHECK(orderDataBefore.volumeLeft_ == updRec.prevVolume_);

    }

    { // change Price and Qty
        OrderBookRecord orderDataBefore;
        orderDataBefore.orderId_ = 9;
        orderDataBefore.instrumentId_ = INSTRUMENTID;
        orderDataBefore.price_ = 125000;
        orderDataBefore.side_ = oli::Side::buy_Side;
        orderDataBefore.volumeLeft_ = 10000;

        proc.onNewOrder(orderDataBefore);
        BOOST_REQUIRE(1 == updObserver.updatesPublished());
        updObserver.clearPublished();
        
        OrderBookRecord orderDataAfter = orderDataBefore;
        orderDataAfter.orderId_ = 10;
        orderDataAfter.price_ = 135000;
        orderDataAfter.volumeLeft_ = 12000;
        proc.onChangeOrder(orderDataBefore, orderDataAfter);
        BOOST_REQUIRE(1 == updObserver.updatesPublished());
        MDLevel2Record updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(change_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderDataAfter.side_ == updRec.side_);
        BOOST_CHECK(orderDataAfter.price_ == updRec.price_);
        BOOST_CHECK(orderDataAfter.volumeLeft_ == updRec.volume_);
        BOOST_CHECK(orderDataBefore.price_ == updRec.prevPrice_);
        BOOST_CHECK(orderDataBefore.volumeLeft_ == updRec.prevVolume_);

    }

    { // change of non exist order - exception thrown
        OrderBookRecord orderNExist;
        orderNExist.orderId_ = 777;
        orderNExist.instrumentId_ = INSTRUMENTID;
        orderNExist.price_ = 125000;
        orderNExist.side_ = oli::Side::buy_Side;
        orderNExist.volumeLeft_ = 10000;

        OrderBookRecord orderDataAfter = orderNExist;
        orderDataAfter.orderId_ = 778;
        orderDataAfter.price_ = 135000;
        orderDataAfter.volumeLeft_ = 12000;
        BOOST_CHECK_THROW(proc.onChangeOrder(orderNExist, orderDataAfter), std::logic_error);
        BOOST_REQUIRE(0 == updObserver.updatesPublished());
    }
    
}

BOOST_AUTO_TEST_CASE( ChangeOrder_randomOrder )
{
    TestLogger logger;
    const oli::idT INSTRUMENTID = 5;
    TestOrderBookEventObserver updObserver;
    
    oli::idT testInstrumentId = INSTRUMENTID;
    oli::priceT lowestPrice =   100000;
    oli::priceT highestPrice = 3000000;
    oli::priceT priceStep =       5000;
    
    oli::secmaster::InstrumentRecord instrument(testInstrumentId, "TestInstrument", 
                lowestPrice, highestPrice, priceStep);
    
    OBProcessor proc;
                
    proc.attach(&updObserver);
    proc.startProcessing(instrument);    
    { 
        ///generate orders and push to the orderBook
        const oli::priceT LOWER_PRICE_SEQ = 140000;
        const oli::priceT HIGHER_PRICE_SEQ = 160000;
        const oli::priceT PRICE_STEP_SEQ = 5000;
        const oli::quantityT LOWER_QTY_SEQ = 1000;
        const oli::quantityT HIGHER_QTY_SEQ = 10000;
        const oli::quantityT QTY_STEP_SEQ = 500;
        const size_t ORDER_AMOUNT_SEQ = 400;
        
        OrdersT ordersSequence = generateOrders(ORDER_AMOUNT_SEQ, LOWER_PRICE_SEQ, HIGHER_PRICE_SEQ, 
                PRICE_STEP_SEQ, LOWER_QTY_SEQ, HIGHER_QTY_SEQ, QTY_STEP_SEQ, INSTRUMENTID);
        
        PriceLevelsMapT bidPrices2Check, askPrices2Check;
        prepareOrderBooks(ordersSequence, bidPrices2Check, askPrices2Check);
        
        OrdersT orders2Add(ordersSequence.begin(), 
                           ordersSequence.begin() + ORDER_AMOUNT_SEQ/2);
        OrdersT orders4Change(ordersSequence.begin() + (ORDER_AMOUNT_SEQ/2),
                              ordersSequence.end());
        for(auto ord: orders2Add){
            proc.onNewOrder(ord);
        }

        BOOST_REQUIRE(orders2Add.size() == updObserver.updatesPublished());
        updObserver.clearPublished();

        OrdersT ordersSequenceAfterChange = orders2Add;
        /// change orders in random order
        while(!orders2Add.empty()){
            size_t idx = rand()%orders2Add.size();
            OrderBookRecord ordBefore = orders2Add[idx];
            OrdersT::iterator it2Del = orders2Add.begin() + idx;
            orders2Add.erase(it2Del);
            
            OrderBookRecord ordChanged = orders4Change.front();
            ordChanged.side_ = ordBefore.side_;
            if(ordChanged.volumeLeft_ == ordBefore.volumeLeft_)
                ordChanged.volumeLeft_ += 1;
            orders4Change.pop_front();
            replaceOrder(ordersSequenceAfterChange, ordBefore, ordChanged);
            
            proc.onChangeOrder(ordBefore, ordChanged);

            BOOST_REQUIRE(1 == updObserver.updatesPublished());
            MDLevel2Record updRec = updObserver.popFrontUpdate();
            BOOST_CHECK(change_MDLevelChangeType == updRec.type_);
            BOOST_CHECK(ordChanged.side_ == updRec.side_);
            BOOST_CHECK(ordChanged.price_ == updRec.price_);
            BOOST_CHECK(ordChanged.volumeLeft_ == updRec.volume_);
            BOOST_CHECK(ordBefore.price_ == updRec.prevPrice_);
            BOOST_CHECK(ordBefore.volumeLeft_ == updRec.prevVolume_);

            /// compare orderBooks
            PriceLevelsMapT bidPrices2Check, askPrices2Check;
            prepareOrderBooks(ordersSequenceAfterChange, bidPrices2Check, askPrices2Check);

            // get result OB
            PriceLevelsT bidPriceLevels, askPriceLevels;
            proc.getOBSnapshot(-1, -1, bidPriceLevels, askPriceLevels);
            
            /// check results
            BOOST_REQUIRE(bidPrices2Check.size() == bidPriceLevels.size());
            for(auto bidOrd : bidPriceLevels){
                //std::cout<< "Search for price = "<< bidOrd.price_<< std::endl;
                auto bidOrdChk = bidPrices2Check.find(bidOrd.price_);
                BOOST_REQUIRE(bidOrdChk != bidPrices2Check.end());
                BOOST_REQUIRE(bidOrd.price_ == bidOrdChk->second.price_);
                BOOST_REQUIRE(bidOrd.volumeTotal_ == bidOrdChk->second.volumeTotal_);
                BOOST_REQUIRE(bidOrd.orders_.size() == bidOrdChk->second.orders_.size());
                BOOST_REQUIRE(true == equal(bidOrd.orders_.begin(), bidOrd.orders_.end(), bidOrdChk->second.orders_.begin()));
            }
            BOOST_REQUIRE(askPrices2Check.size() == askPriceLevels.size());
            for(auto askOrd : askPriceLevels){
                //std::cout<< "Search for ask price = "<< askOrd.price_<< std::endl;
                auto askOrdChk = askPrices2Check.find(askOrd.price_);
                BOOST_REQUIRE(askOrdChk != askPrices2Check.end());
                BOOST_REQUIRE(askOrd.price_ == askOrdChk->second.price_);
                BOOST_REQUIRE(askOrd.volumeTotal_ == askOrdChk->second.volumeTotal_);
                BOOST_REQUIRE(askOrd.orders_.size() == askOrdChk->second.orders_.size());
                BOOST_REQUIRE(true == equal(askOrd.orders_.begin(), askOrd.orders_.end(), askOrdChk->second.orders_.begin()));
            }
        }
    }
}

BOOST_AUTO_TEST_CASE( TradeOrders_vanilla )
{
    TestLogger logger;
    const oli::idT INSTRUMENTID = 5;
    TestOrderBookEventObserver updObserver;
    
    oli::idT testInstrumentId = INSTRUMENTID;
    oli::priceT lowestPrice =   100000;
    oli::priceT highestPrice = 3000000;
    oli::priceT priceStep =       5000;
    
    oli::secmaster::InstrumentRecord instrument(testInstrumentId, "TestInstrument", 
                lowestPrice, highestPrice, priceStep);
    
    OBProcessor proc;
    oli::idT orderId = 6;
                
    proc.attach(&updObserver);
    proc.startProcessing(instrument);
    
    /// prepare orderBook for trades
    OrderBookRecord orderBid0(++orderId, INSTRUMENTID, 125000, 1000, oli::Side::buy_Side);
    OrderBookRecord orderAsk0(++orderId, INSTRUMENTID, 125000, 1000, oli::Side::sell_Side);
    OrderBookRecord orderBid1(++orderId, INSTRUMENTID, 125000, 10000, oli::Side::buy_Side);
    OrderBookRecord orderAsk1(++orderId, INSTRUMENTID, 125000, 5000, oli::Side::sell_Side);
    OrderBookRecord orderBid2(++orderId, INSTRUMENTID, 125000, 70000, oli::Side::buy_Side);
    OrderBookRecord orderAsk2(++orderId, INSTRUMENTID, 125000, 15000, oli::Side::sell_Side);


    proc.onNewOrder(orderBid0);
    proc.onNewOrder(orderAsk0);
    proc.onNewOrder(orderBid1);
    proc.onNewOrder(orderAsk1);
    proc.onNewOrder(orderBid2);
    proc.onNewOrder(orderAsk2);
    BOOST_REQUIRE(6 == updObserver.updatesPublished());
    updObserver.clearPublished();

    { // trade to close bid and ask orders
        oli::quantityT matchedQty = 1000;
        TradeRecord trade(orderBid0.orderId_, orderBid0.price_, 
                orderAsk0.orderId_, orderAsk0.price_, matchedQty);

        proc.onTrade(trade);
        BOOST_REQUIRE(2 == updObserver.updatesPublished());
        
        MDLevel2Record updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(delete_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderAsk0.side_ == updRec.side_);
        BOOST_CHECK(orderAsk0.price_ == updRec.price_);
        BOOST_CHECK(0 == updRec.volume_);
        BOOST_CHECK(0 == updRec.prevPrice_);
        BOOST_CHECK(0 == updRec.prevVolume_);
        
        updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(delete_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderBid0.side_ == updRec.side_);
        BOOST_CHECK(orderBid0.price_ == updRec.price_);
        BOOST_CHECK(0 == updRec.volume_);
        BOOST_CHECK(0 == updRec.prevPrice_);
        BOOST_CHECK(0 == updRec.prevVolume_);
    }

    oli::quantityT matchedQty1 = 5000;
    { // first trade to close ask order and partfill bid order
        TradeRecord trade1(orderBid1.orderId_, orderBid1.price_, 
                orderAsk1.orderId_, orderAsk1.price_, matchedQty1);

        proc.onTrade(trade1);
        BOOST_REQUIRE(2 == updObserver.updatesPublished());
        
        MDLevel2Record updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(delete_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderAsk1.side_ == updRec.side_);
        BOOST_CHECK(orderAsk1.price_ == updRec.price_);
        BOOST_CHECK(0 == updRec.volume_);
        BOOST_CHECK(0 == updRec.prevPrice_);
        BOOST_CHECK(0 == updRec.prevVolume_);
        
        updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(change_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderBid1.side_ == updRec.side_);
        BOOST_CHECK(orderBid1.price_ == updRec.price_);
        BOOST_CHECK(orderBid1.volumeLeft_ - matchedQty1 == updRec.volume_);
        BOOST_CHECK(orderBid1.price_ == updRec.prevPrice_);
        BOOST_CHECK(orderBid1.volumeLeft_ == updRec.prevVolume_);
    }

    oli::quantityT matchedQty2 = 5000;
    {// trade to close bid order and partfill ask order
        TradeRecord trade2(orderBid1.orderId_, orderBid1.price_, 
                orderAsk2.orderId_, orderAsk2.price_, matchedQty2);

        proc.onTrade(trade2);
        BOOST_REQUIRE(2 == updObserver.updatesPublished());
        
        MDLevel2Record updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(change_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderAsk2.side_ == updRec.side_);
        BOOST_CHECK(orderAsk2.price_ == updRec.price_);
        BOOST_CHECK(orderAsk2.volumeLeft_ - matchedQty1 == updRec.volume_);
        BOOST_CHECK(orderAsk2.price_ == updRec.prevPrice_);
        BOOST_CHECK(orderAsk2.volumeLeft_ == updRec.prevVolume_);

        updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(delete_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderBid1.side_ == updRec.side_);
        BOOST_CHECK(orderBid1.price_ == updRec.price_);
        BOOST_CHECK(0 == updRec.volume_);
        BOOST_CHECK(0 == updRec.prevPrice_);
        BOOST_CHECK(0 == updRec.prevVolume_);
    }
    
    {// trade with not exist bid order 
        TradeRecord trade3(orderBid1.orderId_, orderBid1.price_, 
                orderAsk2.orderId_, orderAsk2.price_, matchedQty2);

        BOOST_CHECK_THROW(proc.onTrade(trade3), std::logic_error);
        BOOST_REQUIRE(0 == updObserver.updatesPublished());
    }
    {// trade with not exist ask order 
        TradeRecord trade4(orderBid2.orderId_, orderBid2.price_, 
                orderAsk1.orderId_, orderAsk1.price_, matchedQty2);

        BOOST_CHECK_THROW(proc.onTrade(trade4), std::logic_error);
        BOOST_REQUIRE(0 == updObserver.updatesPublished());
    }
    
    {// second trade to partfill bid and ask orders
        oli::quantityT matchedQty3 = 1000;
        TradeRecord trade5(orderBid2.orderId_, orderBid2.price_, 
                orderAsk2.orderId_, orderAsk2.price_, matchedQty3);

        proc.onTrade(trade5);
        BOOST_REQUIRE(2 == updObserver.updatesPublished());
        
        MDLevel2Record updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(change_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderAsk2.side_ == updRec.side_);
        BOOST_CHECK(orderAsk2.price_ == updRec.price_);
        BOOST_CHECK(orderAsk2.volumeLeft_ - matchedQty3 - matchedQty2 == updRec.volume_);
        BOOST_CHECK(orderAsk2.price_ == updRec.prevPrice_);
        BOOST_CHECK(orderAsk2.volumeLeft_ - matchedQty2 == updRec.prevVolume_);

        updRec = updObserver.popFrontUpdate();
        BOOST_CHECK(change_MDLevelChangeType == updRec.type_);
        BOOST_CHECK(orderBid2.side_ == updRec.side_);
        BOOST_CHECK(orderBid2.price_ == updRec.price_);
        BOOST_CHECK(orderBid2.volumeLeft_ - matchedQty3 == updRec.volume_);
        BOOST_CHECK(orderBid2.price_ == updRec.prevPrice_);
        BOOST_CHECK(orderBid2.volumeLeft_ == updRec.prevVolume_);
    }
    
    {// trade with invalid qty (too big)
        oli::quantityT matchedQty4 = 20000;
        TradeRecord trade6(orderBid2.orderId_, orderBid2.price_, 
                orderAsk2.orderId_, orderAsk2.price_, matchedQty4);

        BOOST_CHECK_THROW(proc.onTrade(trade6), std::logic_error);
        BOOST_REQUIRE(0 == updObserver.updatesPublished());

    }

    
}


BOOST_AUTO_TEST_SUITE_END()