#include <string>
#include <map>
#include <http_client.hpp>
#include <cstdlib>
#include <iostream>

HTTPHeader::HTTPHeader() {
	_header = new std::map<std::string, std::string>();
}

HTTPHeader::~HTTPHeader() {
	delete _header;
}

int HTTPHeader::insert(const std::string key, const std::string value) {
	if(key.length() == 0 || value.length() == 0) return 1;
	if(_header->find(key) != _header->end()) return 1;
	_header->insert(HTTPHeaderLine(key, value));
	return 0;
}

int HTTPHeader::insert(const HTTPHeaderLine line) {
	if(line.first.length() == 0 || line.second.length() == 0) return 1;
	if(_header->find(line.first) != _header->end()) return 1;
	_header->insert(line);
	return 0;
}

int HTTPHeader::erase(const std::string key) {
	if(key.length() == 0) return 1;
	if(_header->find(key) != _header->end()) return 1;
	_header->erase(key);
	return 0;
}

int HTTPHeader::modify(const std::string key, const std::string value) {
	if(key.length() == 0 || value.length() == 0) return 1;
	std::map<std::string, std::string>::iterator itr = _header->find(key);
	if(itr != _header->end()) return 1;
	_header->erase(itr);
	_header->insert(itr, HTTPHeaderLine(key, value));
	return 0;
}

int HTTPHeader::modify(const HTTPHeaderLine line) {
	if(line.first.length() == 0 || line.second.length() == 0) return 1;
	std::map<std::string, std::string>::iterator itr = _header->find(line.first);
	if(itr != _header->end()) return 1;
	_header->erase(itr);
	_header->insert(itr, line);
	return 0;
}

std::string HTTPHeader::to_string() {
	std::string dst;
	std::map<std::string, std::string>::iterator itr = _header->begin();
	const std::map<std::string, std::string>::iterator end = _header->end();
	for(; itr != end; ++itr)
		{dst += itr->first + _delimiter + itr->second + _cr_lf;}
	return dst;
}

int HTTPHeader::to_string(std::string* dst) {
	std::map<std::string, std::string>::iterator itr = _header->begin();
	const std::map<std::string, std::string>::iterator end = _header->end();
	for(; itr != end; ++itr)
		{dst->append(itr->first + _delimiter + itr->second + _cr_lf);}
	return 0;
}

std::string HTTPHeader::get(const std::string& key) {
	std::map<std::string, std::string>::iterator itr = _header->find(key);
	if(itr == _header->end()) return std::string("");
	return itr->second;
}

int HTTPHeader::get(std::string* key, std::string* val) {
	std::map<std::string, std::string>::iterator itr = _header->find(*key);
	if(itr == _header->end()) val->assign(std::string(""));
	val->assign(itr->second);
	return 0;
}

inline uint32_t min(uint32_t a, uint32_t b) {return (a<b)?a:b;}
inline uint32_t min(uint32_t a, uint32_t b, uint32_t c) {return min(min(a, b), c);}

namespace NewLineCode {
	enum {LF=1, CR=2, CRLF=3};
	const std::string strLF   = "\n";
	const std::string strCR   = "\r";
	const std::string strCRLF = "\r\n";
	
	int detect(const std::string  str) {
		if(str.find(strCR)  == std::string::npos) return LF; else
		if(str.find(strLF)  == std::string::npos) return CR; else
		return CRLF;
	}
	int detect(const std::string *str) {
		if(str->find(strCR) == std::string::npos) return LF; else
		if(str->find(strLF) == std::string::npos) return CR; else
		return CRLF;
	}
	std::string convert(std::string src, int code) {
		int src_code = detect(src);
		if(src_code == code) return std::string(src);
		
		std::string dst;
		switch(src_code) {
		case LF: {
				switch(code) {
				case CR : {
						for(std::string::iterator itr=src.begin(); itr!=src.end(); ++itr) {
							if(*itr == '\n')	dst.append(1, '\r');
							else				dst.append(1, *itr);
						}
					} break;
				case CRLF : {
						for(std::string::iterator itr=src.begin(); itr!=src.end(); ++itr) {
							if(*itr == '\n')	dst.append("\r\n", 2);
							else				dst.append(1, *itr);
						}
					} break;
				}
			} break;
		case CR: {
				switch(code) {
				case LF : {
						for(std::string::iterator itr=src.begin(); itr!=src.end(); ++itr) {
							if(*itr == '\r')	dst.append(1, '\n');
							else				dst.append(1, *itr);
						}
					} break;
				case CRLF : {
						for(std::string::iterator itr=src.begin(); itr!=src.end(); ++itr) {
							if(*itr == '\r')	dst.append("\r\n", 2);
							else				dst.append(1, *itr);
						}
					} break;
				}
			} break;
		case CRLF: {
				switch(code) {
				case LF : {
						for(std::string::iterator itr=src.begin(); itr!=src.end(); ++itr) {
							if(*itr != '\r') dst.append(1, *itr);
						}
					} break;
				case CR : {
						for(std::string::iterator itr=src.begin(); itr!=src.end(); ++itr) {
							if(*itr != '\n') dst.append(1, *itr);
						}
					} break;
				}
			} break;
		}
		
		return dst;
	}
};

std::map<std::string, std::string> HTTPHeader::str_split(std::string& src) {
	std::map<std::string, std::string> hash;
	
	uint64_t i=0, j=0, len=src.size();
	while(i < len) {
		std::string key, val;
		for(j=i; j<len; ++j) {if(src[j] == ':') break;}
		key.assign(src.substr(i, j-i));
		i = j+1;
		
		for(j=i; j<len; ++j) {if(src[j] == ' ' || src[j] == '\t') break;}
		i = j+1;
		
		for(j=i; j<len; ++j) {if(src[j] == '\n') break;}
		val.assign(src.substr(i, j-i));
		i = j+1;
		
		hash.insert(std::pair<std::string, std::string>(key, val));
	}
	
	return hash;
}

int HTTPHeader::set(std::string src) {
	uint32_t len = src.find_first_of("\n");
	if(len == std::string::npos) return 1;
	src = NewLineCode::convert(src, NewLineCode::LF); 
	src = src.erase(0, len);
	
	len = src.find("\n\n", 0);
	if(len == std::string::npos) return 1;
	src = src.erase(len);
	
	std::map<std::string, std::string> hash = str_split(src);
	_header->swap(hash);
	return 0;
}
/*
void HTTPHeader::print() {
	for(std::map<std::string, std::string>::iterator itr=_header->begin(); itr!=_header->end(); ++itr) {
		std::cout << '\"' << itr->first << "\" : \"" << itr->second << '\"' << std::endl;
	}
}
*/