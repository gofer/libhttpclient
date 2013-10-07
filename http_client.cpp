#include <string>
#include <cstdlib>
#include <http_client.hpp>
#include <iostream>

HTTPClient::HTTPClient() {
	_ssl_flag = false;
}

HTTPClient::~HTTPClient() {
	
}

int HTTPClient::splitURL(
	const std::string *url, 
	std::string *ptcl, 
	std::string *addr, 
	std::string *file, 
	int *port
) {
	std::string *_url = new std::string(*url);
	
	std::string::size_type protocol_pos_end = _url->find_first_of("://");
	ptcl->assign( _url->substr(0, protocol_pos_end) );
	if(!((ptcl->compare("http") == 0) || (ptcl->compare("https") == 0)))
		return -1;
	
	_url->erase(0, protocol_pos_end+3);
	
	std::string::size_type addr_pos_end = _url->find_first_of("/", 0);
	addr->assign( _url->substr(0, addr_pos_end) );
	
	file->assign( _url->erase(0, addr_pos_end) );
	if(file->length() == 0) file->assign("/");
	
	std::string::size_type post_pos_begin = addr->find_last_of(":");
	if(post_pos_begin == std::string::npos) {
		*port = (ptcl->compare("https") == 0) ? DEFAULT_HTTPS_PORT : DEFAULT_HTTP_PORT;
	} else {
		*port = atoi( addr->substr(post_pos_begin+1).c_str() );
		addr->erase(post_pos_begin);
	}
	
	delete _url;
	
	return 0;
}

int HTTPClient::send_and_recive(
	std::string *addr, 
	int port, 
	std::string *mesg, 
	std::string *recv_str
) {
	const uint32_t recv_buf_size = 0x00FFFFFF;
	char* recv_buf = new char[recv_buf_size+1];
	if(_ssl_flag) {
		SSL_TCPClient *ssl_tcp_client = new SSL_TCPClient();
		ssl_tcp_client->connect(port, hostname2ipaddr(addr->c_str()).c_str());
		ssl_tcp_client->send(*mesg);
		while(true) {
			uint32_t recv_size = ssl_tcp_client->recive(recv_buf, recv_buf_size);
			
			if(recv_str->max_size() <= recv_buf_size + recv_str->length()) break;
			recv_buf[recv_size] = '\0';
			recv_str->append(recv_buf);
			
			if(recv_size == 0) break;
		}
		ssl_tcp_client->disconnect();
		delete ssl_tcp_client;
	} else {
		TCPClient *tcp_client = new TCPClient();
		tcp_client->connect(port, hostname2ipaddr(addr->c_str()).c_str());
		tcp_client->send(*mesg);
		while(true) {
			uint32_t recv_size = tcp_client->recive(recv_buf, recv_buf_size);
			if(recv_size == -1) return 1;
			std::cout << recv_size << std::endl;
			
			if(recv_str->max_size() <= recv_buf_size + recv_str->length()) break;
			recv_buf[recv_size] = '\0';
			recv_str->append(recv_buf);
			
			if(recv_size == 0) break;
		}
		tcp_client->disconnect();
		delete tcp_client;
	}
	delete recv_buf;
	
	return 0;
}

int HTTPClient::get(
	const std::string *url, 
	std::string *recv_str, 
	HTTPHeader *header
) {
	std::string *ptcl = new std::string();
	std::string *addr = new std::string();
	std::string *file = new std::string();
	int port = 0;
	
	if(splitURL(url, ptcl, addr, file, &port) < 0) return -1;
	
	if(ptcl->compare("https") == 0) _ssl_flag = true;
	
	header->insert(std::string("Host"), *addr);
	header->insert(std::string("User-Agent"), UserAgent);
	std::string *mesg = new std::string(
		"GET " + *file + " HTTP/1.1\r\n" + 
		header->to_string() + "\r\n"
	);
	std::cout << *mesg << std::endl;
	
	send_and_recive(addr, port, mesg, recv_str);
	
	delete file;
	delete addr;
	delete ptcl;
	
	return 0;
}

std::string int2str(int n) {
	char* _str = new char[0x100];
	::sprintf(_str, "%u", n);
	delete _str;
	return std::string(_str);
}

int HTTPClient::post(
	const std::string *url, 
	std::string *recv_str,
	const std::string *post, 
	HTTPHeader *header
) {
	std::string *ptcl = new std::string();
	std::string *addr = new std::string();
	std::string *file = new std::string();
	int port = 0;
	
	if(splitURL(url, ptcl, addr, file, &port) < 0) return -1;
	
	if(ptcl->compare("https") == 0) _ssl_flag = true;
	
	header->insert(std::string("Host"), *addr);
	header->insert(std::string("User-Agent"), UserAgent);
	header->insert(std::string("Content-Length"), int2str(post->length()));
	std::string *mesg = new std::string(
		"POST " + *file + " HTTP/1.1\r\n" +  
		header->to_string() + "\r\n" +
		*post
	);
	std::cout << *mesg << std::endl;
	
	send_and_recive(addr, port, mesg, recv_str);
	
	delete file;
	delete addr;
	delete ptcl;
	
	return 0;
}
