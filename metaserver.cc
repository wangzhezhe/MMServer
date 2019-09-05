// suplement this part if there is extra time
// provide two interfaces
// putMetaData (variable name as key, meta data as value, if there exist the key, update the meta)
// checkMetaData (if the key exist, return OK and meta data, else, return EMPTY)

//this is the tempory implementation of key value server for testing
//this shoule be put into the workflowserver.cc in future

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <queue>
#include <omp.h>

#include <grpc++/grpc++.h>
#include "metaserver.grpc.pb.h"
#include <uuid/uuid.h>

#include "unistd.h"
#include <mutex>
#include <stdint.h> /* for uint64 definition */
#include <stdlib.h> /* for exit() definition */
#include <time.h>   /* for clock_gettime */
#include <map>
#include <queue>

#include "deps/spdlog/spdlog.h"

#include "utils/ipTool.h"

#define BILLION 1000000000L

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using metaserver::Meta;

using metaserver::HelloReply;
using metaserver::HelloRequest;

using metaserver::PutReply;
using metaserver::PutRequest;

using metaserver::GetReply;
using metaserver::GetRequest;

using metaserver::TimeReply;
using metaserver::TimeRequest;

using namespace std;

//write server addr into this folder
const string metaserverDir = "Metaserver";

mutex tmutex;
map<string, timespec> timerMap;

mutex metaMutex;
map<string, queue<string>> metaMap;

mutex metaSpaceMutex;
map<string, string> metaSpace;

// Logic and data behind the server's behavior.
class MetaServiceImpl final : public Meta::Service
{
    Status SayHello(ServerContext *context, const HelloRequest *request,
                    HelloReply *reply) override
    {
        std::string prefix("Hello ");
        reply->set_message(prefix + request->name());
        return Status::OK;
    }

    //record the time
    //if there is no record in map, record the key and insert the value into map
    //if there is record, print the final time and delete the key

    Status Recordtime(ServerContext *context, const TimeRequest *request,
                      TimeReply *reply) override
    {

        //extract the key
        string key = request->key();

        //record the time
        struct timespec start, end;
        double diff;
        tmutex.lock();
        if (timerMap.find(key) == timerMap.end())
        {
            // not found
            // insert key

            clock_gettime(CLOCK_REALTIME, &start);
            
            timerMap[key] = start;
            
            std::cout<<"key (" << key << ")start timing" << std::endl;
        }
        else
        {
            // record time again
            // caculate the difference
            // output

            clock_gettime(CLOCK_REALTIME, &end);

            start = timerMap[key];
            timerMap.erase(key);

            diff = (end.tv_sec - start.tv_sec) * 1.0 + (end.tv_nsec - start.tv_nsec) * 1.0 / BILLION;
            std::cout<<"key (" << key << ") end timing, time " << diff <<std::endl;
        }
        tmutex.unlock();

        reply->set_message("OK");
        return Status::OK;
    }

    Status Recordtimestart(ServerContext *context, const TimeRequest *request,
                           TimeReply *reply) override

    {

        //extract the key
        string key = request->key();

        //record the time
        struct timespec start, end;
        double diff;
        if (timerMap.find(key) == timerMap.end())
        {
            // not found
            // insert key
            clock_gettime(CLOCK_REALTIME, &start);
            tmutex.lock();
            timerMap[key] = start;
            tmutex.unlock();

            std::cout << "key (" << key << ") start timing" << std::endl;
        }
        else
        {
            std::cout << "key (" << key << ") is timing"<<std::endl;
        }
        reply->set_message("OK");
        return Status::OK;
    }

    Status Recordtimetick(ServerContext *context, const TimeRequest *request,
                          TimeReply *reply) override
    {

        //extract the key
        string key = request->key();

        //record the time
        struct timespec start, tick;
        clock_gettime(CLOCK_REALTIME, &tick);

        double diff;
        if (timerMap.find(key) == timerMap.end())
        {
            // not found
            // insert key
            std::cout<<"key ("<<key<<") is not start timing yet"<<std::endl;
        }
        else
        {
            // tick time again
            // caculate the difference
            // output

            clock_gettime(CLOCK_REALTIME, &tick);
            tmutex.lock();
            start = timerMap[key];
            tmutex.unlock();

            diff = (tick.tv_sec - start.tv_sec) * 1.0 + (tick.tv_nsec - start.tv_nsec) * 1.0 / BILLION;
            std::cout<<"key ("<<key<<") tick, time "<<diff<<std::endl;
        }

        reply->set_message("OK");
        return Status::OK;
    }

