project(grpc-cmake-example)
cmake_minimum_required(VERSION 3.2)

add_compile_options(-std=c++11)

# GRPC and Protocol Buffers libraries location
list(APPEND CMAKE_PREFIX_PATH "/home1/zw241/Software/grpcinstall" "/home1/zw241/Software/protoinstall")

# Cmake find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)

set(PROTOS
    ${CMAKE_CURRENT_SOURCE_DIR}/protos/helloworld.proto
)

set(PROTO_SRC_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto-src)
file(MAKE_DIRECTORY ${PROTO_SRC_DIR})
include_directories(${PROTO_SRC_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_SRC_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_SRC_DIR} ${PROTOS})


# Building server
add_executable(adss_server ./server/server.cc
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)
target_link_libraries(adss_server
    gRPC::grpc++_reflection
    protobuf::libprotobuf
)


# Building server
#add_executable(greeter_server
#    ${CMAKE_CURRENT_SOURCE_DIR}/greeter_server.cc
#    ${PROTO_SRCS}
#    ${GRPC_SRCS}
#)
#target_link_libraries(greeter_server
#    gRPC::grpc++_reflection
#    protobuf::libprotobuf
#)

# Building client
#add_executable(greeter_client
#    ${CMAKE_CURRENT_SOURCE_DIR}/greeter_client.cc
#    ${PROTO_SRCS}
#    ${GRPC_SRCS}
#)
#target_link_libraries(greeter_client
#    gRPC::grpc++_reflection
#    protobuf::libprotobuf
#)