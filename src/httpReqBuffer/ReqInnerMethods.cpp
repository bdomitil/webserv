#include "../../includes/MainIncludes.hpp"

bool	Request::isStringHasWhiteSpaceChar(std::string &str) const {
	for(std::size_t i = 0; i < str.length(); i++)
		if (std::isspace(str[i]) == 1)
			return true;
	return false;
}

void	Request::saveStartLine(std::string startLine) {
	std::size_t	lfPos;

	if (!startLine.length())
		throw std::exception();

//	save method
	lfPos = startLine.find(' ');
	if (lfPos == std::string::npos)
		throw std::exception();
	_method = startLine.substr(0, lfPos);
	if (_method != GET or _method != POST or _method != DELELE)
		throw std::exception();

//	save target
	lfPos = startLine.find(' ');
	if (lfPos == std::string::npos)
		throw std::exception();
	_target = startLine.substr(1, lfPos);

//	save HTTP-protocol
	lfPos = startLine.find(' ');
	if (lfPos == std::string::npos)
		throw std::exception();
	_protocol = startLine.substr(1, lfPos);
	if (_protocol != HTTP_PROTOCOL)
		throw std::exception();

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
		throw std::exception();
	headerName = headerLine.substr(0, colonPos);
	headerValue = headerLine.substr(colonPos + 1);
	if (isStringHasWhiteSpaceChar(headerName))
		throw std::exception();
	if (headerValue[0] == ' ')
		headerValue.erase(0, 1);
	_headers.insert(std::pair<string, string>(headerName, headerValue));
	return;
}

void	Request::saveBodyPart(std::string bodyLine) {
	if (bodyLine.length() + _body.length() > _maxBodySize)
		throw std::exception();
	if (!bodyLine.length())
		_parseState = END_STATE;
	_body += bodyLine + LF;
	return;
}
