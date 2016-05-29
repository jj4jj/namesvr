#!/bin/bash

protoc="protoc"
protol="-lprotobuf"
protoi="/usr/include"
if [[ "x$1" = "xu1" ]];then
	protoc='../../protobuf/bin/protoc'
	protoi='../../protobuf/include/'
	protol='../../protobuf/lib/libprotobuf.a'
        evl="../../libev/lib/libev.a"
fi

cd client && make install protoc="$protoc" protoi="$protoi" protol="$protol"
cd -
cd namesvr && make install protoc="$protoc" protoi="$protoi" protol="$protol" evl="${evl}"
cd -
cd test && make install protoc="$protoc" protoi="$protoi" protol="$protol"

