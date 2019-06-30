Metadata Management Server (MMServer) is the simple server for meta data management based on GRPC. This is the single node application for small scale testing cases. It provides two main function currently.

- distributed timer
- message communication (shared space and message queue)


### Build

The only depedency for this project is the grpc and protobuf.

```
Refer to [this](https://github.com/IvanSafonov/grpc-cmake-example) about building grpc and protocol buffer
```
No matter to use cmake or automake, it is better to set the separate the build dir.

for example, if the build dir for grpc is `~/cworkspace/build_grpc`  the and build dir for protobuf is `~/cworkspace/build_protobuf`

put them in following parts in CMakeLists.txt

```
list(APPEND CMAKE_PREFIX_PATH "~/cworkspace/build_grpc" "~/cworkspace/build_protobuf")
```

**Makefile**

**CMake**

### timer for destributed components

### message communication based on shared space

### message communication based on message queue