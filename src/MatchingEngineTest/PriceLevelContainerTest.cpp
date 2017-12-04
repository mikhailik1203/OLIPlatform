//
// Created by sam1203 on 11/19/17.
//

#ifndef __PERF_TEST_ONLY___

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <boost/test/unit_test.hpp>
#pragma GCC diagnostic pop

#include "OLI_Logger.h"
#include "PriceLevelContainer.h"

#include <iostream>
#include <map>
#include <atomic>
#include <memory>

#include <sched.h>
#include <unistd.h>


using namespace oli::mengine;

namespace{

}


BOOST_AUTO_TEST_SUITE( PriceLevelContainerTest )

    BOOST_AUTO_TEST_CASE( PriceLevelContainerTest_AddVanilla )
    {
        PriceLevelContainer testCont;

        MDLevel2Record order2Add;
        order2Add.id_ = 1234;
        order2Add.price_ = 5000;
        order2Add.qty_ = 200;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;
        testCont.add(order2Add);

        PriceLevelIterator it = testCont.begin();
        BOOST_REQUIRE(true == it.isValid());

        OrderData &data = it.data();
        BOOST_REQUIRE(order2Add.id_ == data.id_);
        BOOST_REQUIRE(order2Add.qty_ == data.leftQty_);
        BOOST_REQUIRE(order2Add.price_ == data.price_);

        it.next();
        BOOST_REQUIRE(!it.isValid());
    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainerTest_AddMultiple )
    {
        const size_t ADD_ORDERS_COUNT = 100;
        oli::idT startOrderId = 1234;
        oli::quantityT startOrderQty = 200;

        MDLevel2Record order2Add;
        order2Add.id_ = startOrderId;
        order2Add.price_ = 5000;
        order2Add.qty_ = startOrderQty;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;

        PriceLevelContainer testCont;
        for(size_t i = 0; i < ADD_ORDERS_COUNT; ++i){
            ++order2Add.id_;
            order2Add.qty_ += 10;
            testCont.add(order2Add);

            PriceLevelIterator bit = testCont.begin();
            OrderData &sdata = bit.data();

        }

        PriceLevelIterator it = testCont.begin();

        oli::idT currOrderId = startOrderId;
        oli::quantityT currOrderQty = startOrderQty;
        size_t i = 0;
        while(it.isValid()){
            BOOST_REQUIRE(it.isValid());

            OrderData &data = it.data();

            ++currOrderId;
            currOrderQty += 10;

            BOOST_REQUIRE(currOrderId == data.id_);
            BOOST_REQUIRE(currOrderQty == data.leftQty_);
            BOOST_REQUIRE(order2Add.price_ == data.price_);

            it.next();
            ++i;
        }
        BOOST_REQUIRE(ADD_ORDERS_COUNT == i);
        BOOST_REQUIRE(!it.isValid());
    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainerTest_RemoveVanilla )
    {
        PriceLevelContainer testCont;

        MDLevel2Record order2Add;
        order2Add.id_ = 1234;
        order2Add.price_ = 5000;
        order2Add.qty_ = 200;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;
        testCont.add(order2Add);

        {
            PriceLevelIterator it = testCont.begin();
            BOOST_REQUIRE(true == it.isValid());

            OrderData &data = it.data();
            BOOST_REQUIRE(order2Add.id_ == data.id_);
            BOOST_REQUIRE(order2Add.qty_ == data.leftQty_);
            BOOST_REQUIRE(order2Add.price_ == data.price_);

            it.next();
            BOOST_REQUIRE(!it.isValid());
        }

        MDLevel2Record order2Remove;
        order2Remove.id_ = order2Add.id_;
        order2Remove.price_ = 5000;
        order2Remove.qty_ = 200;
        order2Remove.side_ = oli::Side::buy_Side;
        order2Remove.type_ = MDUpdateType::remove_MDUpdateType;
        testCont.remove(order2Remove);

        {
            PriceLevelIterator it = testCont.begin();
            BOOST_REQUIRE(!it.isValid());
        }

    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainerTest_RemoveMultiple )
    {
        const size_t REMOVE_ORDERS_COUNT = 100;
        oli::idT startOrderId = 1234;
        oli::quantityT startOrderQty = 200;

        PriceLevelContainer testCont;

        {
            MDLevel2Record order2Add;
            order2Add.id_ = startOrderId;
            order2Add.price_ = 5000;
            order2Add.qty_ = startOrderQty;
            order2Add.side_ = oli::Side::buy_Side;
            order2Add.type_ = MDUpdateType::add_MDUpdateType;

            for(size_t i = 0; i < REMOVE_ORDERS_COUNT; ++i){
                ++order2Add.id_;
                order2Add.qty_ += 10;
                testCont.add(order2Add);

                PriceLevelIterator bit = testCont.begin();
                OrderData &sdata = bit.data();

            }
        }

        {
            oli::idT currOrderId = startOrderId;
            oli::quantityT currOrderQty = startOrderQty;

            MDLevel2Record order2Remove;
            order2Remove.id_ = startOrderId;
            order2Remove.price_ = 5000;
            order2Remove.qty_ = startOrderQty;
            order2Remove.side_ = oli::Side::buy_Side;
            order2Remove.type_ = MDUpdateType::remove_MDUpdateType;

            for(size_t i = 0; i < REMOVE_ORDERS_COUNT - 1; ++i){
                ++order2Remove.id_;
                order2Remove.qty_ += 10;
                testCont.remove(order2Remove);

                PriceLevelIterator nextIt = testCont.begin();
                BOOST_REQUIRE(nextIt.isValid());
                OrderData &sdata = nextIt.data();

                BOOST_REQUIRE(order2Remove.id_ + 1 == sdata.id_);
                BOOST_REQUIRE(order2Remove.qty_ + 10 == sdata.leftQty_);
                BOOST_REQUIRE(order2Remove.price_ == sdata.price_);
            }

            ++order2Remove.id_;
            order2Remove.qty_ += 10;
            testCont.remove(order2Remove);

            PriceLevelIterator nextIt = testCont.begin();
            BOOST_REQUIRE(!nextIt.isValid());
        }
    }

    BOOST_AUTO_TEST_CASE( PriceLevelContainer2CmpTest_AddVanilla )
    {
        PriceLevelContainer2Cmp testCont;

        MDLevel2Record order2Add;
        order2Add.id_ = 1234;
        order2Add.price_ = 5000;
        order2Add.qty_ = 200;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;
        testCont.add(order2Add);

        PriceLevelContainer2Cmp::PriceLevelIterator2Cmp it = testCont.begin();
        BOOST_REQUIRE(true == it.isValid());

        OrderData &data = it.data();
        BOOST_REQUIRE(order2Add.id_ == data.id_);
        BOOST_REQUIRE(order2Add.qty_ == data.leftQty_);
        BOOST_REQUIRE(order2Add.price_ == data.price_);

        it.next();
        BOOST_REQUIRE(!it.isValid());
    }


    BOOST_AUTO_TEST_CASE( PriceLevelContainer2CmpTest_RemoveVanilla )
    {
        PriceLevelContainer2Cmp testCont;

        MDLevel2Record order2Add;
        order2Add.id_ = 1234;
        order2Add.price_ = 5000;
        order2Add.qty_ = 200;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;
        testCont.add(order2Add);

        {
            PriceLevelContainer2Cmp::PriceLevelIterator2Cmp it = testCont.begin();
            BOOST_REQUIRE(true == it.isValid());

            OrderData &data = it.data();
            BOOST_REQUIRE(order2Add.id_ == data.id_);
            BOOST_REQUIRE(order2Add.qty_ == data.leftQty_);
            BOOST_REQUIRE(order2Add.price_ == data.price_);

            it.next();
            BOOST_REQUIRE(!it.isValid());
        }

        MDLevel2Record order2Remove;
        order2Remove.id_ = order2Add.id_;
        order2Remove.price_ = 5000;
        order2Remove.qty_ = 200;
        order2Remove.side_ = oli::Side::buy_Side;
        order2Remove.type_ = MDUpdateType::remove_MDUpdateType;
        testCont.remove(order2Remove);

        {
            PriceLevelContainer2Cmp::PriceLevelIterator2Cmp it = testCont.begin();
            BOOST_REQUIRE(!it.isValid());
        }

    }


    BOOST_AUTO_TEST_CASE( PriceLevelContainer2CmpTest_AddMultiple )
    {
        const size_t ADD_ORDERS_COUNT = 100;
        oli::idT startOrderId = 1234;
        oli::quantityT startOrderQty = 200;

        MDLevel2Record order2Add;
        order2Add.id_ = startOrderId;
        order2Add.price_ = 5000;
        order2Add.qty_ = startOrderQty;
        order2Add.side_ = oli::Side::buy_Side;
        order2Add.type_ = MDUpdateType::add_MDUpdateType;

        PriceLevelContainer2Cmp testCont;
        for(size_t i = 0; i < ADD_ORDERS_COUNT; ++i){
            ++order2Add.id_;
            order2Add.qty_ += 10;
            testCont.add(order2Add);

            PriceLevelContainer2Cmp::PriceLevelIterator2Cmp bit = testCont.begin();
            OrderData &sdata = bit.data();

        }

        PriceLevelContainer2Cmp::PriceLevelIterator2Cmp it = testCont.begin();

        oli::idT currOrderId = startOrderId;
        oli::quantityT currOrderQty = startOrderQty;
        size_t i = 0;
        while(it.isValid()){
            BOOST_REQUIRE(it.isValid());

            OrderData &data = it.data();

            ++currOrderId;
            currOrderQty += 10;

            BOOST_REQUIRE(currOrderId == data.id_);
            BOOST_REQUIRE(currOrderQty == data.leftQty_);
            BOOST_REQUIRE(order2Add.price_ == data.price_);

            it.next();
            ++i;
        }
        BOOST_REQUIRE(ADD_ORDERS_COUNT == i);
        BOOST_REQUIRE(!it.isValid());
    }



BOOST_AUTO_TEST_SUITE_END()

#endif