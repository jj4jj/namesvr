//#include <hiredis/hiredis.h>
#include "dcpots/base/logger.h"
#include "dcpots/dcrpc/share/dcrpc.h"
#include "dcpots/dcrpc/server/dcsrpc.h"
#include "dcpots/base/cmdline_opt.h"
#include "dcpots/utility/redis/dcredis.h"
#include "dcpots/base/dcseqnum.hpp"
#include "dcpots/base/msg_proto.hpp"
#include "dcpots/base/dcutils.hpp"
#include "dcpots/utility/mysql/dcmysqlc_pool.h"

using namespace std;
//using namespace google::protobuf;
//register(type, name, id, time)

using namespace dcsutil;
using namespace dcrpc;

struct  MysqlCallBack {
    uint64_t cookie{ 0 };
    int     clientid{ 0 };
};

typedef dcsutil::mysqlclient_pool_t::command_t  command_t;
typedef dcsutil::mysqlclient_pool_t::result_t   result_t;

static void mysql_command_dispatch(void *ud, const result_t & res, const command_t & cmd);
struct NameService : public RpcService {
    RpcServer *             svr{ nullptr };
    mysqlclient_pool_t    * mysql_pool{ nullptr };
    /////////////////////////////////////////////////////////////////////////////////////////////////
public:
    NameService(RpcServer * svr_, mysqlclient_pool_t * p) : RpcService("name", true), svr(svr_), mysql_pool(){
    }
    ~NameService(){
    }
    virtual int yield(uint64_t cookie, const RpcValues & args, std::string & error, int clientid){
        //register(name, id, type)
        const string & name = args.gets(0);
        uint64_t id = args.geti(1);
        int type = args.geti(2);
        //mysql store
        command_t cmd;
        cmd.opaque = cookie;
        string strescape;
        strnprintf(cmd.sql, 512, "INSERT INTO name SET type=%d, id=%lu, name='%s', time=%u;",
            type, id, mysql_pool->mysql()->escape(strescape, name.c_str(), name.length()),
            dcsutil::time_unixtime_s());
        return mysql_pool->execute(cmd, mysql_command_dispatch, this);
    }
};
static void mysql_command_dispatch(void *ud, const result_t & res, const command_t & cmd){
    NameService * service = (NameService*)ud;
    uint64_t cookie = cmd.opaque;
    RpcValues result;
    if (res.status != 0){
        result.addi(res.status);
        result.addi(res.err_no);
        result.adds("mysql error !");
        GLOG_ERR("mysql status status:%d error:%d error:%s",
            res.status, res.err_no, res.error.c_str());
        service->resume(cookie, result, res.status, "mysql error !");
        return;
    }
    else {
        result.addi(res.affects);
        service->resume(cookie, result);
    }
}

#define NAMESVR_VERSION	("0.0.1")
int main(int argc,const char ** argv){
    cmdline_opt_t cmdline(argc, argv);

    //todo addres tobe configuration
    cmdline.parse(""
        "db:r:d:mysql database name:test;"
        "db-user:r::mysql user name:test;"
        "db-pwd:r::mysql password:123456;"
        "listen:r:l:rpc listen address (tcp):127.0.0.1:1888;"
        "daemon:n:D:daemon mode;"
        "log-dir:r::log dir:./;"
        "log-file:r::log file pattern:namesvr.log;"
        "log-level:r::log level settings (DEBUG/INFO/...):INFO;",
		NAMESVR_VERSION);

    if (cmdline.hasopt("daemon")){
        dcsutil::daemonlize();
    }

    logger_config_t logconf;
    logconf.dir = cmdline.getoptstr("log-dir");
    logconf.pattern = cmdline.getoptstr("log-file");
    logconf.lv = INT_LOG_LEVEL(cmdline.getoptstr("log-level"));
    global_logger_init(logconf);

    dcrpc::RpcServer	rpc;
    if (rpc.init(cmdline.getoptstr("listen"))){
        return -1;
    }

    mysqlclient_pool_t          mysql;
    mysqlclient_t::cnnx_conf_t  config;
    config.ip = "127.0.0.1";
    config.uname = cmdline.getoptstr("db-user");
    config.passwd = cmdline.getoptstr("db-pwd");
    config.dbname = cmdline.getoptstr("db");
    if (mysql.init(config, 2)){
        return -3;
    }

    NameService msgsvc(&rpc, &mysql);

    rpc.regis(&msgsvc);

    while (true){
        rpc.update();
        mysql.poll();
    }

    return 0;
}
