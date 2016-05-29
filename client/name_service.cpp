#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "dcpots/base/dcutils.hpp"

#include "dcpots/dcrpc/client/dccrpc.h"

#include "name_random.h"

#include "name_service.h"
using namespace std;
using namespace dcsutil;
using namespace dcrpc;

NS_BEGIN(namesvc)

static struct {
    RpcClient       rpc;
    name_random_t * name_random{ nullptr };
} NAME_ENV;


int             namesvc_init(const namesvc_config_t & conf){
    int ret = NAME_ENV.rpc.init(conf.server, 128);
    if (ret){
        GLOG_ERR("rpc client init error = %d !", ret);
        return -1;
    }
    NAME_ENV.name_random = name_random_create(conf.name_lib_file.c_str());
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
    if (!name || !*name){
        GLOG_ERR("register name is null > ilegal param !");
        return -1;
    }
    RpcValues args;
    //register(name, id, type)
    args.adds(name);
    args.seti(0);//name register
    args.addi(id);
    args.addi(type);
    return NAME_ENV.rpc.call("name", args, std::bind(cb, std::placeholders::_1));
}
int     namesvc_exists(const std::string & name, int type, namesvc_exists_callback_t cb){
    if (name.empty()){
        GLOG_ERR("register name is null > ilegal param !");
        return -1;
    }
    RpcValues args;
    //register(name, id, type)
    args.adds(name);
    args.seti(1);//name exists
    args.addi(type);//type
    return NAME_ENV.rpc.call("name", args, [cb](int ret, const RpcValues & res){
        if (ret){
            cb(ret, true);
        }
        else {
            cb(ret, res.geti() > 0 ? true: false);
        }
    });
}
const char *    namesvc_random(string & name, int type){
    //#warning "todo implementation"    
    return dcsutil::strcharsetrandom(name);    
    //namesvc_random(NAME_ENV.name_random, type);
}


NS_END()