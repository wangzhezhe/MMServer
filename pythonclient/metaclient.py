


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
"""The Python implementation of the GRPC workflowserver.Greeter client."""

from __future__ import print_function

from concurrent import futures
import time
import grpc
import os 
import netifaces as ni
from threading import Thread
from os import listdir
from os.path import isfile, join

import metaserver_pb2
import metaserver_pb2_grpc


def Recordtimetick(addr,key):
    # NOTE(gRPC Python Team): .close() is possible on a channel and should be
    # used in circumstances in which the with statement does not fit the needs
    # of the code.
    
    with grpc.insecure_channel(addr) as channel:
        stub = metaserver_pb2_grpc.MetaStub(channel)

        request = metaserver_pb2.TimeRequest()

        # refer to https://developers.google.com/protocol-buffers/docs/reference/python-generated#embedded_message

        request.key=key
        response = stub.Recordtimetick(request)
   
    print("Recordtimetick client received: " + response.message)

def Recordtimestart(addr,key):
    # NOTE(gRPC Python Team): .close() is possible on a channel and should be
    # used in circumstances in which the with statement does not fit the needs
    # of the code.
    
    with grpc.insecure_channel(addr) as channel:
        stub = metaserver_pb2_grpc.MetaStub(channel)

        request = metaserver_pb2.TimeRequest()

        # refer to https://developers.google.com/protocol-buffers/docs/reference/python-generated#embedded_message

        request.key=key
        response = stub.Recordtimestart(request)
   
    print("Recordtimestart client received: " + response.message)

def Recordtime(addr,key):
    # NOTE(gRPC Python Team): .close() is possible on a channel and should be
    # used in circumstances in which the with statement does not fit the needs
    # of the code.
    
    with grpc.insecure_channel(addr) as channel:
        stub = metaserver_pb2_grpc.MetaStub(channel)

        request = metaserver_pb2.TimeRequest()

        # refer to https://developers.google.com/protocol-buffers/docs/reference/python-generated#embedded_message

        request.key=key
        response = stub.Recordtime(request)
   
    print("Recordtime client received: " + response.message)

def putMeta(addr,key,metainfo):
    # NOTE(gRPC Python Team): .close() is possible on a channel and should be
    # used in circumstances in which the with statement does not fit the needs
    # of the code.
    
    with grpc.insecure_channel(addr) as channel:
        stub = metaserver_pb2_grpc.MetaStub(channel)

        request = metaserver_pb2.PutRequest()

        # refer to https://developers.google.com/protocol-buffers/docs/reference/python-generated#embedded_message

        request.key=key
        request.value=metainfo

        
        print("debug put request")
        print(metainfo)
        response = stub.Putmeta(request)
        #send the publish event
   
    print("PutRequest client received: " + response.message)


def getMeta(addr, key):
    # NOTE(gRPC Python Team): .close() is possible on a channel and should be
    # used in circumstances in which the with statement does not fit the needs
    # of the code.
    
    with grpc.insecure_channel(addr) as channel:
        stub = metaserver_pb2_grpc.MetaStub(channel)

        request = metaserver_pb2.GetRequest()

        # refer to https://developers.google.com/protocol-buffers/docs/reference/python-generated#embedded_message

        request.key=key

        response = stub.Getmeta(request)
        #send the publish event
   
    return response.message
    
def putMetaspace(addr,key,metainfo):
    # NOTE(gRPC Python Team): .close() is possible on a channel and should be
    # used in circumstances in which the with statement does not fit the needs
    # of the code.
    
    with grpc.insecure_channel(addr) as channel:
        stub = metaserver_pb2_grpc.MetaStub(channel)

        request = metaserver_pb2.PutRequest()

        # refer to https://developers.google.com/protocol-buffers/docs/reference/python-generated#embedded_message

        request.key=key
        request.value=metainfo

        
        print("debug put request")
        print(metainfo)
        response = stub.Putmetaspace(request)
        #send the publish event
   
    print("PutRequest client received: " + response.message)


def getMetaspace(addr, key):
    # NOTE(gRPC Python Team): .close() is possible on a channel and should be
    # used in circumstances in which the with statement does not fit the needs
    # of the code.
    
    with grpc.insecure_channel(addr) as channel:
        stub = metaserver_pb2_grpc.MetaStub(channel)

        request = metaserver_pb2.GetRequest()

        # refer to https://developers.google.com/protocol-buffers/docs/reference/python-generated#embedded_message

        request.key=key

        response = stub.Getmetaspace(request)
        #send the publish event
   
    return response.message



def getServerAddr():
    # only for one server test case
    serverAddrDir  = "/project1/parashar-001/zw241/software/eventDrivenWorkflow/src/metadatamanagement/Metaserver"
    onlyfiles = [f for f in listdir(serverAddrDir) if isfile(join(serverAddrDir, f))]
    return onlyfiles




if __name__ == '__main__':

    addrList = getServerAddr()
    print (addrList)


    addr = addrList[0]

    # this shoule be deleted
    key = "test"
    metainfo = "GRID[<0,0>:<1,1>]%TS[5]"
    putMeta(addr, key,metainfo)

    meta=getMeta(addr, key)
    print("get meta", meta)

    meta=getMeta(addr, "emptykey")
    print("get meta", meta)

    
    reply=getMetaspace(addr,"testkey")
    print("test get metaspace reply:",reply)
    reply=putMetaspace(addr,"testkey","testmetainfo")
    print("test put metaspace reply:",reply)
    reply=getMetaspace(addr,"testkey")
    print("test get metaspace reply:",reply)


