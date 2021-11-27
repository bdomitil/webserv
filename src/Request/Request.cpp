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

std::string	Request::getMethod() const {
	return _method;
}

bool	Request::saveRequestData(ssize_t recvRet) {

	std::string	data;
	std::size_t	newLinePos;

	data = _tmpBuffer;
	_buffer[recvRet] = '\0';
	data += _buffer;

	if (_parseState == START_LINE or _parseState == HEADER_LINE)
		saveStartLineHeaders(data);
	if (_parseState == BODY_LINE) {
		if (_transferEncoding == "chunked")
			saveChunkedBody(data);
		else
			saveSimpleBody(data);
	}
	_tmpBuffer = data;
	if (_parseState == END_STATE)
		_isReqDone = true;
		showState();
	}
	return _isReqDone;
}

std::string	Request::getUrl(std::uint32_t &status) const {
	std::map<std::string, Location>::const_iterator	i;
	std::string										pathToTarget;
	std::string										target;
	std::string										tmp;
	size_t											lastSlashPos;

	lastSlashPos = _uri.find_last_of("/");
	target = _uri.substr(lastSlashPos + 1);
	pathToTarget = _uri.substr(0, lastSlashPos + 1);
	tmp = pathToTarget.substr(0, lastSlashPos + 1);
	while (lastSlashPos != std::string::npos) {
		for (i = _locationsMap.begin(); i != _locationsMap.end(); i++) {
			if (tmp == i->first) {
				if (i->second.redirect.first) {
					status = static_cast<std::uint32_t>(i->second.redirect.first);
					return (i->second.redirect.second);
				}
				if (!target.length())
					target = i->second.index;
				status = 200;
				pathToTarget = i->second.root + pathToTarget;
				pathToTarget += ((pathToTarget[pathToTarget.length() - 1] == '/') ? target : "/" + target);
				return (pathToTarget);
			}
		}
		lastSlashPos = pathToTarget.find_last_of("/", lastSlashPos - 1);
		tmp = pathToTarget.substr(0, lastSlashPos);
	}
	status = 404;
	return ("unknown url");
}

void	Request::showState(void) const {

	std::cout << YELLOW "STATUS: "
		<< ((_isReqDone) ? GREEN "TRUE" RESET : RED "FALSE" RESET);
	std::cout << std::endl;

	std::cout << MAGENTA ">>>> START LINE <<<<" RESET << std::endl;
	std::cout << _method << " " << _uri << " " << _protocol << std::endl;

	std::cout << MAGENTA ">>>> HEADERS <<<<" RESET << std::endl;
	for (std::map<std::string, std::string>::const_iterator i = _headers.begin();
		i != _headers.end(); i++) {
		std::cout << i->first << ": ";
		std::cout << i->second << std::endl;
	}
	std::cout << MAGENTA ">>>> BODY <<<<" RESET << std::endl;
	std::cout << YELLOW << "Body size: " GREEN << _bodySize << RESET << std::endl;
	std::cout << YELLOW << "Max body size: " GREEN << _maxBodySize << RESET << std::endl;
	std::cout << BLUE << _body << RESET;
	std::cout << RED "________________________endOfRequest________________________" RESET
		<< std::endl << std::endl;
	return;
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

//	getting path to needed resource
//	and the resource itself
	if (lastSlashPos == _uri.length() - 1) {
		pathToTarget = _uri;
		target = "";
	}
	else {
		pathToTarget = _uri.substr(0, lastSlashPos + 1);
		target = _uri.substr(lastSlashPos + 1);
	}

//	check if uri path is one of the locations
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
