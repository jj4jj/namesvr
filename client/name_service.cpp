#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"


#include "dcpots/dcrpc/client/dccrpc.h"


#include "name_service.h"
using namespace std;
using namespace dcsutil;
using namespace dcrpc;

static struct {
    RpcClient   rpc;
} NAME_ENV;

NS_BEGIN(namesvc)

int             namesvc_init(const namesvc_config_t & conf){
    int ret = NAME_ENV.rpc.init(conf.server, 128);
    if (ret){
        GLOG_ERR("rpc client init error = %d !", ret);
        return -1;
    }
    //todo add local random name lib
    return 0;
}
void            namesvc_update(){
    NAME_ENV.rpc.update();
}
void            namesvc_destroy(){
    NAME_ENV.rpc.destroy();
}
int             namesvc_register(const char * name, uint64_t id, int type , namesvc_regist_callback_t cb){
    RpcValues args;
    //register(name, id, type)
    args.adds(name);
    args.addi(id);
    args.addi(type);
    return NAME_ENV.rpc.call("name", args, std::bind(cb, std::placeholders::_1));
}
const char *    namesvc_random(int type){
#warning "todo implementation"
    return nullptr;
}


NS_END()