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

static void mysql_command_name_register_dispatch(void *ud, const result_t & res, const command_t & cmd);
struct NameService : public RpcService {
    mysqlclient_pool_t    * mysql_pool{ nullptr };
    /////////////////////////////////////////////////////////////////////////////////////////////////
public:
    NameService(mysqlclient_pool_t * p) : RpcService("name", true), mysql_pool(){
    }
    ~NameService(){
    }
    virtual int yield(uint64_t cookie, const RpcValues & args, std::string & error, int clientid){
        //0:register(name, id, type)
        //1:exists(name, type)
        int intf = args.geti(0);
        ///////////////////////////////////////
        uint64_t id = 0;
        int type = 0;
        //mysql store
        command_t cmd;
        cmd.need_result = true;
        cmd.opaque = cookie;
        std::string strescape;
        const std::string & name = args.gets();
        string strdebug;
        switch (intf){
        case 0: //register
            if (args.length() < 3){
                GLOG_ERR("args length error ! info:%s", args.debug(strdebug));
                return -2;
            }
            id = args.geti(1);
            type = args.geti(2);
            strnprintf(cmd.sql, 256, "INSERT INTO name SET type=%d, id=%lu, name='%s', time=%u;",
                type, id, mysql_pool->mysql()->escape(strescape, name.c_str(), name.length()),
                dcsutil::time_unixtime_s());
            return mysql_pool->execute(cmd, mysql_command_name_register_dispatch, this);
        case 1: //exists
            if (args.length() < 2){
                GLOG_ERR("args length error ! info:%s", args.debug(strdebug));
                return -2;
            }
            type = args.geti(1);
            strnprintf(cmd.sql, 256, "SELECT id FROM name WHERE type=%d AND name='%s';",
                type, mysql_pool->mysql()->escape(strescape, name.c_str(), name.length()));
            return mysql_pool->execute(cmd, mysql_command_name_register_dispatch, this);
        default:
            return -1;
        }
    }
};
static inline void mysql_error_resume(NameService* service, uint64_t cookie,
    RpcValues & result, const result_t & res){
    result.addi(res.status);
    result.addi(res.err_no);
    result.adds("mysql error !");
    GLOG_ERR("mysql status status:%d error:%d error:%s",
        res.status, res.err_no, res.error.c_str());
    service->resume(cookie, result, res.status, "mysql error !");
}
static void mysql_command_name_exists_dispatch(void *ud, const result_t & res,
    const command_t & cmd){
    NameService * service = (NameService*)ud;
    uint64_t cookie = cmd.opaque;
    RpcValues result;
    if (res.status != 0){
        return mysql_error_resume(service, cookie, result, res);
    }
    else {
        result.addi(res.fetched_results.size());
        service->resume(cookie, result);
    }
}
static void mysql_command_name_register_dispatch(void *ud, const result_t & res,
    const command_t & cmd){
    NameService * service = (NameService*)ud;
    uint64_t cookie = cmd.opaque;
    RpcValues result;
    if (res.status != 0){
        return mysql_error_resume(service, cookie, result, res);
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
        NameService            *nsc{ nullptr };
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
            /////////////////////////////////////////////////////////////
            rpc.regis(new NameService(&mysql));
			/////////////////////////////////////////////////////////////            
            
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
