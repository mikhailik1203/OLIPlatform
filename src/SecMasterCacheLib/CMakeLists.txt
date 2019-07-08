cmake_minimum_required(VERSION 3.8)
project(SecMasterCacheLib)

set(CMAKE_CXX_STANDARD 17)

include_directories(. ../include ../MessagesLib/src/ $ENV{BOOST_INCLUDE})

#message (${CMAKE_BINARY_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../lib/Debug)

#message (${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

add_definitions(-DFUSION_MAX_VECTOR_SIZE=40)
add_definitions(-DBOOST_MPL_CFG_NO_PREPROCESSED_HEADERS)
add_definitions(-DBOOST_MPL_LIMIT_VECTOR_SIZE=40)
add_definitions(-DBOOST_MPL_LIMIT_MAP_SIZE=30)

set(SOURCE_FILES ./InstrumentDispatcher.cpp )

add_library(SecMasterCacheLib ${SOURCE_FILES})