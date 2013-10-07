g++ -c http_client.cpp http_header.cpp -std=c++11 \
	-I. -I../libtcpclient -L../libtcpclient \
	-lssl -lcrypto -ltcpclient -lpthread -lstdc++
ar rsv libhttpclient.a http_client.o http_header.o
rm -f http_client.o > /dev/null
rm -f http_header.o > /dev/null
