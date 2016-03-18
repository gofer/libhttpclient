# HTTP and HTTPS client library for C++

## What's this
This is HTTP and HTTPS client library for C++.

## Attention
This code uses C++11 features.
This library needs libtcpclient.

## How to use
This library has 2 classes.
* Class HTTPClient

* Class HTTPHeader

### Examples
In detail, please show http_client_test.cpp.
This is compiled with a following command.

    g++ http_client_test.cpp -I. -L. -lhttpclient -ltcpclient -lssl -lcrypto -lstdc++ -std=c++11 -o test.out


## Licence
These codes are licensed by New BSD License.
