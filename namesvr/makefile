namesvr: namesvr.cpp
	g++ -o $@ $^ ../../dcpots/lib/libdcrpc.a ../../dcpots/lib/libdcbase.a ../../dcpots/lib/libdcutil-mysql.a ${protol} -I../.. --std=c++11 ${evl} -pthread -lmysqlclient -ldl

clean:
	rm -f namesvr

install: namesvr
	mkdir ../bin/ -p
	mv namesvr ../bin/



