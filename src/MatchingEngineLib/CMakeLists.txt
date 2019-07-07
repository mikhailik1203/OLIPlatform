cmake_minimum_required(VERSION 3.8)
project(MatchingEngineLib)

set(CMAKE_CXX_STANDARD 17)

include_directories(. ../include
        ../SecMasterCacheLib
        ../MessagesLib
        ../MessagesLib/src
        ../MessagesLib/src/protobuf
        $ENV{BOOST_INCLUDE})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../lib/${CMAKE_BUILD_TYPE})

set (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -mtune=native") #-pg
message (${CMAKE_CXX_FLAGS_RELEASE})

set(SOURCE_FILES MatchingEngine.cpp MatchingEngine.h MatchingEngineDef.h MEOrderBookCache.cpp
        MEOrderBookCache.h MatchingEngineDispatcher.cpp MatchingEngineDispatcher.h
        METransportAdaptor.cpp METransportAdaptor.h MEFactory.cpp MEFactory.h MatchMarketOrder.h
        MatchAggressorOrder.cpp MatchAggressorOrder.h MatchMarketOrder.cpp PriceLevelContainer.cpp PriceLevelContainer.h)
add_library(MatchingEngineLib ${SOURCE_FILES})