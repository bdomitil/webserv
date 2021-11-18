#include "../../includes/MainIncludes.hpp"

Request::Request(std::map<std::string, Location> &l)
: _locationsMap(l), _parseState(START_LINE),
_method(""), _protocol(""), _target(""),_body(""),
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

bool	Request::saveRequestData(ssize_t recvRet) {

	std::string	data;
	std::size_t	newLinePos;

	data = _tmpBuffer;
	_buffer[recvRet] = '\0';
	data += _buffer;

	newLinePos = data.find(LF);
	for ( ; newLinePos != std::string::npos; newLinePos = data.find(LF)) {
		if (_parseState == START_LINE)
			saveStartLine(data.substr(0, newLinePos));
		else if (_parseState == HEADER_LINE)
			saveHeaderLine(data.substr(0, newLinePos));
		else
			saveBodyPart(data.substr(0, newLinePos));
		data.erase(0, newLinePos + 1);
	}
	_tmpBuffer = data;
	if (_parseState == END_STATE)
		_isReqDone = true;
	return _isReqDone;
}

void	Request::showState(void) const {

	std::cout << "STATUS: "
		<< ((_isReqDone) ? "TRUE" : "FALSE");
	std::cout << std::endl;

	//std::cout << "START LINE: " << _reqStartLine << std::endl;

/*
	std::cout << ">>>> HEADERS <<<<" << std::endl;
	std::vector<std::string const>::iterator i = std::begin(_reqHeaders);
	for ( ; i != std::end(_reqHeaders); i++)
		std::cout << *i << std::endl;
*/

	std::cout << ">>>> BODY <<<<" << std::endl;
	std::cout << _body << std::endl;
	std::cout << "________________________" << std::endl << std::endl;
	return;
}
