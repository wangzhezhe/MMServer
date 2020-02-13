#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include "unistd.h"
#include <time.h> /* for clock_gettime */
#include <map>
#include <queue>
#include "../utils/ipTool.h"

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

tl::engine *globalServerEnginePtr = nullptr;

void hello(const tl::request &req)
{
    tl::mutex tmutex;
    tmutex.lock();
    std::cout << "Hello World!" << std::endl;
    tmutex.unlock();
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

    std::string serverAddr = serverEnginge.self();
    std::cout << "original addr: " << serverAddr << std::endl;
    //for testing
    globalServerEnginePtr->define("hello", hello).disable_response();

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
    serverEnginge.wait_for_finalize();
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
