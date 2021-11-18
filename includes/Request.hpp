#ifndef WEBSERV_UEST_HPP
# define WEBSERV_UEST_HPP

/*
	parse states
*/
# define START_LINE		0
# define HEADER_LINE	1
# define BODY_LINE		2
# define END_STATE		3

/*
	default http request methods
	and HTTP-protocol version
*/
# define GET			"GET"
# define POST			"POST"
# define DELELE			"DELETE"
# define HTTP_PROTOCOL	"HTTP/1.1"

/*
	end of start line
	or header fields
	or request at all
*/
# define CR				"\r"
# define LF				"\n"

class Request {

public:

	Request(std::uint32_t,
		std::map<std::string, Location> &);
	~Request();

	bool	saveRequestData(ssize_t);
	void	showState() const ;
	char	*getBuffer() const ;

private:

	bool	isStringHasWhiteSpaceChar(std::string &) const ;
	void	saveStartLine(std::string);
	void	saveHeaderLine(std::string);
	void	saveBodyPart(std::string);

	std::map<std::string, std::string>	_headers;
	std::map<std::string, Location>		&_locationsMap;
	std::uint32_t						_maxBodySize;
	std::uint8_t						_parseState;
	std::string							_method;
	std::string							_protocol;
	std::string							_target;
	std::string							_body;
	std::string							_tmpBuffer;
	bool								_isReqDone;
	char								*_buffer;

};

#endif
