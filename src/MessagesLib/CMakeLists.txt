cmake_minimum_required(VERSION 3.7)
project(MessagesLib)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/../include
                    ${PROJECT_SOURCE_DIR}
                    ${PROJECT_SOURCE_DIR}/src/protobuf)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../lib/${CMAKE_BUILD_TYPE})

#message (${PROJECT_SOURCE_DIR})

add_custom_target(generate_proto)
add_custom_command(
        TARGET generate_proto
        PRE_BUILD
        COMMAND protoc --proto_path=${PROJECT_SOURCE_DIR}/src/protobuf --cpp_out=${PROJECT_SOURCE_DIR}/src/protobuf ${PROJECT_SOURCE_DIR}/src/protobuf/oli_messageslib.proto
        COMMENT " Generating proto for MessageLib"
)

set(SOURCE_FILES ./src/protobuf/oli_messageslib.pb.cc
                 ./src/InMessageWrapper.cpp
                 ./src/MessageLib.cpp
                 ./src/MsgHelpers.cpp
                 ./src/OutMessageWrapper.cpp)

set (CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native -mtune=native ") #-pg -D__PERF_TEST_ONLY___

add_library(MessagesLib ${SOURCE_FILES})

add_dependencies(MessagesLib generate_proto)
