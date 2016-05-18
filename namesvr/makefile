namesvr: namesvr.cpp
	g++ -g -o $@ $^ ../../dcpots/lib/libdcrpc.a  -lprotobuf ../../dcpots/lib/libdcbase.a ../../dcpots/lib/libdcutil-mysql.a -I../.. --std=c++11 /usr/lib/x86_64-linux-gnu/libev.a -pthread -lmysqlclient

clean:
	rm -f namesvr test
