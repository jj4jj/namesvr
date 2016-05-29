install: nametest
	mv nametest ../bin/

nametest: ./name_test.cpp ../lib/libnamesvc.a ../../dcpots/lib/libdcrpc.a ../../dcpots/lib/libdcbase.a
	g++ -o $@ $^ --std=c++11 ${protol} -ldl
