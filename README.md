Metadata Management Server (MMServer) is a simple server for meta data management based on GRPC. This is the single node application for small scale testing cases. It provides two main function currently.

- distributed timer
- message communication (shared space and message queue)


### Build

The only depedency for this project is the grpc and protobuf.
Refer to [this](https://github.com/IvanSafonov/grpc-cmake-example) about building grpc and protocol buffer

**CMake**

No matter to use cmake or automake, it is better to set the separate the build dir.

for example, if the build dir for grpc is `~/cworkspace/build_grpc`  the and build dir for protobuf is `~/cworkspace/build_protobuf`

put them in following parts in CMakeLists.txt

```
list(APPEND CMAKE_PREFIX_PATH "~/cworkspace/build_grpc" "~/cworkspace/build_protobuf")
```

then create another build dir to build the MMServer

```
mkdir build_MMServer
cd build_MMServer
cmake ../MMServer
make
```

**Makefile**

There is a `Makefile` in the build folder used to build the project by Makefile explicitly.


```
export ROOT_PATH=~/cworkspace
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$ROOT_PATH/build_protobuf/lib/pkgconfig
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$ROOT_PATH/build_grpc/lib/pkgconfig
```
then execute make

### Example

The addr info will be write into the dir Metaserver 

start the server:

```
-bash-4.2$ rm -rf Metaserver/
-bash-4.2$ ./metaserver 
key (world) start timing
key (world) end timing, time (1.000475)
key (pattern_tick) start timing
key (pattern_tick) is timing
key (pattern_tick) tick, time (0.000244)
key (pattern_tick) tick, time (0.000391)
```
start the client:

```
-bash-4.2$ ./metaclient 
Timer received: OK
Timer received: OK
Get pattern1 recieve: NULL
Put meta1 recieve: OK
Put meta2 recieve: OK
Put meta3 recieve: OK
Get pattern1 recieve: meta1
Get pattern1 recieve: meta2
Get pattern1 recieve: meta3
Get pattern1 recieve: NULL
------test tick------
Put pattern_tick 1st recieve: OK
Put pattern_tick 2st recieve: OK
Put pattern_tick 3st recieve: OK
Put pattern_tick 4st recieve: OK
```
for pythonclient, install the grpc and protocol for python firstly, then execute make in pythonclient dir.

### TODO List
  - delete addr folder before start