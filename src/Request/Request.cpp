#include "../../includes/MainIncludes.hpp"

Request::Request(std::multimap<std::string, Location> const &l)
: _locationsMap(l), _location(nullptr), _bodySize(0), _chunkSize(0),
_parseState(START_LINE), _isReqDone(false), _isChunkSize(false),
_buffer(new char[RECV_BUFFER_SIZE + 1]), _errorStatus(0) {
	return;
}

Request::~Request(void) {
	delete [] _buffer;
	return;
}

const Location	*Request::getLocation(void) const {
	return _location;
}

char	*Request::getBuffer(void) const {
	return _buffer;
}

std::map<std::string, std::string>	&Request::getHeaders(void) {
	return _headers;
}

std::string	Request::getMethod(void) const {
	return _method;
}

std::string	Request::getBody(void) {
	return _body;
}

std::string	Request::getQueryString(void) {
	return _query;
}

uint32_t	Request::getErrorStatus(void) const {
	return _errorStatus;
}

void	Request::setErrorStatus(const int s) {
	_errorStatus = s;
}

bool	Request::saveRequestData(ssize_t recvRet) {

	std::string	data;

	data = _tmpBuffer;

	_buffer[recvRet] = '\0';
	data.append(_buffer);

	if (_parseState == END_STATE)
		resetRequest();
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
	return _isReqDone;
}

std::string	Request::validateUrl(std::string &fullPath,
								std::uint32_t &status,
								std::uint8_t mode) {

	std::string	tmp;

	if (mode == DIR_MODE) {
		if (_method == "PUT") {
			status = 201;
			return fullPath;
		}
		tmp = fullPath + "/" + _location->getIndex();
		if (isDirOrFile(tmp.c_str()) == FILE_MODE) {
			if (!access(tmp.c_str(), R_OK))
				return tmp;
		}
		if (_location->getAutoIndex() == "on") {
			status = 1;
			return fullPath;
		}
		status = 403;
		return "forbidden";
	}
	else if (mode == FILE_MODE) {
		if (!access(fullPath.c_str(), R_OK))
			return fullPath;
		status = 403;
		return "forbidden";
	}
	if (_method == "POST")
		status = 201;
	else
		status = 404;
	return fullPath;
}

std::string	Request::getUrl(std::uint32_t &status) {

	std::string		fullPath;
	std::uint8_t	mode;
	std::string		tmp;

	if (_location and _location->redirect.second.length()) {
		status = _location->redirect.first;
		return _location->redirect.second;
	}
	status = 200;
	fullPath = _location->getRoot() + _uri;
	for (std::size_t i = 0; i < fullPath.length() - 1; i++)
		if (fullPath[i] == '/' and fullPath[i + 1] == '/')
			fullPath.erase(i + 1, 1);
	if (fullPath[fullPath.length() - 1] == '/')
		fullPath.pop_back();
	mode = isDirOrFile(fullPath.c_str());
	if (mode == NOT_FOUND) {
		status = 404;
		return "unknown url";
	}
	return (validateUrl(fullPath, status, mode));
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

void	Request::resetRequest(void) {

	_headers.clear();
	_maxBodySize = 0;
	_bodySize = 0;
	_chunkSize = 0;
	_parseState = START_LINE;
	_transferEncoding.clear();
	_method.clear();
	_protocol.clear();
	_uri.clear();
	_query.clear();
	_body.clear();
	_tmpBuffer.clear();
	_isChunkSize = false;
	_isReqDone = false;
	_errorStatus = 0;
	_location = nullptr;

	return;
}
