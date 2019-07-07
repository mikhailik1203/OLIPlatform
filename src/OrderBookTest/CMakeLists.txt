cmake_minimum_required(VERSION 3.8)
project(OrderBookTest)

set(CMAKE_CXX_STANDARD 17)

include_directories( ${PROJECT_SOURCE_DIR}/../OrderBookLib/
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/../include
        ${PROJECT_SOURCE_DIR}/../MessagesLib
        ${PROJECT_SOURCE_DIR}/../OMSServiceLib/.
        ${PROJECT_SOURCE_DIR}/../UtilsLib/src
        ${PROJECT_SOURCE_DIR}/../SecMasterCacheLib/
        ${PROJECT_SOURCE_DIR}
        $ENV{BOOST_INCLUDE})

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../bin/Debug)
message (${CMAKE_BINARY_DIR})
#message (${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
message (${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


set(SOURCE_FILES obDispatcherTest.cpp
        obProcessorTest.cpp
        orderBookTest.cpp
        )

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/../../../lib/Debug
        $ENV{BOOST_LIB})

add_executable(OrderBookTest ${SOURCE_FILES})

target_link_libraries(OrderBookTest
        OrderBookLib
        MessagesLib
        UtilsLib
        boost_serialization
        boost_system
        boost_thread
        protobuf
        )
