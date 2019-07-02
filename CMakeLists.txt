project(MMServer)
cmake_minimum_required(VERSION 3.2)

add_compile_options(-std=c++11)

# GRPC and Protocol Buffers libraries location
list(APPEND CMAKE_PREFIX_PATH "~/cworkspace/build_grpc" "~/cworkspace/build_protobuf")

# Cmake find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)

set(PROTOS
    ${CMAKE_CURRENT_SOURCE_DIR}/protos/metaserver.proto
)

set(PROTO_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto-src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
include_directories(${PROTO_SRC_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS})



# Building server
add_executable(metaserver ./metaserver.cc ./utils/ipTool.cpp
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

target_include_directories(metaserver PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps>
)

target_link_libraries(metaserver
    gRPC::grpc++_reflection
    protobuf::libprotobuf
)

add_executable(metaclient ./metaclient.cc ./utils/ipTool.cpp
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

target_include_directories(metaclient PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps>
)

target_link_libraries(metaclient
    gRPC::grpc++_reflection
    protobuf::libprotobuf
)