    Status Putmeta(ServerContext *context, const PutRequest *request,
                   PutReply *reply) override

    {
        //extract the key
        string key = request->key();
        string meta = request->value();

        //if key not exist in metamap, insert key value pair
        if (metaMap.find(key) == metaMap.end())
        {
            // not found
            queue<string> metaQueue;
            metaQueue.push(meta);

            metaMutex.lock();
            metaMap[key] = metaQueue;
            metaMutex.unlock();
        }
        else
        {

            metaMutex.lock();
            metaMap[key].push(meta);
            metaMutex.unlock();
        }

        reply->set_message("OK");
        return Status::OK;
    }

    Status Getmeta(ServerContext *context, const GetRequest *request,
                   GetReply *reply) override

    {

        //extract the key
        string key = request->key();

        //if key not exist in metamap, insert key value pair
        if (metaMap.find(key) == metaMap.end())
        {
            // not found
            reply->set_message("NULL");
            return Status::OK;
        }
        else
        {
            if (metaMap[key].empty())
            {
                reply->set_message("NULL");
                return Status::OK;
            }
            else
            {
                metaMutex.lock();
                string meta = metaMap[key].front();
                metaMap[key].pop();
                metaMutex.unlock();

                reply->set_message(meta);
                return Status::OK;
            }
        }
    }

    Status Putmetaspace(ServerContext *context, const PutRequest *request,
                        PutReply *reply) override
    {
        //extract the key
        string key = request->key();
        string meta = request->value();

        //if key not exist in metamap, insert key value pair
        if (metaSpace.find(key) == metaSpace.end())
        {
            // not found

            metaSpaceMutex.lock();
            metaSpace[key] = meta;
            metaSpaceMutex.unlock();
        }
        else
        {
            //TODO add the update interface, which will update the key anyway
            std::cout<<"key ("<<key<<") with meta ("<<meta<<") is stored already"<<std::endl;
        }

        reply->set_message("OK");
        return Status::OK;
    }

    Status Getmetaspace(ServerContext *context, const GetRequest *request,
                        GetReply *reply) override
    {
        //extract the key
        string key = request->key();

        if (metaSpace.find(key) == metaSpace.end())
        {
            // not found
            reply->set_message("NULL");
            return Status::OK;
        }
        else
        {

            metaSpaceMutex.lock();
            string meta = metaSpace[key];
            metaSpaceMutex.unlock();

            reply->set_message(meta);
            return Status::OK;
        }
    }
};

void writeSocket(string socketAddr)
{

    //dele folder
    //deleteDir(metaserverDir);

    createDir(metaserverDir);

    char addrFile[100];
    sprintf(addrFile, "%s/%s", metaserverDir.data(), socketAddr.data());

    FILE *fpt = fopen(addrFile, "w");

    if (fpt == NULL)
    {
        std::cout<<"failed to create " << addrFile <<std::endl;
        return;
    }

    fclose(fpt);

    return;
}

//key is the variable name
//value is a string containing the json info matching with the rpc call
//runserver, single server is ok for testing
void RunServer(string serverIP, string serverPort)
{
    //init thread pool

    string socketAddr = serverIP + ":" + serverPort;

    spdlog::debug("server socket addr {}", socketAddr.data());
    string server_address(socketAddr);

    writeSocket(socketAddr);

    MetaServiceImpl service;

    ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<Server> server(builder.BuildAndStart());
    //std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char *argv[])
{

    int freePort = getFreePortNum();
    //this option should be automic in multithread case
    string ServerPort = to_string(freePort);

    string ServerIP;
    //TODO send the parameter from the commandline
    //eno1

    if (argc!=2){
        std::cout << "<server> <interface>" <<std::endl;
        return 0;
    }

    std::string interface = argv[1];
    recordIPPortWithoutFile(ServerIP,interface);

    //start server
    RunServer(ServerIP, ServerPort);

    return 0;
}
