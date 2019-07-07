cmake_minimum_required(VERSION 3.7)
project(UtilsLib)

set(CMAKE_CXX_STANDARD 17)

include_directories(./include ../include $ENV{BOOST_INCLUDE})

#message (${CMAKE_BINARY_DIR})

set (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -mtune=native ") #-pg -D__PERF_TEST_ONLY___

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../lib/${CMAKE_BUILD_TYPE})

add_definitions(-DBOOST_LOG_DYN_LINK)
#add_definitions(-DBOOST_LOG_USE_CHAR)

#message (${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
execute_process (COMMAND bash -c "${CMAKE_C_COMPILER} --version")

#set (CMAKE_C_FLAGS_RELEASE)
message (${CMAKE_CXX_FLAGS_RELEASE})
#set (CMAKE_CXX_FLAGS_RELEASE -O3)


set(SOURCE_FILES ./src/EventScheduler.cpp ./src/tinyxml2.cpp ./include/EventScheduler.h FileLogger.cpp FileLogger.h)
add_library(UtilsLib ${SOURCE_FILES})

