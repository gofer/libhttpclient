#include <string>
#include <http_client.hpp>
#include <http_client_utils.hpp>
#include <iostream>

HTTPClient::HTTPClient() {
	_ssl_flag = false;
	recv_status = new std::string();
	recv_header = new HTTPHeader();
	recv_body = new std::string();
}

HTTPClient::~HTTPClient() {
	delete recv_body;
	delete recv_header;
	delete recv_status;
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

int HTTPClient::split_header_body(std::string* _src) {
	std::string src = NewLineCode::convert(*_src, NewLineCode::LF);
	std::string::size_type p = src.find("\n");
	
	recv_status->assign( src.substr(0, p) );
	
	std::string::size_type q = src.find("\n\n", p+1);
	
	std::string* _header = new std::string();
	_header->assign( src.substr(p+1, q-p-1) );
	recv_header->set(*_header);
	delete _header;
	
	recv_body->assign( src.substr(q+2) );
	return 0;
}

int HTTPClient::get(
	const std::string *url, 
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
	//std::cout << *mesg << std::endl;
	
	std::string *recv_str = new std::string();
	send_and_recive(addr, port, mesg, recv_str);
	split_header_body(recv_str);
	delete recv_str;
	
	delete file;
	delete addr;
	delete ptcl;
	
	return 0;
}

int HTTPClient::post(
	const std::string *url, 
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
	//std::cout << *mesg << std::endl;
	
	std::string *recv_str = new std::string();
	send_and_recive(addr, port, mesg, recv_str);
	split_header_body(recv_str);
	delete recv_str;
	
	delete file;
	delete addr;
	delete ptcl;
	
	return 0;
}

int HTTPClient::status_code() {
	std::string::size_type p = recv_status->find(' ')+1;
	std::string::size_type q = recv_status->find(' ', p+1);
	return str2int( recv_status->substr(p, q-p) );
}
std::string* HTTPClient::status() {return recv_status;}
HTTPHeader* HTTPClient::header()  {return recv_header;}
std::string* HTTPClient::body()   {return recv_body;}
