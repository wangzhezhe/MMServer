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

#ifdef USE_GNI
extern "C"
{
#include <rdmacred.h>
}
#include <mercury.h>
#include <margo.h>
#define DIE_IF(cond_expr, err_fmt, ...)                                                                           \
    do                                                                                                            \
    {                                                                                                             \
        if (cond_expr)                                                                                            \
        {                                                                                                         \
            fprintf(stderr, "ERROR at %s:%d (" #cond_expr "): " err_fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__); \
            exit(1);                                                                                              \
        }                                                                                                         \
    } while (0)
#endif

namespace tl = thallium;

//write server addr into this folder
const std::string metaserverAddr = "Metaserver_conf";
const std::string metaserverCred = "Metaserver_cred_conf";

struct globalTimer
{
    tl::mutex tmutex;
    std::map<std::string, timespec> timerMap;
};

tl::engine *globalServerEnginePtr = nullptr;
globalTimer *gtimer = nullptr;

void recordtime(const tl::request &req, Input &input)
{
    //extract the key
    std::string key = input.m_recordKey;

    //record the time
    struct timespec start, end;
    double diff;
    if (gtimer->timerMap.find(key) == gtimer->timerMap.end())
    {
        // not found
        // insert key
        clock_gettime(CLOCK_REALTIME, &start);
        gtimer->tmutex.lock();
        gtimer->timerMap[key] = start;
        gtimer->tmutex.unlock();
        std::cout << "key (" << key << ") start timing" << std::endl;
    }
    else
    {
        clock_gettime(CLOCK_REALTIME, &end);
        start = gtimer->timerMap[key];
        gtimer->tmutex.lock();
        gtimer->timerMap.erase(key);
        gtimer->tmutex.unlock();
        diff = (end.tv_sec - start.tv_sec) * 1.0 + (end.tv_nsec - start.tv_nsec) * 1.0 / BILLION;
        std::cout << "key (" << key << ") end timing, time " << diff << std::endl;
    }

    req.respond(0);
}

void recordtimestart(const tl::request &req, Input &input)
{
    //extract the key
    std::string key = input.m_recordKey;

    //record the time
    struct timespec start, end;
    double diff;
    if (gtimer->timerMap.find(key) == gtimer->timerMap.end())
    {
        // not found
        // insert key
        clock_gettime(CLOCK_REALTIME, &start);
        gtimer->tmutex.lock();
        gtimer->timerMap[key] = start;
        gtimer->tmutex.unlock();

        std::cout << "key (" << key << ") start timing" << std::endl;
    }
    else
    {
        std::cout << "key (" << key << ") is timing" << std::endl;
    }
    req.respond(0);
}

void recordtimetick(const tl::request &req, Input &input)
{
    //extract the key
    std::string key = input.m_recordKey;

    //record the time
    struct timespec start, tick;
    clock_gettime(CLOCK_REALTIME, &tick);

    double diff;
    if (gtimer->timerMap.find(key) == gtimer->timerMap.end())
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
        gtimer->tmutex.lock();
        start = gtimer->timerMap[key];
        gtimer->tmutex.unlock();

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
#ifdef USE_GNI
    uint32_t drc_credential_id;
    drc_info_handle_t drc_credential_info;
    uint32_t drc_cookie;
    char drc_key_str[256] = {0};
    int ret;

    struct hg_init_info hii;
    memset(&hii, 0, sizeof(hii));

    ret = drc_acquire(&drc_credential_id, DRC_FLAGS_FLEX_CREDENTIAL);
    DIE_IF(ret != DRC_SUCCESS, "drc_acquire");

    ret = drc_access(drc_credential_id, 0, &drc_credential_info);
    DIE_IF(ret != DRC_SUCCESS, "drc_access");
    drc_cookie = drc_get_first_cookie(drc_credential_info);
    sprintf(drc_key_str, "%u", drc_cookie);
    hii.na_init_info.auth_key = drc_key_str;

    ret = drc_grant(drc_credential_id, drc_get_wlm_id(), DRC_FLAGS_TARGET_WLM);
    DIE_IF(ret != DRC_SUCCESS, "drc_grant");

    printf("grant the drc_credential_id: %d\n", drc_credential_id);
    printf("use the drc_key_str %s\n", drc_key_str);

    //output the credential id into the config files
    std::ofstream credFile;
    credFile.open(metaserverCred);
    credFile << drc_credential_id << "\n";
    credFile.close();
    margo_instance_id mid = margo_init_opt("gni", MARGO_SERVER_MODE, &hii, 0, -1);
    tl::engine serverEnginge(mid);
    globalServerEnginePtr = &serverEnginge;

#else
    //there is the problem of the scope
    tl::engine serverEnginge(networkingType, THALLIUM_SERVER_MODE);
    globalServerEnginePtr = &serverEnginge;

#endif
    //all the thallium objects have their lifetime limited to the lifetime of the margo instance id or the thallium engine
    gtimer = new globalTimer();

    std::string serverAddr = serverEnginge.self();
    std::cout << "original addr: " << serverAddr << std::endl;
    //for testing
    globalServerEnginePtr->define("hello", hello).disable_response();
    globalServerEnginePtr->define("recordtime", recordtime);
    globalServerEnginePtr->define("recordtimestart", recordtimestart);
    globalServerEnginePtr->define("recordtimetick", recordtimetick);

    std::string simplifiedAddr = getClientAdddr(networkingType, serverAddr);
    std::ofstream confFile;
    confFile.open(metaserverAddr);
    confFile << simplifiedAddr << "\n";
    confFile.close();
    std::cout << "server listen on: " << simplifiedAddr << std::endl;

#ifdef USE_GNI
    // call the margo_finalize() explicitly
    // When you initialize the thallium engine with an existing margo instance
    // otherwise, there will be an error about the abtio and mutex
    margo_wait_for_finalize(mid);
#endif
}

int main(int argc, char *argv[])
{
    tl::abt scope;

    //start server
    if (argc != 2)
    {

        std::cerr << "Usage: metaserver protocol" << std::endl;
        exit(0);
    }
    std::string protocol = std::string(argv[1]);
    runRerver(protocol);
    return 0;
}
