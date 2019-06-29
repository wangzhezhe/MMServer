# Makefile for building the sample on Linux, with protoc and the gRPC plugins
# installed on the system following the instructions in gRPC's INSTALL file.
#
# Parts of are taken verbatim from examples/cpp/helloworld/Makefile, so if you
# want to be strict about these things - gRPC's (3-clause BSD) license applies
# to these parts.
# 
# Eli Bendersky [http://eli.thegreenplace.net]
# This code is in the public domain.

CXX = g++
ROOT_PATH = /home1/zw241/Software
CPPFLAGS += -I/usr/local/include -pthread -I$(ROOT_PATH)/grpc/third_party/protobuf/src -I$(ROOT_PATH)/grpcinstall/include
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib -L/usr/local/lib -L$(ROOT_PATH)/protoinstall/lib -L$(ROOT_PATH)/grpc/libs/opt/protobuf -L$(ROOT_PATH)/grpcinstall/lib -lgrpc++_unsecure -lgrpc -lprotobuf -lpthread -ldl `pkg-config --libs protobuf grpc++ grpc`
PROTOC = /home1/zw241/Software/grpc/third_party/protobuf/src/protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`
GRPC_PYTHON_PLUGIN = grpc_python_plugin
GRPC_PYTHON_PLUGIN_PATH ?= `which $(GRPC_PYTHON_PLUGIN)`
PROTOS_PATH = ../../proto

all: metaclient_pb2.py

# Rule for producing the Python protobuf bindings
metaclient_pb2.py: ../../proto/metaserver.proto
	$(PROTOC) -I $(PROTOS_PATH) --python_out=. --grpc_out=. \
		--plugin=protoc-gen-grpc=$(GRPC_PYTHON_PLUGIN_PATH) $<

clean:
	rm -rf $(EXECUTABLES) *.pb.cc *.pb.h *.pb.o *.o *_pb2.py *.pyc __pycache__ *_pb2_grpc.py