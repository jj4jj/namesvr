namesvr: namesvr.cpp
	g++ -o $@ $^ ../../dcpots/lib/libdcrpc.a ../../dcpots/lib/libdcbase.a ../../dcpots/lib/libdcutil-mysql.a ${protol} -I../.. --std=c++11 -lev -pthread -lmysqlclient

clean:
	rm -f namesvr

install:
	mkdir ../bin/ -p
	mv namesvr ../bin/



