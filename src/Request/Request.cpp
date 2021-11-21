#include "../../includes/MainIncludes.hpp"

Request::Request(std::map<std::string, Location> const &l)
: _locationsMap(l), _parseState(START_LINE),
_method(""), _protocol(""), _uri(""), _body(""),
_tmpBuffer(""), _isReqDone(false), _buffer(new char[RECV_BUFFER_SIZE + 1]) {
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
	showState();
	return _isReqDone;
}

void	Request::showState(void) const {

	std::cout << "STATUS: "
		<< ((_isReqDone) ? "TRUE" : "FALSE");
	std::cout << std::endl;

	std::cout << ">>>> START LINE <<<<" << std::endl;
	std::cout << _method << " " << _uri << " " << _protocol << std::endl;

	std::cout << ">>>> HEADERS <<<<" << std::endl;
	for (std::map<std::string, std::string>::const_iterator i = _headers.begin();
		i != _headers.end(); i++) {
		std::cout << i->first << ": ";
		std::cout << i->second << std::endl;
	}
	std::cout << ">>>> BODY <<<<" << std::endl;
	std::cout << _body << std::endl;
	std::cout << "________________________" << std::endl << std::endl;
	return;
}

std::string	Request::getUrl(std::uint32_t &status) const {

	std::string	pathToTarget;
	std::string	target;
	size_t		lastSlashPos;

	(void)status;
	lastSlashPos = _uri.find_last_of("/");
	if (lastSlashPos == std::string::npos) {
		return "lala";
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

//	check if location is present
	for (std::map<std::string, Location>::const_iterator i = _locationsMap.begin();
		i != _locationsMap.end(); i++) {
		if (pathToTarget == i->first) {
			if (!target.length())
				target = i->second.index;
			return (i->second.root + pathToTarget + target);
		}
	}

	return "hz";
}
