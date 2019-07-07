#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <boost/test/unit_test.hpp> 
#pragma GCC diagnostic pop

#include "OLI_Logger.h"
#include "OrderBookDispatcher.h"
#include "SecMasterDef.h"

#include <iostream>
#include <map>
#include <atomic>
#include <memory>


using namespace oli::obook;

namespace{
    
    class TestOBProcessor final: public IOBookEventProcessor
    {
    public:
        TestOBProcessor(){}
        virtual ~TestOBProcessor(){}
        
    public:
        virtual void onNewOrder(const OrderBookRecord &order) override
        {
            events_.push_back(EventParams(onNewOrder_eventType, order.orderId_));
        }
        
        virtual void onChangeOrder(const OrderBookRecord &oldOrder, const OrderBookRecord &newOrder) override
        {
            events_.push_back(EventParams(onChangeOrder_eventType, newOrder.orderId_));
        }
        
        virtual void onDeleteOrder(const OrderBookRecord &order) override
        {
            events_.push_back(EventParams(onDeleteOrder_eventType, order.orderId_));
        }
        
        virtual void onDeleteOrders(const std::deque<OrderBookRecord> &orders) override
        {
            events_.push_back(EventParams(onDeleteOrders_eventType, orders.size()));
        }
        
        virtual void stopProcessing() override
        {
            events_.push_back(EventParams(stopProcessing_eventType, oli::INVALID_ID));
        }
        virtual void startProcessing(const oli::secmaster::InstrumentRecord &instr)override 
        {
            events_.push_back(EventParams(startProcessing_eventType, instr.instrId_));
        }
        
        virtual void onTrade(const TradeRecord &trade) override
        {
            events_.push_back(EventParams(onTrade_eventType, trade.askOrderId_));
            events_.push_back(EventParams(onTrade_eventType, trade.bidOrderId_));
        }
        virtual void getOBSnapshot(size_t maxLevels, size_t maxRecordsPerLevel, 
                    PriceLevelsT &bidPriceLevels, PriceLevelsT &askPriceLevels)const override
        {
            events_.push_back(EventParams(getOBSnapshot_eventType, askPriceLevels.size()));
            events_.push_back(EventParams(getOBSnapshot_eventType, bidPriceLevels.size()));
        }
    public:
        enum EventType{
            invalid_eventType = 0,
            onNewOrder_eventType, 
            onChangeOrder_eventType, 
            onDeleteOrder_eventType, 
            onDeleteOrders_eventType, 
            stopProcessing_eventType, 
            startProcessing_eventType, 
            onTrade_eventType, 
            getOBSnapshot_eventType
        };
        
        struct EventParams{
            EventType type_;
            oli::idT id_;
            
            EventParams(): type_(invalid_eventType), id_(oli::INVALID_ID){}
            EventParams(EventType type, oli::idT id): type_(type), id_(id){}
        };
        
        size_t eventCount()const{return events_.size();}
        
        void clearEvents(){events_.clear();}
        
        EventParams popFront(){
            if(events_.empty())
                throw std::logic_error("TestOBProcessor::popFront: no events!");
            EventParams evnt = events_.front();
            events_.pop_front();
            return evnt;
        }
    private:
        size_t processingStarted_;
        
        typedef std::deque<EventParams> EventsT;
        mutable EventsT events_;
    };
    
    class TestOBProcessorFactory final : public IOBookProcessorFactory
    {
    public:    
        TestOBProcessorFactory(){}
        virtual ~TestOBProcessorFactory(){}
        
    public:
        virtual IOBookEventProcessor *create()const override
        {
            return new TestOBProcessor;
        }
    };
    
    class TestInstrumentCache final : public oli::secmaster::IInstrumentCache
    {
    public:
        TestInstrumentCache(): obs_(nullptr){}
        virtual ~TestInstrumentCache(){}
        
