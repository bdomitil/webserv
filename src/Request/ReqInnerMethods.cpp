#include "../../includes/MainIncludes.hpp"

bool	Request::isStringHasWhiteSpaceChar(std::string const &str) const {

	for(std::size_t i = 0; i < str.length(); i++)
		if (std::isspace(str[i]) != 0)
			return true;
	return false;
}

std::size_t	Request::skipWhiteSpaces(std::string const &str, std::size_t start = 0) const {

	if (start >= str.length())
		return str.length();
	while (start < str.length() and ikael::isCharWhiteSpace(str[start]))
		start++;
	return start;
}

const Location	*Request::getLoc(void) const {

	std::string	tmp;
	std::string	tmp1;
	std::size_t	lastSlashPos;
	std::size_t	len;

	lastSlashPos = _uri.find_last_of("/");
	if (lastSlashPos == std::string::npos)
		throw ErrorException(400, "Bad request");

	tmp = _uri.substr(0, lastSlashPos);
	len = std::count(_uri.begin(), _uri.end(), '/');
	for (std::size_t i = 0; i < len; i++) {
		std::multimap<std::string, Location>::const_iterator j = _locationsMap.begin();
		for (; j != _locationsMap.end(); j++) {
			(!tmp.length()) ? tmp = "/" : tmp = tmp;
			(j->first != "/" and j->first[j->first.length() - 1] == '/') ?
				tmp1 = j->first.substr(0, j->first.find_last_of("/")) : tmp1 = j->first;
			if (tmp == tmp1)
				return &j->second;
		}
		lastSlashPos = tmp.find_last_of("/", lastSlashPos);
		tmp = tmp.substr(0, lastSlashPos);
	}
	return nullptr;
}

void	Request::validateStartLine(void) {

	_location = getLoc();
	if (!_location)
		throw ErrorException(404, "Not Found");

	std::map<std::string, bool>::const_iterator i = _location->methods.begin();
	for (; i != _location->methods.end(); i++) {
		if (i->first == _method) {
			if (!i->second)
				throw ErrorException(405, "Method Not Allowed");
			break;
		}
	}
	if (i == _location->methods.end())
		throw ErrorException(404, "Bad Request");
	if (_protocol != HTTP_PROTOCOL)
		throw ErrorException(505, "Http Version Not Supported");
	_maxBodySize = _location->getLimit();
	parseUri();
}

void	Request::saveStartLine(std::string startLine) {

	std::size_t	lfPos;
	std::size_t	i;

	if (!startLine.length())
		throw ErrorException(400, "Bad request");
	lfPos = startLine.find(' ');
	if (lfPos == std::string::npos)
		throw ErrorException(400, "Bad request");
	_method = startLine.substr(0, lfPos);
	startLine.erase(0, skipWhiteSpaces(startLine, lfPos));

	lfPos = startLine.find(' ');
	if (lfPos == std::string::npos)
		throw ErrorException(400, "Bad request");
	_uri = startLine.substr(0, lfPos);
	startLine.erase(0, skipWhiteSpaces(startLine, lfPos));

	_protocol = startLine;
	_protocol.erase(std::remove_if(_protocol.begin(),
		_protocol.end(), &ikael::isCharWhiteSpace), _protocol.end());

	validateStartLine();
	_parseState = HEADER_LINE;
	return;
}

void	Request::saveHeaderLine(std::string headerLine) {

	std::size_t	colonPos;
	std::string	headerName;
	std::string	headerValue;

	headerLine.erase(std::remove_if(headerLine.begin(),
		headerLine.end(), &ikael::isCharWhiteSpace), headerLine.end());
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
	_headers.insert(std::pair<std::string,
		std::string>(headerName, headerValue));
	if (headerName == "Content-Length")
		_bodySize = static_cast<std::uint32_t>(std::atol(headerValue.c_str()));
	if (headerName == "Transfer-Encoding")
		_transferEncoding = headerValue;
	return;
}

void	Request::saveStartLineHeaders(std::string &data) {

	std::size_t	newLinePos;

	newLinePos = data.find(LF);
	while (newLinePos != std::string::npos
		and (_parseState != BODY_LINE and _parseState != END_STATE)) {
		if (_parseState == START_LINE) {
			saveStartLine(data.substr(0, newLinePos));
			data.erase(0, newLinePos + 1);
		}
		if (_parseState == HEADER_LINE) {
			newLinePos = data.find(LF);
			saveHeaderLine(data.substr(0, newLinePos));
			data.erase(0, newLinePos + 1);
		}
		newLinePos = data.find(LF);
	}
	return;
}

void	Request::saveSimpleBody(std::string &data) {

	std::size_t	bodySize;

	bodySize = static_cast<std::size_t>(std::atol(_headers["Content-Length"].c_str()));
	if (bodySize > _maxBodySize)
		throw ErrorException(413, "Request Entity Too Large");
	if (_body.length() + data.length() > _maxBodySize)
		throw ErrorException(413, "Request Entity Too Large");

	_body.append(data);
	data.clear();
	if (_body.length() == bodySize)
		_parseState = END_STATE;
	return;
}

void	Request::parseChunkSize(std::string &data) {

	std::stringstream	ss;
	std::size_t			pos;

	pos = data.find(LF);
	if (pos == std::string::npos)
		return;
	ss << std::hex << data.substr(0, pos);
	ss >> _chunkSize;
	if (!_chunkSize)
		_parseState = END_STATE;
	_isChunkSize = true;
	data.erase(0, pos + 1);
	return;
}

void	Request::parseChunkedBody(std::string &data) {

	std::size_t	i;

	i = 0;
	while (i < data.length() and _chunkSize) {
		if (data[i] == '\n' and (i - 1 >= 0 and data[i - 1] == '\r'))
			_body.push_back('\n');
		else if (data[i] != '\r')
			_body.push_back(data[i]);
		i++;
		_chunkSize--;
	}
	if (!_chunkSize) {
		_isChunkSize = false;
		i += 2;
	}
	data.erase(0, i);
}

void	Request::saveChunkedBody(std::string &data) {

	while (_parseState != END_STATE) {
		if (!_isChunkSize)
			parseChunkSize(data);
		if (_isChunkSize and _parseState != END_STATE)
			parseChunkedBody(data);
	}
}

void	Request::parseUri(void) {

	std::size_t	pos;

	pos = _uri.find("?");
	if (pos != std::string::npos) {
		_query = _uri.substr(pos + 1);
		_uri.erase(pos);
	}

	parsePercent(_uri);
	/*
		probably add some parsing for _query
	*/
	return;
}

void	Request::parsePercent(std::string &strRef) {

	std::stringstream	ss;
	std::string			tmp;
	int					c;

	for (std::size_t i = 0; i < strRef.length(); i++) {
		if (strRef[i] == '%') {
			try {
				ss << std::hex << strRef.substr(i + 1, 2);
				ss >> c;
				tmp = strRef.substr(i + 3);
				strRef.erase(i);
				strRef.push_back(static_cast<char>(c));
				strRef.append(tmp);
				ss.clear();
			}
			catch(std::exception &e) {
				throw ErrorException(400, "Bad Request");
			}
		}
		else if (strRef[i] == '+')
			strRef = strRef.substr(0, i) + " " + strRef.substr(i + 1);
	}
	return;
}
