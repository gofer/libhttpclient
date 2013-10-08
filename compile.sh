g++ -c http_client.cpp http_header.cpp new_line.cpp http_client_utils.cpp -std=c++11 \
	-I. -I../libtcpclient -L../libtcpclient \
	-lssl -lcrypto -ltcpclient -lpthread -lstdc++
ar rsv libhttpclient.a http_client.o http_header.o new_line.o http_client_utils.o
rm -f http_client.o       > /dev/null
rm -f http_header.o       > /dev/null
rm -f new_line.o          > /dev/null
rm -f http_client_utils.o > /dev/null
