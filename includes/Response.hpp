#ifndef RESPONSE_HPP
#define RESPONSE_HPP
#include "MainIncludes.hpp"

class Response
{
private:
	std::string							_statusLine;
	std::map <std::string, std::string>	_reqHeaders;
	std::map<int, std::string>			_errorPages;
	char*						 		_body;
	std::uint64_t						_bodySize;
	uint32_t							_statusCode;
	std::ifstream						_FILE;
	std::string							_url;
	std::string							_headers;
	std::string							_contentType;
	std::string							_response;
	bool								_inProc;
	long long int						_leftBytes;
	bool								_autoindex;
	const Location						*_reqLocation;
	Cgi									*_cgiPtr;
	int									_cgiFd[2];

public:
	std::string					getResponse() const ;
	std::string					makeStatusLine();
	std::string					makeHeaders();
	std::string					getErrorPage();
	char						*makeBody(int &readSize);
	void						setStatusCode(int);
	void						sendRes(int Socket);
	bool						isSent(){return (!_leftBytes);}
	char					*filesListing(std::string const &path);
	Response(Request &request, std::map<int, std::string> errorPages);
	~Response();
};

#endif