        virtual oli::secmaster::InstrumentRecord findInstrument(const oli::idT &instrId)const override
        {
            InstrumentsT::const_iterator it = instruments_.find(instrId);
            if(instruments_.end() == it)
                throw std::logic_error("TestInstrumentCache::findInstrument(): unable to find instrument!");
            return it->second;
        }
        
        virtual void getAvailableInstruments(oli::secmaster::InstrumentIdsT &instr)const override
        {
            for(auto instrIt: instruments_){
                instr.insert(instrIt.second.instrId_);
            }
        }
        
        virtual void attach(oli::secmaster::IInstrumentEventObserver *obs)override
        {
            obs_ = obs;
        }
        
        virtual void dettach(oli::secmaster::IInstrumentEventObserver *obs)override
        {
            obs_ = nullptr;
        }
        
    public:
        void addInstrument(const oli::secmaster::InstrumentRecord &instr)
        {
            instruments_[instr.instrId_] = instr;
        }
        
        bool instrumentObserverAttached()const{
            return nullptr != obs_;
        }
    private:
        oli::secmaster::IInstrumentEventObserver *obs_;
        
        typedef std::map<oli::idT, oli::secmaster::InstrumentRecord> InstrumentsT;
        InstrumentsT instruments_;
    };
    
    class TestOrderBookDispatcher final : public OrderBookDispatcher{
    public:    
        TestOrderBookDispatcher(IOBookProcessorFactory *fact, 
                            oli::secmaster::IInstrumentCache *instrCache): 
            OrderBookDispatcher(fact, instrCache)
        {}
    
        virtual ~TestOrderBookDispatcher(){}
    public:
        using OrderBookDispatcher::findProcessor;
    };
    

}

BOOST_AUTO_TEST_SUITE( OB_Dispatcher )

