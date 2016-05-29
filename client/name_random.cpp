#include "dcpots/base/stdinc.h"
#include "dcpots/base/logger.h"
#include "name_random.pb.h"
#include "name_random.h"

NS_BEGIN(namesvc)

#define CLASS_RANDNAME_MAX_NUM 1024
#define MAX_NAME_LEN 25     //21长度, 3个字节一个汉字, 7个汉字
#define MAX_NAME_CLASS_NUM  4
struct NameClass {
    int       num;
    char      names[CLASS_RANDNAME_MAX_NUM][MAX_NAME_LEN];
};

struct NameRandom {
    NameClass random_name_classes[MAX_NAME_CLASS_NUM];
} ;

struct name_random_t {
    TBRandNameDesc namelib;
    NameRandom random_name_lib[EN_RANDNAMETYPE_MAX]; //random lib
    int reload(const char * file){
        memset(&random_name_lib, 0, sizeof(random_name_lib));
        namelib.Clear();
        //#warning "todo read file"
        //namelib read from file
        for (int i = 0; i < namelib.list_size(); i++){
            const RandNameDesc & rRandNameDesc = namelib.list(i);
            RandNameType random_type = rRandNameDesc.randtype();
            if (random_type <= EN_RANDNAMETYPE_MIN || random_type >= EN_RANDNAMETYPE_MAX){
                GLOG_ERR("Invalid RandNameType:%d Info:%s", random_type, rRandNameDesc.ShortDebugString().c_str());
                return -1;
            }
            NameRandom & random_name = random_name_lib[random_type - 1];
            for (int j = 0; j < rRandNameDesc.name_size(); ++j){
                if (!rRandNameDesc.name(j).empty()){
                    int & names_num = random_name.random_name_classes[j].num;
                    if (names_num >= CLASS_RANDNAME_MAX_NUM){
                        GLOG_ERR("Invalid Random Num class idx:%d Max random class:%d Info:%s too much !",
                            j, CLASS_RANDNAME_MAX_NUM, rRandNameDesc.ShortDebugString().c_str());
                        return -2;
                    }
                    strncpy(random_name.random_name_classes[j].names[names_num], rRandNameDesc.name(j).c_str(), MAX_NAME_LEN - 1);
                    names_num++;
                }
            }
            for (int i = EN_RANDNAMETYPE_MIN + 1; i < EN_RANDNAMETYPE_MAX; i++){
                NameRandom * pstRandName = &(random_name_lib[i - 1]);
                for (int j = 0; j < MAX_NAME_CLASS_NUM - 1; ++j){
                    if (pstRandName->random_name_classes[j].num <= 0){
                        GLOG_ERR("Invalid RandNameNum Type:%d class idx:%d num <= 0",
                            i, j);
                        return -3;
                    }
                }
                if (i == EN_RANDNAMETYPE_2 && pstRandName->random_name_classes[MAX_NAME_CLASS_NUM - 1].num <= 0){
                    GLOG_ERR("Invalid RandNameNum Type:%d class idx:%d num <= 0",
                        i, MAX_NAME_CLASS_NUM - 1);
                    return -4;
                }
            }
        }
        return 0;
    }
    int init(const char * file){
        return reload(file);
    }
    const char * random(std::string & namebuff, int type){
        return "hello";
    }
};

name_random_t * name_random_create(const char * file){
    auto p = new name_random_t;
    if (p->init(file)){
        delete p;
        return nullptr;
    }

    return p;
}
void            name_random_destroy(name_random_t * nr){
    if (nr){
        delete nr;
    }
}
const   char *  name_random(name_random_t * nr, std::string & namebuff, int type){
    return nr->random(namebuff, type);
}



NS_END()