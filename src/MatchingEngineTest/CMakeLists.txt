cmake_minimum_required(VERSION 3.8)
project(MatchingEngineTest)

set(CMAKE_CXX_STANDARD 17)

include(FindProtobuf)

find_package(Protobuf REQUIRED)

include_directories(
        ${PROJECT_SOURCE_DIR}/../MatchingEngineLib/
        ${PROJECT_SOURCE_DIR}/../OrderBookLib/
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/../include
        ${PROJECT_SOURCE_DIR}/../MessagesLib
        ${PROJECT_SOURCE_DIR}/../OMSServiceLib/.
        ${PROJECT_SOURCE_DIR}/../UtilsLib/src
        ${PROJECT_SOURCE_DIR}/../SecMasterCacheLib/
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/../MessagesLib/src/protobuf
        $ENV{BOOST_INCLUDE}
        ${PROTOBUF_INCLUDE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin/${CMAKE_BUILD_TYPE})
#message (${CMAKE_BUILD_TYPE})
#message (${CMAKE_BINARY_DIR})
#message (${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
#message (${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -mtune=native ") #-pg -D__PERF_TEST_ONLY___
#message (${CMAKE_CXX_FLAGS_RELEASE})


set(SOURCE_FILES MatchingEngineLibTest.cpp MatchingEngineTest.cpp MatchAggressorOrderTest.cpp
        MatchMarketOrderTest.cpp MatchingEnginePerfTest.cpp PriceLevelContainerTest.cpp
        PriceLevelContainerTestPerf.cpp)

LINK_DIRECTORIES(
        ${CMAKE_BINARY_DIR}/../../../lib/${CMAKE_BUILD_TYPE}
        $ENV{BOOST_LIB}
        /usr/local/lib)

add_executable(MatchingEngineTest ${SOURCE_FILES})

target_link_libraries(MatchingEngineTest
        MatchingEngineLib
        MessagesLib
        OrderBookLib
        UtilsLib
        boost_serialization
        boost_system
        boost_thread
        pthread
        protobuf
        )
