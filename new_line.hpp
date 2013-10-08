#ifndef __NEW_LINE_HPP__
#define __NEW_LINE_HPP__

#include <string>

namespace NewLineCode {
	enum {LF=1, CR=2, CRLF=3};
	const std::string strLF   = "\n";
	const std::string strCR   = "\r";
	const std::string strCRLF = "\r\n";
	
	int detect(const std::string&);
	int detect(const std::string*);
	std::string convert(const std::string&, int);
};

#endif
