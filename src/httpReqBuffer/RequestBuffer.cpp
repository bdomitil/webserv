#include "RequestBuffer.hpp"

RequestBuffer::RequestBuffer(std::uint32_t bodySize)
: _maxBodySize(bodySize), _callCount(START_LINE), _tmpBuffer(""),
_reqBodyLines(""), _reqStartLine(""), _isReqDone(false) {
	return;
}

void	RequestBuffer::saveStartLine(std::string startLine) {
	if (!startLine.length())
		throw std::exception();
	_callCount = HEADER_LINE;
	_reqStartLine = startLine;
	return;
}

void	RequestBuffer::saveHeaderLine(std::string headerLine) {
	if (!headerLine.length())
		_callCount = BODY_LINE;
	_reqHeaders.insert(std::end(_reqHeaders), headerLine);
	return;
}

void	RequestBuffer::saveBodyPart(std::string bodyLine) {
	if (bodyLine.length() + _reqBodyLines.length() > _maxBodySize)
		throw std::exception();
	if (!bodyLine.length())
		_isReqDone = true;
	_reqBodyLines += bodyLine;
	return;
}

bool	RequestBuffer::saveRequestData(void *buffer, std::size_t len) {

	std::string	data;
	std::size_t	newLinePos;

	data = _tmpBuffer;
	(static_cast<char *>(buffer))[len] = '\0';
	data += static_cast<char *>(buffer);

	newLinePos = data.find("\n");
	for( ; newLinePos != std::string::npos; newLinePos = data.find("\n")) {
		if (_callCount == START_LINE)
			saveStartLine(data.substr(0, newLinePos));
		else if (_callCount == HEADER_LINE)
			saveHeaderLine(data.substr(0, newLinePos));
		else
			saveBodyPart(data.substr(0, newLinePos));
		data.erase(0, newLinePos);
	}
	_tmpBuffer = data;
	return _isReqDone;
}

void	RequestBuffer::showState(void) const {

	std::cout << "STATUS: "
		<< (_isReqDone) ? "TRUE" : "FALSE";
	std::cout << std::endl;

	std::cout << "START LINE: " << _reqStartLine << std::endl;

	std::cout << ">>>> HEADERS <<<<" << std::endl;
	std::vector<std::string const>::iterator i = std::begin(_reqHeaders);
	for( ; i != std::end(_reqHeaders); i++)
		std::cout << *i << std::endl;

	std::cout << ">>>> BODY <<<<" << std::endl;
	std::cout << _reqBodyLines << std::endl;
	return;
}
