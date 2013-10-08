#!/bin/sh

rm -f libhttpclient.a > /dev/null
sh compile.sh

g++ http_client_test.cpp -std=c++11 -o my_test.out \
	-I. -L. -lhttpclient \
	-I../libtcpclient -L../libtcpclient -ltcpclient\
	-lssl -lcrypto -lstdc++
./my_test.out
