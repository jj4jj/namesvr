
#include "../client/name_service.h"
#include <iostream>

using namespace std;
using namespace namesvc;

int main(){
    namesvc_config_t nc;
    nc.server = "127.0.0.1:1888";
    if (namesvc_init(nc)){
        cerr << "namesvc init error !" << endl;
        return -1;
    }
#if 0
#endif
    namesvc::namesvc_register("hello", 1, 0, [](int ret){
        cout << "register hello result:"<< ret << endl;
    });
    namesvc::namesvc_register("hello", 1, 0, [](int ret){
        cout << "register hello result:" << ret << endl;
    });
    namesvc::namesvc_exists("hello", 0, [](int ret, bool exists){
        cout << "exists hello result:" << ret <<" exists:"<< exists << endl;
    });
    namesvc::namesvc_exists("hello2", 0, [](int ret, bool exists){
        cout << "exists hello2 result:" << ret << " exists:" << exists << endl;
    });
    namesvc::namesvc_register("hello2", 1, 0, [](int ret){
        cout << "register hello2 result:" << ret << endl;
    });
    namesvc::namesvc_exists("hello2", 0, [](int ret, bool exists){
        cout << "exists hello2 result:" << ret << " exists:" << exists << endl;
    });
    while (true){
        namesvc_update();
    }

    return 0;
}