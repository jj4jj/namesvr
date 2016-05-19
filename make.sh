#!/bin/bash

if [[ $1 = "" ]];then
	cd client && make protoc=protoc protoi='/usr/include/' protol='-lprotobuf' && make install
else
	cd client && make protoc='../../protobuf/bin/protoc' protoi='../../protobuf/include/' && make install
fi
cd -
cd namesvr && make protol='../../protobuf/lib/libprotobuf.a' && make install