BOOST_AUTO_TEST_CASE( onAddInstrument_VanillaScenario )
{
    oli::idT entityId = 5;
    oli::secmaster::InstrumentRecord instr1(++entityId, "TestInstrument1", 
                150000, 250000, 5000);
    oli::secmaster::InstrumentRecord instr2(++entityId, "TestInstrument2", 
                120000, 220000, 5000);
    oli::secmaster::InstrumentRecord instr3(++entityId, "TestInstrument3", 
                120000, 220000, 1000);
    oli::secmaster::InstrumentRecord instr4(++entityId, "TestInstrument3", 
                17000, 30000, 1000);
    
    TestOBProcessorFactory testFact; 
    TestInstrumentCache testInstrCache;
    testInstrCache.addInstrument(instr1);
    testInstrCache.addInstrument(instr2);
    testInstrCache.addInstrument(instr3);
    testInstrCache.addInstrument(instr4);
    
    TestOrderBookDispatcher dispatcher2Test(&testFact, &testInstrCache);
    
    dispatcher2Test.onAddInstrument(instr4);
    IOBookEventProcessor *eventProc = dispatcher2Test.findProcessor(instr4.instrId_);
    TestOBProcessor *testProc4 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc4, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");

    BOOST_REQUIRE_MESSAGE(1 == testProc4->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    TestOBProcessor::EventParams ent = testProc4->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::startProcessing_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't started!");
    BOOST_REQUIRE_MESSAGE(instr4.instrId_ == ent.id_, 
            "onAddInstrument_VanillaScenario: test processor started with invalid instrument!");

    dispatcher2Test.onAddInstrument(instr3);
    eventProc = dispatcher2Test.findProcessor(instr3.instrId_);
    TestOBProcessor *testProc3 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc3, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    BOOST_REQUIRE_MESSAGE(testProc4 != testProc3, 
            "onAddInstrument_VanillaScenario: new test processor has to be used!");
            
    BOOST_REQUIRE_MESSAGE(1 == testProc3->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    ent = testProc3->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::startProcessing_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't started!");
    BOOST_REQUIRE_MESSAGE(instr3.instrId_ == ent.id_, 
            "onAddInstrument_VanillaScenario: test processor started with invalid instrument!");

    dispatcher2Test.onAddInstrument(instr2);
    eventProc = dispatcher2Test.findProcessor(instr2.instrId_);
    TestOBProcessor *testProc2 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc2, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    BOOST_REQUIRE_MESSAGE(testProc4 != testProc2, 
            "onAddInstrument_VanillaScenario: new test processor has to be used!");
    BOOST_REQUIRE_MESSAGE(testProc3 != testProc2, 
            "onAddInstrument_VanillaScenario: new test processor has to be used!");

    BOOST_REQUIRE_MESSAGE(1 == testProc2->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    ent = testProc2->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::startProcessing_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't started!");
    BOOST_REQUIRE_MESSAGE(instr2.instrId_ == ent.id_, 
            "onAddInstrument_VanillaScenario: test processor started with invalid instrument!");

    
}

BOOST_AUTO_TEST_CASE( onAddInstrument_Duplicate )
{
    oli::idT entityId = 5;
    oli::secmaster::InstrumentRecord instr1(++entityId, "TestInstrument1", 
                150000, 250000, 5000);
    oli::secmaster::InstrumentRecord instr2(++entityId, "TestInstrument2", 
                120000, 220000, 5000);
    oli::secmaster::InstrumentRecord instr3(++entityId, "TestInstrument3", 
                120000, 220000, 1000);
    oli::secmaster::InstrumentRecord instr4(++entityId, "TestInstrument3", 
                17000, 30000, 1000);
    
    TestOBProcessorFactory testFact; 
    TestInstrumentCache testInstrCache;
    testInstrCache.addInstrument(instr1);
    testInstrCache.addInstrument(instr2);
    testInstrCache.addInstrument(instr3);
    testInstrCache.addInstrument(instr4);
    
    TestOrderBookDispatcher dispatcher2Test(&testFact, &testInstrCache);
    
    dispatcher2Test.onAddInstrument(instr4);
    IOBookEventProcessor *eventProc = dispatcher2Test.findProcessor(instr4.instrId_);
    TestOBProcessor *testProc4 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc4, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");

    BOOST_REQUIRE_MESSAGE(1 == testProc4->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    TestOBProcessor::EventParams ent = testProc4->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::startProcessing_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't started!");

    /// add same instrument again - same processor needs to be used and for startProcessing() call
    dispatcher2Test.onAddInstrument(instr4);
    eventProc = dispatcher2Test.findProcessor(instr4.instrId_);
    TestOBProcessor *testProc3 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc3, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    BOOST_REQUIRE_MESSAGE(testProc4 == testProc3, 
            "onAddInstrument_VanillaScenario: new test processor has to be used!");
    BOOST_REQUIRE_MESSAGE(1 == testProc4->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    ent = testProc4->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::startProcessing_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't started!");
            
    
}

BOOST_AUTO_TEST_CASE( onRemoveInstrument_VanillaScenario )
{
    oli::idT entityId = 5;
    oli::secmaster::InstrumentRecord instr1(++entityId, "TestInstrument1", 
                150000, 250000, 5000);
    oli::secmaster::InstrumentRecord instr2(++entityId, "TestInstrument2", 
                120000, 220000, 5000);
    oli::secmaster::InstrumentRecord instr3(++entityId, "TestInstrument3", 
                120000, 220000, 1000);
    oli::secmaster::InstrumentRecord instr4(++entityId, "TestInstrument3", 
                17000, 30000, 1000);
    
    TestOBProcessorFactory testFact; 
    TestInstrumentCache testInstrCache;
    testInstrCache.addInstrument(instr1);
    testInstrCache.addInstrument(instr2);
    testInstrCache.addInstrument(instr3);
    testInstrCache.addInstrument(instr4);
    
    TestOrderBookDispatcher dispatcher2Test(&testFact, &testInstrCache);
    
    dispatcher2Test.onAddInstrument(instr1);
    IOBookEventProcessor *eventProc = dispatcher2Test.findProcessor(instr1.instrId_);
    TestOBProcessor *testProc1 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc1, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc1->clearEvents();

    dispatcher2Test.onAddInstrument(instr2);
    eventProc = dispatcher2Test.findProcessor(instr2.instrId_);
    TestOBProcessor *testProc2 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc2, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc2->clearEvents();
    
    dispatcher2Test.onAddInstrument(instr3);
    eventProc = dispatcher2Test.findProcessor(instr3.instrId_);
    TestOBProcessor *testProc3 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc3, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc3->clearEvents();
    
    dispatcher2Test.onAddInstrument(instr4);
    eventProc = dispatcher2Test.findProcessor(instr4.instrId_);
    TestOBProcessor *testProc4 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc4, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc4->clearEvents();


    dispatcher2Test.onRemoveInstrument(instr4);
    BOOST_REQUIRE_MESSAGE(1 == testProc4->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    TestOBProcessor::EventParams ent = testProc4->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::stopProcessing_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't stopped!");
    BOOST_REQUIRE_MESSAGE(oli::INVALID_ID == ent.id_, 
            "onAddInstrument_VanillaScenario: test processor stopped with invalid instrument!");

    dispatcher2Test.onRemoveInstrument(instr2);
    BOOST_REQUIRE_MESSAGE(1 == testProc2->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    ent = testProc2->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::stopProcessing_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't stopped!");
    BOOST_REQUIRE_MESSAGE(oli::INVALID_ID == ent.id_, 
            "onAddInstrument_VanillaScenario: test processor stopped with invalid instrument!");

    dispatcher2Test.onRemoveInstrument(instr1);
    BOOST_REQUIRE_MESSAGE(1 == testProc1->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    ent = testProc1->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::stopProcessing_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't stopped!");
    BOOST_REQUIRE_MESSAGE(oli::INVALID_ID == ent.id_, 
            "onAddInstrument_VanillaScenario: test processor stopped with invalid instrument!");

    dispatcher2Test.onRemoveInstrument(instr3);
    BOOST_REQUIRE_MESSAGE(1 == testProc3->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    ent = testProc3->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::stopProcessing_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't stopped!");
    BOOST_REQUIRE_MESSAGE(oli::INVALID_ID == ent.id_, 
            "onAddInstrument_VanillaScenario: test processor stopped with invalid instrument!");
    
}


BOOST_AUTO_TEST_CASE( onRemoveInstrument_Duplicate )
{
    oli::idT entityId = 5;
    oli::secmaster::InstrumentRecord instr1(++entityId, "TestInstrument1", 
                150000, 250000, 5000);
    oli::secmaster::InstrumentRecord instr2(++entityId, "TestInstrument2", 
                120000, 220000, 5000);
    oli::secmaster::InstrumentRecord instr3(++entityId, "TestInstrument3", 
                120000, 220000, 1000);
    oli::secmaster::InstrumentRecord instr4(++entityId, "TestInstrument3", 
                17000, 30000, 1000);
    
    TestOBProcessorFactory testFact; 
    TestInstrumentCache testInstrCache;
    testInstrCache.addInstrument(instr1);
    testInstrCache.addInstrument(instr2);
    testInstrCache.addInstrument(instr3);
    testInstrCache.addInstrument(instr4);
    
    TestOrderBookDispatcher dispatcher2Test(&testFact, &testInstrCache);
    
    dispatcher2Test.onAddInstrument(instr1);
    IOBookEventProcessor *eventProc = dispatcher2Test.findProcessor(instr1.instrId_);
    TestOBProcessor *testProc1 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc1, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc1->clearEvents();

    dispatcher2Test.onAddInstrument(instr2);
    eventProc = dispatcher2Test.findProcessor(instr2.instrId_);
    TestOBProcessor *testProc2 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc2, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc2->clearEvents();
    
    dispatcher2Test.onAddInstrument(instr3);
    eventProc = dispatcher2Test.findProcessor(instr3.instrId_);
    TestOBProcessor *testProc3 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc3, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc3->clearEvents();
    
    dispatcher2Test.onAddInstrument(instr4);
    eventProc = dispatcher2Test.findProcessor(instr4.instrId_);
    TestOBProcessor *testProc4 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc4, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc4->clearEvents();

    dispatcher2Test.onRemoveInstrument(instr2);
    BOOST_REQUIRE_MESSAGE(1 == testProc2->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    TestOBProcessor::EventParams ent = testProc2->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::stopProcessing_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't stopped!");
    BOOST_REQUIRE_MESSAGE(oli::INVALID_ID == ent.id_, 
            "onAddInstrument_VanillaScenario: test processor stopped with invalid instrument!");

    /// remove same instrument again
    dispatcher2Test.onRemoveInstrument(instr2);
    BOOST_REQUIRE_MESSAGE(1 == testProc2->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    ent = testProc2->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::stopProcessing_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't stopped!");
    BOOST_REQUIRE_MESSAGE(oli::INVALID_ID == ent.id_, 
            "onAddInstrument_VanillaScenario: test processor stopped with invalid instrument!");


}

BOOST_AUTO_TEST_CASE( onRemoveInstrument_NotExist )
{
    oli::idT entityId = 5;
    oli::secmaster::InstrumentRecord instr1(++entityId, "TestInstrument1", 
                150000, 250000, 5000);
    oli::secmaster::InstrumentRecord instr2(++entityId, "TestInstrument2", 
                120000, 220000, 5000);
    oli::secmaster::InstrumentRecord instr3(++entityId, "TestInstrument3", 
                120000, 220000, 1000);
    oli::secmaster::InstrumentRecord instr4(++entityId, "TestInstrument3", 
                17000, 30000, 1000);
    
    TestOBProcessorFactory testFact; 
    TestInstrumentCache testInstrCache;
    testInstrCache.addInstrument(instr1);
    testInstrCache.addInstrument(instr2);
    testInstrCache.addInstrument(instr3);
    testInstrCache.addInstrument(instr4);
    
    TestOrderBookDispatcher dispatcher2Test(&testFact, &testInstrCache);
    
    dispatcher2Test.onAddInstrument(instr1);
    IOBookEventProcessor *eventProc = dispatcher2Test.findProcessor(instr1.instrId_);
    TestOBProcessor *testProc1 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc1, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc1->clearEvents();

    dispatcher2Test.onAddInstrument(instr3);
    eventProc = dispatcher2Test.findProcessor(instr3.instrId_);
    TestOBProcessor *testProc3 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc3, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc3->clearEvents();
    
    dispatcher2Test.onAddInstrument(instr4);
    eventProc = dispatcher2Test.findProcessor(instr4.instrId_);
    TestOBProcessor *testProc4 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc4, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc4->clearEvents();

    /// No onAddInstrument call for instr2
    /// event should be ignored - no processing required
    dispatcher2Test.onRemoveInstrument(instr2);

}

BOOST_AUTO_TEST_CASE( onNewOrder_VanillaScenario )
{
    oli::idT entityId = 5;
    oli::secmaster::InstrumentRecord instr1(++entityId, "TestInstrument1", 
                150000, 250000, 5000);
    oli::secmaster::InstrumentRecord instr2(++entityId, "TestInstrument2", 
                120000, 220000, 5000);
    oli::secmaster::InstrumentRecord instr3(++entityId, "TestInstrument3", 
                120000, 220000, 1000);
    
    TestOBProcessorFactory testFact; 
    TestInstrumentCache testInstrCache;
    testInstrCache.addInstrument(instr1);
    testInstrCache.addInstrument(instr2);
    testInstrCache.addInstrument(instr3);
    
    TestOrderBookDispatcher dispatcher2Test(&testFact, &testInstrCache);
    
    dispatcher2Test.onAddInstrument(instr1);
    IOBookEventProcessor *eventProc = dispatcher2Test.findProcessor(instr1.instrId_);
    TestOBProcessor *testProc1 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc1, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc1->clearEvents();

    dispatcher2Test.onAddInstrument(instr3);
    eventProc = dispatcher2Test.findProcessor(instr3.instrId_);
    TestOBProcessor *testProc3 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc3, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc3->clearEvents();
            
    dispatcher2Test.onAddInstrument(instr2);
    eventProc = dispatcher2Test.findProcessor(instr2.instrId_);
    TestOBProcessor *testProc2 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc2, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc2->clearEvents();

    OrderBookRecord ord(++entityId, instr2.instrId_, 0, 0, oli::Side::buy_Side);
    dispatcher2Test.onNewOrder(ord);
    BOOST_REQUIRE_MESSAGE(1 == testProc2->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    TestOBProcessor::EventParams ent = testProc2->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::onNewOrder_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't receive NewOrder!");
    BOOST_REQUIRE_MESSAGE(ord.orderId_ == ent.id_, 
            "onAddInstrument_VanillaScenario: test processor received NewOrder with invalid id!");

    ord.instrumentId_ = instr1.instrId_;
    dispatcher2Test.onNewOrder(ord);
    BOOST_REQUIRE_MESSAGE(0 == testProc2->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor has events!");
    BOOST_REQUIRE_MESSAGE(1 == testProc1->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor hasn't events!");
    ent = testProc1->popFront();
    BOOST_REQUIRE_MESSAGE(TestOBProcessor::onNewOrder_eventType == ent.type_, 
            "onAddInstrument_VanillaScenario: test processor didn't receive NewOrder!");
    BOOST_REQUIRE_MESSAGE(ord.orderId_ == ent.id_, 
            "onAddInstrument_VanillaScenario: test processor received NewOrder with invalid id!");
    
    
}

BOOST_AUTO_TEST_CASE( onNewOrder_UnknownInstrument )
{
    oli::idT entityId = 5;
    oli::secmaster::InstrumentRecord instr1(++entityId, "TestInstrument1", 
                150000, 250000, 5000);
    oli::secmaster::InstrumentRecord instr2(++entityId, "TestInstrument2", 
                120000, 220000, 5000);
    oli::secmaster::InstrumentRecord instr3(++entityId, "TestInstrument3", 
                120000, 220000, 1000);
    oli::secmaster::InstrumentRecord instr4(++entityId, "TestInstrument3", 
                120000, 220000, 1000);
    
    TestOBProcessorFactory testFact; 
    TestInstrumentCache testInstrCache;
    testInstrCache.addInstrument(instr1);
    testInstrCache.addInstrument(instr2);
    testInstrCache.addInstrument(instr3);
    testInstrCache.addInstrument(instr4);
    
    TestOrderBookDispatcher dispatcher2Test(&testFact, &testInstrCache);
    
    dispatcher2Test.onAddInstrument(instr1);
    IOBookEventProcessor *eventProc = dispatcher2Test.findProcessor(instr1.instrId_);
    TestOBProcessor *testProc1 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc1, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc1->clearEvents();

    dispatcher2Test.onAddInstrument(instr3);
    eventProc = dispatcher2Test.findProcessor(instr3.instrId_);
    TestOBProcessor *testProc3 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc3, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc3->clearEvents();
            
    dispatcher2Test.onAddInstrument(instr2);
    eventProc = dispatcher2Test.findProcessor(instr2.instrId_);
    TestOBProcessor *testProc2 = static_cast<TestOBProcessor *>(eventProc);
    BOOST_REQUIRE_MESSAGE(nullptr != testProc2, 
            "onAddInstrument_VanillaScenario: test processor is not assinged!");
    testProc2->clearEvents();

    OrderBookRecord ord(++entityId, instr4.instrId_, 0, 0, oli::Side::buy_Side);
    BOOST_CHECK_THROW(dispatcher2Test.onNewOrder(ord), std::logic_error);
    BOOST_REQUIRE_MESSAGE(0 == testProc1->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor has events!");
    BOOST_REQUIRE_MESSAGE(0 == testProc2->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor has events!");
    BOOST_REQUIRE_MESSAGE(0 == testProc3->eventCount(), 
            "onAddInstrument_VanillaScenario: test processor has events!");
    
    
}


BOOST_AUTO_TEST_SUITE_END()