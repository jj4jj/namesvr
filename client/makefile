
libnamesvc.a: name_service.cpp name_random.cpp name_random.pb.cc
	g++ --std=c++11 -c $^ -I../../
	ar -rcs $@ *.o

name_random.pb.cc: name_random.proto
	protoc name_random.proto --cpp_out=.

clean:
	rm -f *.o
