#include "../../includes/MainIncludes.hpp"

Request::Request(std::map<std::string, Location> const &l)
: _locationsMap(l), _bodySize(0), _parseState(START_LINE),
_isReqDone(false), _buffer(new char[RECV_BUFFER_SIZE + 1]) {
	return;
}

Request::~Request(void) {
	delete [] _buffer;
	return;
}

char	*Request::getBuffer(void) const {
	return _buffer;
}

std::map<std::string, std::string>	&Request::getHeaders() {
	return _headers;
}

std::string	Request::getMethod() const {
	return _method;
}

bool	Request::saveRequestData(ssize_t recvRet) {

	std::string	data;
	std::size_t	newLinePos;

	data = _tmpBuffer;
	_buffer[recvRet] = '\0';
	data.append(_buffer);

	if (_parseState == START_LINE)
		_headers.clear();
	if (_parseState == START_LINE or _parseState == HEADER_LINE)
		saveStartLineHeaders(data);
	if (_parseState == BODY_LINE) {
		if (_transferEncoding == "chunked")
			saveChunkedBody(data);
		else
			saveSimpleBody(data);
	}
	_tmpBuffer = data;
	if (_parseState == END_STATE) {
		_isReqDone = true;
		_tmpBuffer.clear();
		_body.clear();
		parseUri();
		showState();
		_parseState = START_LINE;
	}
	return _isReqDone;
}

std::string	Request::getUrl(std::uint32_t &status) const {

	std::string	pathToTarget;
	std::string	target;
	std::string	tmp;
	size_t		lastSlashPos;

	lastSlashPos = _uri.find_last_of("/");
	target = _uri.substr(lastSlashPos + 1);
	pathToTarget = _uri.substr(0, lastSlashPos);
	for (std::size_t i = 0; i < std::count(_uri.begin(), _uri.end(), '/'); i++) {
		std::map<std::string, Location>::const_iterator	j = _locationsMap.begin();
		for (; j != _locationsMap.end(); j++) {
			tmp = (j->first != "/" and j->first[j->first.length() - 1] == '/') ?
				j->first.substr(0, j->first.find_last_of("/")) : j->first;
			if (((!pathToTarget.substr(0, lastSlashPos).length()) ?
				"/" : pathToTarget.substr(0, lastSlashPos)) == tmp) {
				if (j->second.redirect.first) {
					status = static_cast<std::uint32_t>(j->second.redirect.first);
					return (j->second.redirect.second);
				}
				if (!target.length())
					target = j->second.index;
				status = 200;
				pathToTarget = j->second.root + pathToTarget;
				pathToTarget += ((pathToTarget[pathToTarget.length() - 1] == '/') ?
					target : "/" + target);
				return (pathToTarget);
			}
		}
		lastSlashPos = _uri.find_last_of("/", lastSlashPos - 1);
	}
	status = 404;
	return "unknown url";
}

void	Request::showState(void) const {

	std::cout << YELLOW "STATUS: "
		<< ((_isReqDone) ? GREEN "TRUE" RESET : RED "FALSE" RESET);
	std::cout << std::endl;

	std::cout << MAGENTA ">>>> START LINE <<<<" RESET << std::endl;
	std::cout << BLUE << _method << " "
		<< _uri << " " << _protocol << RESET << std::endl << std::endl;

	std::cout << MAGENTA ">>>> HEADERS <<<<" BLUE << std::endl;
	for (std::map<std::string, std::string>::const_iterator i = _headers.begin();
		i != _headers.end(); i++) {
		std::cout << i->first << ": ";
		if (i->first == "Transfer-Encoding")
			std::cout << _transferEncoding << std::endl;
		else
			std::cout << i->second << std::endl;
	}
	std::cout << std::endl;
	std::cout << MAGENTA ">>>> BODY <<<<" RESET << std::endl;
	std::cout << YELLOW << "Body size: " GREEN << _bodySize << RESET << std::endl;
	std::cout << YELLOW << "Max body size: " GREEN << _maxBodySize << RESET << std::endl;
	std::cout << BLUE << _body << RESET << std::endl;
	std::cout << RED "________________________endOfRequest________________________" RESET
		<< std::endl << std::endl;
	return;
}
