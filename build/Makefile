#
# Copyright 2015 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# TIPS pubsubclient will use the proto files in different folder, build that firstly if the proto files are modified

# ROOT_PATH = /lustre/atlas/scratch/zw241/csc103/Software
ROOT_PATH = /home1/zw241/Software
LD_PATH_PROTO = $(ROOT_PATH)/grpc/third_party/protobuf/src
LD_PATH_GRPC = $(ROOT_PATH)/grpcinstall/include

INCLUDE_DEPS = /project1/parashar-001/zw241/software/eventDrivenWorkflow/deps
HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)
#CXX = g++ -I$(LD_PATH_PROTO) -I$(LD_PATH_GRPC) 
CXX = mpic++ -I$(LD_PATH_PROTO) -I$(LD_PATH_GRPC) -I$(INCLUDE_DEPS)
CPPFLAGS += `pkg-config --cflags protobuf grpc` -I/home1/zw241/Software/grpc/third_party/protobuf/src 
CXXFLAGS += -std=c++11 -fopenmp
#CXXFLAGS += -std=c++11 -I../deps/  -I../deps/hiredis/ -DDEBUG
ifeq ($(SYSTEM),Darwin)
LDFLAGS += -L/usr/local/lib -L/home1/zw241/Software/protoinstall/lib -L/home1/zw241/Software/grpc/libs/opt/protobuf -L/home1/zw241/Software/grpcinstall/lib `pkg-config --libs protobuf grpc++ grpc`\
           -lgrpc++_reflection\
           -ldl -lpthread -lgpr
else
LDFLAGS += -L/usr/local/lib -L$(ROOT_PATH)/grpcinstall/lib -L$(ROOT_PATH)/grpc/libs/opt/protobuf -I$(LD_PATH) `pkg-config --libs protobuf grpc++ grpc`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl -lpthread -lgpr -luuid -fopenmp -luuid 
endif
PROTOC = /home1/zw241/Software/grpc/third_party/protobuf/src/protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = ../proto

vpath %.proto $(PROTOS_PATH)

all: metaserver metaclient


dlm.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o dlm.o ../utils/dlm/dlm.cc

groupManager.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o groupManager.o ../utils/groupManager/groupManager.cpp

metaserver: metaserver.pb.o metaserver.grpc.pb.o groupManager.o dlm.o metaserver.o 
	$(CXX) $^ $(LDFLAGS) -o $@

metaclient: metaserver.pb.o metaserver.grpc.pb.o groupManager.o dlm.o metaclient.o 
	$(CXX) $^ $(LDFLAGS) -o $@

.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<

clean:
	rm -rf *.o *.pb.cc *.pb.h metaserver metaclient Metaserver


# The following is to test your system and ensure a smoother experience.
# They are by no means necessary to actually compile a grpc-enabled software.

PROTOC_CMD = which $(PROTOC)
PROTOC_CHECK_CMD = $(PROTOC) --version | grep -q libprotoc.3
PLUGIN_CHECK_CMD = which $(GRPC_CPP_PLUGIN)
HAS_PROTOC = $(shell $(PROTOC_CMD) > /dev/null && echo true || echo false)
ifeq ($(HAS_PROTOC),true)
HAS_VALID_PROTOC = $(shell $(PROTOC_CHECK_CMD) 2> /dev/null && echo true || echo false)
endif
HAS_PLUGIN = $(shell $(PLUGIN_CHECK_CMD) > /dev/null && echo true || echo false)

SYSTEM_OK = false
ifeq ($(HAS_VALID_PROTOC),true)
ifeq ($(HAS_PLUGIN),true)
SYSTEM_OK = true
endif
endif

system-check:
ifneq ($(HAS_VALID_PROTOC),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have protoc 3.0.0 installed in your path."
	@echo "Please install Google protocol buffers 3.0.0 and its compiler."
	@echo "You can find it here:"
	@echo
	@echo "   https://github.com/google/protobuf/releases/tag/v3.0.0"
	@echo
	@echo "Here is what I get when trying to evaluate your version of protoc:"
	@echo
	-$(PROTOC) --version
	@echo
	@echo
endif
ifneq ($(HAS_PLUGIN),true)
	@echo " DEPENDENCY ERROR"
	@echo
	@echo "You don't have the grpc c++ protobuf plugin installed in your path."
	@echo "Please install grpc. You can find it here:"
	@echo
	@echo "   https://github.com/grpc/grpc"
	@echo
	@echo "Here is what I get when trying to detect if you have the plugin:"
	@echo
	-which $(GRPC_CPP_PLUGIN)
	@echo
	@echo
endif
ifneq ($(SYSTEM_OK),true)
	@false
endif
