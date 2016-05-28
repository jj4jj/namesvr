//#include <hiredis/hiredis.h>
#include "dcpots/base/logger.h"
#include "dcpots/dcrpc/share/dcrpc.h"
#include "dcpots/dcrpc/server/dcsrpc.h"
#include "dcpots/base/cmdline_opt.h"
#include "dcpots/utility/redis/dcredis.h"
#include "dcpots/base/dcseqnum.hpp"
#include "dcpots/base/msg_proto.hpp"
#include "dcpots/base/dcutils.hpp"
#include "dcpots/base/app.hpp"
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
	struct NameServer : dcsutil::App {
		dcrpc::RpcServer		rpc;
		mysqlclient_pool_t		mysql;
	public:		
		NameServer():dcsutil::App(NAMESVR_VERSION){
		}
		std::string options(){
			return ""
				"db:r:d:mysql database name:test;"
				"db-user:r::mysql user name:test;"
				"db-pwd:r::mysql password:123456;"
				"db-host:r::mysql connection host:127.0.0.1;"
				"db-thread:r::mysql connection thread num:2;"
				"listen:r:l:rpc listen address (tcp):127.0.0.1:1888;"
				"";
		}
		int on_init(const char * sconfig){
			const char * listen = cmdopt().getoptstr("listen");
			if (rpc.init(listen)){
				return -1;
			}
			mysqlclient_t::cnnx_conf_t  config;
			config.dbname = cmdopt().getoptstr("db");
			config.ip = cmdopt().getoptstr("db-host");
			config.uname = cmdopt().getoptstr("db-user");
			config.passwd = cmdopt().getoptstr("db-pwd");
			if (mysql.init(config, cmdopt().getoptint("db-thread"))){
				return -2;
			}

			return 0;
		}
		int on_loop(){
			rpc.update();
			mysql.poll();
		}
	};	
	NameServer ns;
	if (ns.init(argc, argv)){
		GLOG_ERR("nameserver init error !");
		return -1;
	}
	return ns.run();
}
