#include "../../includes/MainIncludes.hpp"

bool	Request::isStringHasWhiteSpaceChar(std::string const &str) const {
	for(std::size_t i = 0; i < str.length(); i++)
		if (std::isspace(str[i]) == 1)
			return true;
	return false;
}

void	Request::saveStartLine(std::string startLine) {
	std::size_t	lfPos;

	if (!startLine.length())
		throw ErrorException(400, "Bad request");

//	save method
	lfPos = startLine.find(' ');
	if (lfPos == std::string::npos)
		throw ErrorException(400, "Bad request");
	_method = startLine.substr(0, lfPos);
	if (_method != GET and _method != POST and _method != DELELE)
		throw ErrorException(405, "Method Not Allowed");
	startLine.erase(0, lfPos + 1);

//	save target
	lfPos = startLine.find(' ');
	if (lfPos == std::string::npos)
		throw ErrorException(400, "Bad request");
	_uri = startLine.substr(0, lfPos);
	startLine.erase(0, lfPos + 1);

//	save HTTP-protocol
	_protocol = startLine;
	if (_protocol != HTTP_PROTOCOL)
		throw ErrorException(505, "HTTP Version Not Supported");

	_parseState = HEADER_LINE;
	return;
}

void	Request::saveHeaderLine(std::string headerLine) {
	std::size_t	colonPos;
	std::string	headerName;
	std::string	headerValue;

	if (!headerLine.length()) {
		if (_headers.find("Host") == std::end(_headers))
			throw std::exception();
		if (_headers.find("Transfer-Encoding") == std::end(_headers)
			and _headers.find("Content-Length") == std::end(_headers))
			_parseState = END_STATE;
		else
			_parseState = BODY_LINE;
		return;
	}

	colonPos = headerLine.find(":");
	if (colonPos == std::string::npos)
		throw ErrorException(400, "Bad request");
	headerName = headerLine.substr(0, colonPos);
	headerValue = headerLine.substr(colonPos + 1);
	if (isStringHasWhiteSpaceChar(headerName))
		throw ErrorException(400, "Bad request");
	if (headerValue[0] == ' ')
		headerValue.erase(0, 1);
	_headers.insert(std::pair<std::string, std::string>
		(headerName, headerValue));
	return;
}

void	Request::saveBodyPart(std::string bodyLine) {
	if (bodyLine.length() + _body.length() > _maxBodySize)
		throw ErrorException(413, "Request Entity Too Large");
	if (!bodyLine.length())
		_parseState = END_STATE;
	_body += bodyLine + LF;
	return;
}
