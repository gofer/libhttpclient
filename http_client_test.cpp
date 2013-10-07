#include <iostream>
#include <string>
#include <http_client.hpp>

void header_test1() {
	HTTPHeader httpHeader1;
	httpHeader1.insert("hoge", "foobar");
	httpHeader1.insert("oops!", "hogehoge");
	std::cout << httpHeader1.to_string() << std::endl;
}

void header_test2() {
	HTTPHeader *httpHeader2 = new HTTPHeader();
	httpHeader2->insert("hoge", "foobar");
	httpHeader2->insert("oops!", "hogehoge");
	std::string *str = new std::string();
	httpHeader2->to_string(str);
	std::cout << *str << std::endl;
	delete str;
	delete httpHeader2;
}

void http_access_test() {
	HTTPClient* httpClient = new HTTPClient();
	HTTPHeader* httpHeader = new HTTPHeader();
	
	httpHeader->insert(
		std::string("Content-Type"), 
		std::string("application/x-www-form-urlencoded")
	);
	httpHeader->insert(
		std::string("Connection"), 
		std::string("close")
	);
	std::string* recv_str = new std::string();
	httpClient->get(
		new std::string("http://www.ex-studio.info/"), 
		recv_str, 
		httpHeader
	);
	std::cout << *recv_str << std::endl;
	
	HTTPHeader* recvHeader = new HTTPHeader();
	recvHeader->set(*recv_str);
	//recvHeader->print();
	std::cout << "Date is " << recvHeader->get("Date") << std::endl;
	delete recvHeader;
	
	delete recv_str;
	
	delete httpHeader;
	delete httpClient;
}

int main() {
	header_test1();
	
	header_test2();
	
	http_access_test();
	
	return 0;
}
