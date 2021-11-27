#include "../../includes/MainIncludes.hpp"

Request::Request(std::map<std::string, Location> const &l)
: _locationsMap(l), _parseState(START_LINE), _maxBodySize(1000), _method(""),
_protocol(""), _uri(""), _body(""), _tmpBuffer(""),
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

	newLinePos = data.find(CR LF);
	for (; newLinePos != std::string::npos; newLinePos = data.find(CR LF)) {
		if (_parseState == START_LINE)
			saveStartLine(data.substr(0, newLinePos));
		else if (_parseState == HEADER_LINE)
			saveHeaderLine(data.substr(0, newLinePos));
		else
			saveBodyPart(data.substr(0, newLinePos));
		data.erase(0, newLinePos + 2);
	}
	_tmpBuffer = data;
	if (_parseState == END_STATE)
		_isReqDone = true;
	// showState();
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
	return "unknown url";
}
