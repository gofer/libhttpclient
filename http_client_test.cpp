#include <iostream>
#include <string>
#include <http_client.hpp>

void header_test1() {
	HTTPHeader httpHeader1;
	httpHeader1.insert("hoge", "foobar");
	httpHeader1.insert("oops!", "hogehoge");
	std::cout << httpHeader1.to_string() << std::endl;
	return;
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
	return;
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
	
	httpClient->get(
		new std::string("http://www.ex-studio.info/"), 
		httpHeader
	);
		
	int status_code = httpClient->status_code();
	std::cout << "Status code: " << status_code << std::endl;
	if(status_code != 200) {
		delete httpHeader;
		delete httpClient;
		return;
	}
	
	HTTPHeader* recvHeader = httpClient->header();
	std::string* recvBody = httpClient->body();
	std::cout << "Date is " << recvHeader->get("Date") << std::endl;
	std::cout << "body: " << *recvBody << std::endl;
	
	delete httpHeader;
	delete httpClient;
	return;
}

int main() {
	header_test1();
	
	header_test2();
	
	http_access_test();
	
	return 0;
}
