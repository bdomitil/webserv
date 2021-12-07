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

	Request(std::multimap<std::string, Location> const &);
	~Request();

	const Location						*getLocation() const ;
	std::string							getUrl(std::uint32_t &);
	std::map<std::string, std::string>	&getHeaders();
	std::map<int, std::string>			&getErrorPages();
	std::string							getMethod() const ;
	std::string							getBody();
	std::string							getQueryString();
	void								setErrorStatus(int const);
	char								*getBuffer() const ;
	uint32_t							getErrorStatus() const ;
	void								getUrlEncodedBody(std::map<std::string, std::string> &);

	bool								saveRequestData(ssize_t);
	void								showState() const ;
	void								resetRequest();

private:

	const Location	*getLoc();

	std::uint32_t	checkPath(std::string &) const ;
	std::string		validateUrl(std::string &, std::uint32_t &, std::uint8_t);
	void			saveStartLineHeaders(std::string &);
	void			saveSimpleBody(std::string &);
	void			saveChunkedBody(std::string &);
	void			saveStartLine(std::string);
	void			saveHeaderLine(std::string);
	void			parseUri();
	void			parsePercent(std::string &);
	void			parseChunkSize(std::string &);
	void			parseChunkedBody(std::string &);
	void			validateStartLine();

	std::map<std::string, std::string>			_headers;
	std::multimap<std::string, Location> const	&_locationsMap;
	const Location								*_location;
	std::uint32_t								_maxBodySize;
	std::uint32_t								_bodySize;
	std::uint32_t								_chunkSize;
	std::uint8_t								_parseState;
	std::string									_transferEncoding;
	std::string									_method;
	std::string									_protocol;
	std::string									_uri;
	std::string									_query;
	std::string									_body;
	std::string									_tmpBuffer;
	bool										_isReqDone;
	bool										_isChunkSize;
	char										*_buffer;
	uint32_t									_errorStatus;

};

#endif
