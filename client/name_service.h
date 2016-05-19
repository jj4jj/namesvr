#pragma once
#include <functional>
#include <string>
namespace namesvc {


struct namesvc_config_t {
    std::string  server;
    std::string  name_lib_file;
};
typedef std::function<void(int ret)>    namesvc_regist_callback_t;




int             namesvc_init(const namesvc_config_t & conf);
void            namesvc_destroy();
void            namesvc_update();
int             namesvc_register(const char * name, uint64_t id, int type, namesvc_regist_callback_t cb);
const char *    namesvc_random(std::string & name, int type = 0);





}

