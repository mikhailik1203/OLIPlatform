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


using namespace oli::obook;

namespace{

    class TestLogger final: public oli::Logger
    {
    public:
        TestLogger(){}
        virtual ~TestLogger(){}

        virtual void trace(const std::string &msg){
            //BOOST_TEST_MESSAGE("[TRACE] "<< msg);
        }
        virtual void debug(const std::string &msg){
            //BOOST_TEST_MESSAGE("[DEBUG] "<< msg);
        }
        virtual void note(const std::string &msg){
            //BOOST_TEST_MESSAGE("[NOTE] "<< msg);
        }
        virtual void warn(const std::string &msg){
            //BOOST_TEST_MESSAGE("[WARN] "<< msg);
        }
        virtual void error(const std::string &msg){
            //BOOST_TEST_MESSAGE("[ERROR] "<< msg);
        }
        virtual void fatal(const std::string &msg){
            //BOOST_TEST_MESSAGE("[FATAL] "<< msg);
        }
    };


}

BOOST_AUTO_TEST_SUITE( MatchingEngine )

    BOOST_AUTO_TEST_CASE( MatchingEngine_VanillaScenario )
    {
        //TestLogger logger;
    }



BOOST_AUTO_TEST_SUITE_END()

#endif