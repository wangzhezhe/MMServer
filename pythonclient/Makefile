# Makefile for building the sample on Linux, with protoc and the gRPC plugins
# installed on the system following the instructions in gRPC's INSTALL file.
#
# Parts of are taken verbatim from examples/cpp/helloworld/Makefile, so if you
# want to be strict about these things - gRPC's (3-clause BSD) license applies
# to these parts.
# 
# Eli Bendersky [http://eli.thegreenplace.net]
# This code is in the public domain.


ROOT_PATH = /ilab/users/zw241/cworkspace
PATH_PROTO = $(ROOT_PATH)/build_protobuf
PATH_GRPC = $(ROOT_PATH)/build_grpc
INCLUDE_DEPS = $(ROOT_PATH)/MMServer/deps

CPPFLAGS += -I$(PATH_PROTO)/include -I$(PATH_GRPC)/include -I$(INCLUDE_DEPS)
CXXFLAGS += -std=c++11
LDFLAGS += -L/usr/local/lib -L/usr/local/lib -L$(PATH_GRPC)/lib -L$(PATH_PROTO)/lib -Wl,-rpath $(PATH_GRPC)/lib -Wl,-rpath $(PATH_PROTO)/lib -lgrpc++_unsecure -lgrpc -lprotobuf -lpthread -ldl `pkg-config --libs protobuf grpc++ grpc`
PROTOC = $(PATH_PROTO)/bin/protoc
GRPC_CPP_PLUGIN = $(PATH_GRPC)/bin/grpc_cpp_plugin
GRPC_PYTHON_PLUGIN = $(PATH_GRPC)/bin/grpc_python_plugin
PROTOS_PATH = ../protos

all: metaclient_pb2.py

# Rule for producing the Python protobuf bindings
metaclient_pb2.py: ../protos/metaserver.proto
	$(PROTOC) -I $(PROTOS_PATH) --python_out=. --grpc_out=. \
		--plugin=protoc-gen-grpc=$(GRPC_PYTHON_PLUGIN) $<

clean:
	rm -rf $(EXECUTABLES) *.pb.cc *.pb.h *.pb.o *.o *_pb2.py *.pyc __pycache__ *_pb2_grpc.py