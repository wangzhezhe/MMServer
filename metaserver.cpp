//this is the metaserver start by the thallium
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include "unistd.h"
#include <time.h> /* for clock_gettime */
#include <map>
#include <queue>
#include "utils/ipTool.h"
#include "common.h"

#include <thallium.hpp>
#define BILLION 1000000000L

namespace tl = thallium;

tl::abt scope;

//write server addr into this folder
const std::string metaserverDir = "Metaserver_conf";

thallium::mutex tmutex;
std::map<std::string, timespec> timerMap;

tl::engine *globalServerEnginePtr = nullptr;

void recordtime(const tl::request &req, Input& input)
{
    //extract the key
    std::string key = input.m_recordKey;

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

        std::cout << "key (" << key << ")start timing" << std::endl;
    }
    else
    {
        clock_gettime(CLOCK_REALTIME, &end);
        start = timerMap[key];
        timerMap.erase(key);

        diff = (end.tv_sec - start.tv_sec) * 1.0 + (end.tv_nsec - start.tv_nsec) * 1.0 / BILLION;
        std::cout << "key (" << key << ") end timing, time " << diff << std::endl;
    }

    tmutex.unlock();
    req.respond(0);
}

void recordtimestart(const tl::request &req, Input& input)
{
    //extract the key
    std::string key = input.m_recordKey;

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
        std::cout << "key (" << key << ") is timing" << std::endl;
    }
    req.respond(0);
}

void recordtimetick(const tl::request &req, Input& input)
{
    //extract the key
    std::string key = input.m_recordKey;

    //record the time
    struct timespec start, tick;
    clock_gettime(CLOCK_REALTIME, &tick);

    double diff;
    if (timerMap.find(key) == timerMap.end())
    {
        // not found
        // insert key
        std::cout << "key (" << key << ") is not start timing yet" << std::endl;
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
        std::cout << "key (" << key << ") tick, time " << diff << std::endl;
    }
    req.respond(0);
}

void hello(const tl::request &req)
{
    std::cout << "Hello World!" << std::endl;
}

void runRerver(std::string networkingType)
{
    tl::engine serverEnginge(networkingType, THALLIUM_SERVER_MODE);
    globalServerEnginePtr = &serverEnginge;

    //for testing
    globalServerEnginePtr->define("hello", hello).disable_response();
    globalServerEnginePtr->define("recordtime", recordtime);
    globalServerEnginePtr->define("recordtimestart", recordtimestart);
    globalServerEnginePtr->define("recordtimetick", recordtimetick);

    std::string serverAddr = serverEnginge.self();
    std::string simplifiedAddr = getClientAdddr(networkingType, serverAddr);
    std::ofstream confFile;
    confFile.open(metaserverDir);
    confFile << simplifiedAddr << "\n";
    confFile.close();
    std::cout << "server listen on: " << simplifiedAddr << std::endl;
}

int main(int argc, char *argv[])
{
    //start server
    runRerver("verbs");
    return 0;
}