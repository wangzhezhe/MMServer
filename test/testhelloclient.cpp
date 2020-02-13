#include "../metaclient.h"
#include "unistd.h"
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

const std::string metaserverCred = "Metaserver_cred_conf";


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: testclient protocol" << std::endl;
        exit(0);
    }
    std::string protocol = std::string(argv[1]);
    MetaClient *metaclient = NULL;

#ifdef USE_GNI
    //get the drc id from the shared file
    std::ifstream infile(metaserverCred);
    std::string cred_id;
    std::getline(infile, cred_id);
    std::cout << "load cred_id: " << cred_id << std::endl;

    struct hg_init_info hii;
    memset(&hii, 0, sizeof(hii));
    char drc_key_str[256] = {0};
    uint32_t drc_cookie;
    uint32_t drc_credential_id;
    drc_info_handle_t drc_credential_info;
    int ret;
    drc_credential_id = (uint32_t)atoi(cred_id.c_str());

    ret = drc_access(drc_credential_id, 0, &drc_credential_info);
    DIE_IF(ret != DRC_SUCCESS, "drc_access %u", drc_credential_id);
    drc_cookie = drc_get_first_cookie(drc_credential_info);

    sprintf(drc_key_str, "%u", drc_cookie);
    hii.na_init_info.auth_key = drc_key_str;
    printf("use the drc_key_str %s\n", drc_key_str);

    margo_instance_id mid;
    mid = margo_init_opt("gni", MARGO_CLIENT_MODE, &hii, 0, -1);

    tl::engine clientEnginge(mid);
    metaclient = new MetaClient(&clientEnginge);
#else
    tl::engine clientEnginge(protocol, THALLIUM_CLIENT_MODE);
    metaclient = new MetaClient(&clientEnginge);
#endif

    metaclient->hello();
    return 0;
}
