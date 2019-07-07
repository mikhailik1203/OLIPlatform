cmake_minimum_required(VERSION 3.8)
project(OrderBookLib)

set(CMAKE_CXX_STANDARD 17)

include_directories(. ../include ../SecMasterCacheLib ../MessagesLib ../MessagesLib/src $ENV{BOOST_INCLUDE})

#message (${CMAKE_BINARY_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../lib/${CMAKE_BUILD_TYPE})

#message (${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

set(SOURCE_FILES ./OBProcessor.cpp ./OBProcessorFactory.cpp ./OBTransportAdaptor.cpp ./OrderBookDispatcher.cpp)

add_library(OrderBookLib ${SOURCE_FILES})


