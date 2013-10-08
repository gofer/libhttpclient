#include <new_line.hpp>

namespace NewLineCode {
	int detect(const std::string& str) {
		if(str.find(strCR)  == std::string::npos) return LF; else
		if(str.find(strLF)  == std::string::npos) return CR; else
		return CRLF;
	}
	
	int detect(const std::string* str) {
		if(str->find(strCR) == std::string::npos) return LF; else
		if(str->find(strLF) == std::string::npos) return CR; else
		return CRLF;
	}
	
	std::string replace(
		const std::string& src, 
		const std::string& tar, 
		const std::string& res
	) {
		std::string::size_type i = 0, j = 0;
		const std::string::size_type Ls = src.length();
		const std::string::size_type Lt = tar.length();
		std::string dst;
		while(i < Ls) {
			j = src.find(tar, i);
			if(j == std::string::npos) break;
			dst.append(src.substr(i, j-i));
			dst.append(res);
			i = j + Lt;
		}
		dst.append(src.substr(i));
		return dst;
	}
	
	std::string convert(const std::string& src, int code) {
		int src_code = detect(src);
		if(src_code == code) return std::string(src);
		
		switch(src_code) {
		case LF: {
				switch(code) {
				case CR :	return replace(src, "\n", "\r");
				case CRLF :	return replace(src, "\n", "\r\n");
				}
			} break;
		case CR: {
				switch(code) {
				case LF :	return replace(src, "\r", "\n");
				case CRLF :	return replace(src, "\r", "\r\n");
				}
			} break;
		case CRLF: {
				switch(code) {
				case LF :	return replace(src, "\r\n", "\n");
				case CR :	return replace(src, "\r\n", "\r");
				}
			} break;
		}
		
		return std::string();
	}
};
