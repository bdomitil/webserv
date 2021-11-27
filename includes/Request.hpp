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



class Request {

public:

	Request(std::map<std::string, Location> const &);
	~Request();

	std::string	getUrl(std::uint32_t &) const ;
	std::string	getMethod() const ;
	char		*getBuffer() const ;

	bool		saveRequestData(ssize_t);
	void		showState() const ;

private:

	int		getLimitBodySize() const ;

	bool	isStringHasWhiteSpaceChar(std::string const &) const ;
	void	saveStartLineHeaders(std::string &);
	void	saveSimpleBody(std::string &);
	void	saveStartLine(std::string);
	void	saveHeaderLine(std::string);
	void	saveBodyPart(std::string);
	void	saveChunkedBody(std::string);

	std::map<std::string, std::string>		_headers;
	std::map<std::string, Location> const	&_locationsMap;
	std::uint32_t							_maxBodySize;
	std::uint32_t							_bodySize;
	std::uint8_t							_parseState;
	std::string								_transferEncoding;
	std::string								_method;
	std::string								_protocol;
	std::string								_uri;
	std::string								_body;
	std::string								_tmpBuffer;
	bool									_isReqDone;
	char									*_buffer;

};

#endif
