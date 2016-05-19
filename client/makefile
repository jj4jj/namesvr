
libnamesvc.a: name_service.cpp name_random.cpp name_random.pb.cc
	g++ --std=c++11 -c $^ -I../../ -I${protoi}
	ar -rcs $@ *.o

name_random.pb.cc: name_random.proto
	${protoc} name_random.proto --cpp_out=.

clean:
	rm -f *.o *.pb.cc *.pb.h

install: libnamesvc.a
	mkdir ../lib/ -p
	mv libnamesvc.a ../lib/
