#include "../../includes/MainIncludes.hpp"

Request::Request(std::map<std::string, Location> const &l)
: _locationsMap(l), _maxBodySize(1000), _bodySize(0),
_parseState(START_LINE), _isReqDone(false),
_buffer(new char[RECV_BUFFER_SIZE + 1]) {
	return;
}

Request::~Request(void) {
	delete [] _buffer;
	return;
}

char	*Request::getBuffer(void) const {
	return _buffer;
}

std::string	Request::getMethod() const {
	return _method;
}

bool	Request::saveRequestData(ssize_t recvRet) {

	std::string	data;
	std::size_t	newLinePos;

	data = _tmpBuffer;
	_buffer[recvRet] = '\0';
	data += _buffer;
	newLinePos = data.find(CR LF);
	while (newLinePos != std::string::npos
		and (_parseState == START_LINE or _parseState == HEADER_LINE)) {

		if (_parseState == START_LINE)
			saveStartLine(data.substr(0, newLinePos));
		else if (_parseState == HEADER_LINE)
			saveHeaderLine(data.substr(0, newLinePos));
		data.erase(0, newLinePos + 2);
		newLinePos = data.find(CR LF);
	}


	newLinePos = data.find(LF);
	while (newLinePos != std::string::npos
		and _parseState == BODY_LINE) {
		saveBodyPart(data.substr(0, newLinePos));
		data.erase(0, newLinePos + 1);
		newLinePos = data.find(LF);
	}
	_tmpBuffer = data;
	if (_parseState == END_STATE) {
		_isReqDone = true;
	}
	showState();
	return _isReqDone;
}

std::string	Request::getUrl(std::uint32_t &status) const {

	std::string	pathToTarget;
	std::string	target;
	size_t		lastSlashPos;

	lastSlashPos = _uri.find_last_of("/");
	if (lastSlashPos == std::string::npos) {
		return "bad url";
		//throw ErrorException(403, "Forbidden");
	}

	if (lastSlashPos == _uri.length() - 1) {
		pathToTarget = _uri;
		target = "";
	}
	else {
		pathToTarget = _uri.substr(0, lastSlashPos + 1);
		target = _uri.substr(lastSlashPos + 1);
	}

	std::map<std::string, Location>::const_iterator i = _locationsMap.begin();
	for ( ; i != _locationsMap.end(); i++) {
		if (pathToTarget == i->first) {
			if (i->second.redirect.first) {
				status = static_cast<std::uint32_t>(i->second.redirect.first);
				return (i->second.redirect.second);
			}
			if (!target.length())
				target = i->second.index;
			status = 200;
			return (i->second.root + pathToTarget + target);
		}
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
	std::cout << BLUE << _body << RESET;
	std::cout << RED "________________________endOfRequest________________________" RESET
		<< std::endl << std::endl;
	return;
}
