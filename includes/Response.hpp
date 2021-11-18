#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "MainIncludes.hpp"

class Response
{
private:
	std::string					_statusLine;
	std::map <string, string>	_headers;
	std::string					_body; 						// TODO make body class for chunks
	std::uint64_t				_bodySize;
	uint32_t					_statusCode;
	string						_contentType;
public:
	void						makeStatusLine(std::string comment);
	void						makeHeaders();
	void						setStatusCode(int);
	std::string					send();


	Response();
};


#endif