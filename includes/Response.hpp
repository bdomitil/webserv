#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "MainIncludes.hpp"

class Response
{
private:
	std::string					_statusLine;
	std::map <std::string, std::string>	_headers;
	char*						 _body; 						// TODO make body class for chunks
	std::uint64_t				_bodySize;
	uint32_t					_statusCode;
	std::ifstream					_FILE;
	std::string					_url;
	std::string					_tmpHead;
	std::string					_contentType;
	std::string					_response;
	bool						_inProc;
	long long int				_leftBytes;

public:
	std::string					getResponse() const ;
	std::string					makeStatusLine();
	std::string					makeHeaders();
	char						*makeBody(int &readSize);
	void						setStatusCode(int);
	void						sendRes(int Socket);
	bool						isSent(){return (!_leftBytes);}
	Response(Request &request);
};

#endif
