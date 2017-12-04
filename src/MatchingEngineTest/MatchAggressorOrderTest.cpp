//
// Created by sam1203 on 10/17/17.
//

#ifndef __PERF_TEST_ONLY___

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

#include "MatchAggressorOrder.h"


using namespace oli::mengine;

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

    template<typename T>
    T fillOrderBook(oli::idT ordId, oli::priceT startPrice, oli::priceT priceStep, int sign, oli::quantityT qty,
                                   size_t orderCountPerLevel, size_t priceLevels)
    {
        T obook;
        for(size_t pxLevel = 0; pxLevel < priceLevels; ++pxLevel)
        {
            OrderAtPriceLevelT orders;
            oli::priceT px = startPrice + sign*priceStep*pxLevel;
            for(size_t ordAtLevel = 0; ordAtLevel < orderCountPerLevel; ++ordAtLevel)
            {
                OrderData ord = {ordId++, qty, px};
                orders.push_back(ord);
            }
            obook[px] = orders;
        }

        return obook;
    }

}

BOOST_AUTO_TEST_SUITE( MatchAggrOrd )

    BOOST_AUTO_TEST_CASE( MatchAggrOrd_BidVanillaScenario )
    {
        //TestLogger logger;
        AggressorOrderData order = {123, oli::Side::buy_Side, 100, 11000};

        BidOrderBookT bidBook;
        AskOrderBookT askBook = fillOrderBook<AskOrderBookT>(2000, 11000, 500, 1, 2000, 5, 5);

        MatchAggressorOrder matcher;
        matcher.init(&order);

        BOOST_CHECK_NO_THROW(matcher.handle(bidBook, askBook));

        const MatchedOrdersT &matchedOrders = matcher.matchedOrders();
        BOOST_REQUIRE(1 == matchedOrders.size());
        BOOST_REQUIRE(2000 == matchedOrders.at(0).askId_);
        BOOST_REQUIRE(123 == matchedOrders.at(0).bidId_);
        BOOST_REQUIRE(11000 == matchedOrders.at(0).matchPx_);
        BOOST_REQUIRE(100 == matchedOrders.at(0).matchQty_);

        auto pxLevel = askBook.begin();
        BOOST_REQUIRE(11000 == pxLevel->first);
        BOOST_REQUIRE(5 == pxLevel->second.size());
        auto obOrder = pxLevel->second.begin();
        BOOST_REQUIRE(2000 == obOrder->id_);
        BOOST_REQUIRE(11000 == obOrder->price_);
        BOOST_REQUIRE(1900 == obOrder->leftQty_);
    }

    BOOST_AUTO_TEST_CASE( MatchAggrOrd_BidNoMatch )
    {
        //TestLogger logger;
        AggressorOrderData order = {123, oli::Side::buy_Side, 100, 10500};

        BidOrderBookT bidBook;
        AskOrderBookT askBook = fillOrderBook<AskOrderBookT>(2000, 11000, 500, 1, 2000, 5, 5);

        MatchAggressorOrder matcher;
        matcher.init(&order);

        BOOST_CHECK_NO_THROW(matcher.handle(bidBook, askBook));

        const MatchedOrdersT &matchedOrders = matcher.matchedOrders();
        BOOST_REQUIRE(0 == matchedOrders.size());
    }

    BOOST_AUTO_TEST_CASE( MatchAggrOrd_BidMultipleMatch )
    {
        //TestLogger logger;
        AggressorOrderData order = {123, oli::Side::buy_Side, 1500, 12000};

        BidOrderBookT bidBook;// = fillOrderBook<BidOrderBookT>(1000, 10000, 500, -1, 100, 5, 5);
        AskOrderBookT askBook = fillOrderBook<AskOrderBookT>(2000, 11000, 500, 1, 300, 3, 5);

        MatchAggressorOrder matcher;
        matcher.init(&order);

        BOOST_CHECK_NO_THROW(matcher.handle(bidBook, askBook));

        const MatchedOrdersT &matchedOrders = matcher.matchedOrders();
        BOOST_REQUIRE(5 == matchedOrders.size());
        auto matchData = matchedOrders.at(0);
        BOOST_CHECK(2000 == matchData.askId_);
        BOOST_CHECK(123 == matchData.bidId_);
        BOOST_CHECK(300 == matchData.matchQty_);
        BOOST_CHECK(11000 == matchData.matchPx_);

        matchData = matchedOrders.at(1);
        BOOST_CHECK(2001 == matchData.askId_);
        BOOST_CHECK(123 == matchData.bidId_);
        BOOST_CHECK(300 == matchData.matchQty_);
        BOOST_CHECK(11000 == matchData.matchPx_);

        matchData = matchedOrders.at(2);
        BOOST_CHECK(2002 == matchData.askId_);
        BOOST_CHECK(123 == matchData.bidId_);
        BOOST_CHECK(300 == matchData.matchQty_);
        BOOST_CHECK(11000 == matchData.matchPx_);

        matchData = matchedOrders.at(3);
        BOOST_CHECK(2003 == matchData.askId_);
        BOOST_CHECK(123 == matchData.bidId_);
        BOOST_CHECK(300 == matchData.matchQty_);
        BOOST_CHECK(11500 == matchData.matchPx_);

        matchData = matchedOrders.at(4);
        BOOST_CHECK(2004 == matchData.askId_);
        BOOST_CHECK(123 == matchData.bidId_);
        BOOST_CHECK(300 == matchData.matchQty_);
        BOOST_CHECK(11500 == matchData.matchPx_);

    }

    BOOST_AUTO_TEST_CASE( MatchAggrOrd_BidSequenceMatch )
    {
        BidOrderBookT bidBook;// = fillOrderBook<BidOrderBookT>(1000, 10000, 500, -1, 100, 5, 5);
        AskOrderBookT askBook = fillOrderBook<AskOrderBookT>(2000, 11000, 500, 1, 300, 3, 5);

        {
            AggressorOrderData order1 = {123, oli::Side::buy_Side, 1500, 12000};

            MatchAggressorOrder matcher;
            matcher.init(&order1);

            BOOST_CHECK_NO_THROW(matcher.handle(bidBook, askBook));

            const MatchedOrdersT &matchedOrders = matcher.matchedOrders();
            BOOST_REQUIRE(5 == matchedOrders.size());
            auto matchData = matchedOrders.at(0);
            BOOST_CHECK(2000 == matchData.askId_);
            BOOST_CHECK(123 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(11000 == matchData.matchPx_);

            matchData = matchedOrders.at(1);
            BOOST_CHECK(2001 == matchData.askId_);
            BOOST_CHECK(123 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(11000 == matchData.matchPx_);

            matchData = matchedOrders.at(2);
            BOOST_CHECK(2002 == matchData.askId_);
            BOOST_CHECK(123 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(11000 == matchData.matchPx_);

            matchData = matchedOrders.at(3);
            BOOST_CHECK(2003 == matchData.askId_);
            BOOST_CHECK(123 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(11500 == matchData.matchPx_);

            matchData = matchedOrders.at(4);
            BOOST_CHECK(2004 == matchData.askId_);
            BOOST_CHECK(123 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(11500 == matchData.matchPx_);
        }
        {
            AggressorOrderData order2 = {125, oli::Side::buy_Side, 1500, 12000};

            MatchAggressorOrder matcher;
            matcher.init(&order2);

            BOOST_CHECK_NO_THROW(matcher.handle(bidBook, askBook));

            const MatchedOrdersT &matchedOrders = matcher.matchedOrders();
            BOOST_REQUIRE(4 == matchedOrders.size());
            auto matchData = matchedOrders.at(0);
            BOOST_CHECK(2005 == matchData.askId_);
            BOOST_CHECK(125 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(11500 == matchData.matchPx_);

            matchData = matchedOrders.at(1);
            BOOST_CHECK(2006 == matchData.askId_);
            BOOST_CHECK(125 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(12000 == matchData.matchPx_);

            matchData = matchedOrders.at(2);
            BOOST_CHECK(2007 == matchData.askId_);
            BOOST_CHECK(125 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(12000 == matchData.matchPx_);

            matchData = matchedOrders.at(3);
            BOOST_CHECK(2008 == matchData.askId_);
            BOOST_CHECK(125 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(12000 == matchData.matchPx_);

        }

    }


    BOOST_AUTO_TEST_CASE( MatchAggrOrd_AskVanillaScenario )
    {
        //TestLogger logger;
        AggressorOrderData order = {123, oli::Side::sell_Side, 100, 9000};

        BidOrderBookT bidBook = fillOrderBook<BidOrderBookT>(1000, 10000, 500, -1, 2000, 5, 5);
        AskOrderBookT askBook;

        MatchAggressorOrder matcher;
        matcher.init(&order);

        BOOST_CHECK_NO_THROW(matcher.handle(bidBook, askBook));

        const MatchedOrdersT &matchedOrders = matcher.matchedOrders();
        BOOST_REQUIRE(1 == matchedOrders.size());
        BOOST_REQUIRE(123 == matchedOrders.at(0).askId_);
        BOOST_REQUIRE(1000 == matchedOrders.at(0).bidId_);
        BOOST_REQUIRE(10000 == matchedOrders.at(0).matchPx_);
        BOOST_REQUIRE(100 == matchedOrders.at(0).matchQty_);

        auto pxLevel = bidBook.begin();
        BOOST_REQUIRE(10000 == pxLevel->first);
        BOOST_REQUIRE(5 == pxLevel->second.size());
        auto obOrder = pxLevel->second.begin();
        BOOST_REQUIRE(1000 == obOrder->id_);
        BOOST_REQUIRE(10000 == obOrder->price_);
        BOOST_REQUIRE(1900 == obOrder->leftQty_);
    }

    BOOST_AUTO_TEST_CASE( MatchAggrOrd_AskNoMatch )
    {
        //TestLogger logger;
        AggressorOrderData order = {123, oli::Side::sell_Side, 100, 10500};

        BidOrderBookT bidBook = fillOrderBook<BidOrderBookT>(1000, 10000, 500, -1, 100, 5, 5);;
        AskOrderBookT askBook;

        MatchAggressorOrder matcher;
        matcher.init(&order);

        BOOST_CHECK_NO_THROW(matcher.handle(bidBook, askBook));

        const MatchedOrdersT &matchedOrders = matcher.matchedOrders();
        BOOST_REQUIRE(0 == matchedOrders.size());
    }

    BOOST_AUTO_TEST_CASE( MatchAggrOrd_AskMultipleMatch )
    {
        //TestLogger logger;
        AggressorOrderData order = {123, oli::Side::sell_Side, 1500, 9000};

        BidOrderBookT bidBook = fillOrderBook<BidOrderBookT>(1000, 10000, 500, -1, 300, 3, 5);
        AskOrderBookT askBook;// = fillOrderBook<AskOrderBookT>(2000, 11000, 500, 1, 300, 3, 5);

        MatchAggressorOrder matcher;
        matcher.init(&order);

        BOOST_CHECK_NO_THROW(matcher.handle(bidBook, askBook));

        const MatchedOrdersT &matchedOrders = matcher.matchedOrders();
        BOOST_REQUIRE(5 == matchedOrders.size());
        auto matchData = matchedOrders.at(0);
        BOOST_CHECK(123 == matchData.askId_);
        BOOST_CHECK(1000 == matchData.bidId_);
        BOOST_CHECK(300 == matchData.matchQty_);
        BOOST_CHECK(10000 == matchData.matchPx_);

        matchData = matchedOrders.at(1);
        BOOST_CHECK(123 == matchData.askId_);
        BOOST_CHECK(1001 == matchData.bidId_);
        BOOST_CHECK(300 == matchData.matchQty_);
        BOOST_CHECK(10000 == matchData.matchPx_);

        matchData = matchedOrders.at(2);
        BOOST_CHECK(123 == matchData.askId_);
        BOOST_CHECK(1002 == matchData.bidId_);
        BOOST_CHECK(300 == matchData.matchQty_);
        BOOST_CHECK(10000 == matchData.matchPx_);

        matchData = matchedOrders.at(3);
        BOOST_CHECK(123 == matchData.askId_);
        BOOST_CHECK(1003 == matchData.bidId_);
        BOOST_CHECK(300 == matchData.matchQty_);
        BOOST_CHECK(9500 == matchData.matchPx_);

        matchData = matchedOrders.at(4);
        BOOST_CHECK(123 == matchData.askId_);
        BOOST_CHECK(1004 == matchData.bidId_);
        BOOST_CHECK(300 == matchData.matchQty_);
        BOOST_CHECK(9500 == matchData.matchPx_);

    }

    BOOST_AUTO_TEST_CASE( MatchAggrOrd_AskSequenceMatch )
    {
        BidOrderBookT bidBook = fillOrderBook<BidOrderBookT>(1000, 10000, 500, -1, 300, 3, 5);
        AskOrderBookT askBook;

        {
            AggressorOrderData order1 = {123, oli::Side::sell_Side, 1500, 9000};

            MatchAggressorOrder matcher;
            matcher.init(&order1);

            BOOST_CHECK_NO_THROW(matcher.handle(bidBook, askBook));

            const MatchedOrdersT &matchedOrders = matcher.matchedOrders();
            BOOST_REQUIRE(5 == matchedOrders.size());
            auto matchData = matchedOrders.at(0);
            BOOST_CHECK(123 == matchData.askId_);
            BOOST_CHECK(1000 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(10000 == matchData.matchPx_);

            matchData = matchedOrders.at(1);
            BOOST_CHECK(123 == matchData.askId_);
            BOOST_CHECK(1001 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(10000 == matchData.matchPx_);

            matchData = matchedOrders.at(2);
            BOOST_CHECK(123 == matchData.askId_);
            BOOST_CHECK(1002 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(10000 == matchData.matchPx_);

            matchData = matchedOrders.at(3);
            BOOST_CHECK(123 == matchData.askId_);
            BOOST_CHECK(1003 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(9500 == matchData.matchPx_);

            matchData = matchedOrders.at(4);
            BOOST_CHECK(123 == matchData.askId_);
            BOOST_CHECK(1004 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(9500 == matchData.matchPx_);
        }
        {
            AggressorOrderData order2 = {125, oli::Side::sell_Side, 1500, 9000};

            MatchAggressorOrder matcher;
            matcher.init(&order2);

            BOOST_CHECK_NO_THROW(matcher.handle(bidBook, askBook));

            const MatchedOrdersT &matchedOrders = matcher.matchedOrders();
            BOOST_REQUIRE(4 == matchedOrders.size());
            auto matchData = matchedOrders.at(0);
            BOOST_CHECK(125 == matchData.askId_);
            BOOST_CHECK(1005 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(9500 == matchData.matchPx_);

            matchData = matchedOrders.at(1);
            BOOST_CHECK(125 == matchData.askId_);
            BOOST_CHECK(1006 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(9000 == matchData.matchPx_);

            matchData = matchedOrders.at(2);
            BOOST_CHECK(125 == matchData.askId_);
            BOOST_CHECK(1007 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(9000 == matchData.matchPx_);

            matchData = matchedOrders.at(3);
            BOOST_CHECK(125 == matchData.askId_);
            BOOST_CHECK(1008 == matchData.bidId_);
            BOOST_CHECK(300 == matchData.matchQty_);
            BOOST_CHECK(9000 == matchData.matchPx_);

        }

    }


BOOST_AUTO_TEST_SUITE_END()


#endif