

#include "../metaclient.h"
#include "unistd.h"

int main()
{

    tl::engine clientEnginge("verbs", THALLIUM_CLIENT_MODE);
    MetaClient *metaclient = new MetaClient(&clientEnginge);

    metaclient->hello();

    
    std::string testkey1("test1");
    metaclient->Recordtime(testkey1);
    sleep(1);
    metaclient->Recordtime(testkey1);

    std::string testkey2("test2");

    metaclient->Recordtimestart(testkey2);
    sleep(1);
    metaclient->Recordtimetick(testkey2);
    
}