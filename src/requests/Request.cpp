#include "Request.hpp"

Request::Request() :
_method(""), _uri(""), _proto(""), _body(""), _header(""), _mapHeaders(std::map<std::string, std::string>()) {
}

Request::~Request() {}

void print_buffer(std::string & buffer) {
	for (size_t i = 0; i < buffer.length(); ++i) {
		if (buffer[i] == '\n')
			printf("%s", "\\n");
		else if (buffer[i] == '\r')
			printf("%s", "\\r");
		else
			printf("%c", buffer[i]);
	}
	printf("\n");
}

void Request::printRequest() {
	printf("%s|\n%s|\n%s|\n%s|", _method.c_str(), _uri.c_str(), _proto.c_str(), _body.c_str());
}

void Request::printMap() {
	for (std::map<std::string, std::string>::iterator i = _mapHeaders.begin(); i != _mapHeaders.end(); ++i) {
		std::string key = i->first;
//		printf("%s\n", _mapHeaders[key].c_str());
		printf("[%s] = ", key.c_str());
		print_buffer(_mapHeaders[key]);
	}
}

void Request::_parsFirstHeader(const std::string& buffer) {
    _method = buffer.substr(0, _findNth(buffer, 1, " "));
    if (_method.find("HEAD") != std::string::npos)
        _method = "HEAD";
	_uri = buffer.substr(_findNth(buffer, 1, " ") + 1,
						 _findNth(buffer, 2, " ") - _findNth(buffer, 1, " ") - 1);
	_proto = buffer.substr(_findNth(buffer, 2, " ") + 1, buffer.length() - _findNth(buffer, 2, " "));
}

std::string Request::_Key(std::string &buffer) {
	std::string tmpBuf = buffer.substr(0, buffer.find(':'));
	buffer.erase(0, buffer.find(':') + 1);
	buffer.erase(buffer.begin(),
				 std::find_if(buffer.begin(), buffer.end(), std::bind1st(std::not_equal_to<char>(), ' ')));
	return tmpBuf;
}

std::string Request::_Value(std::string &buffer) {
	std::string tmpBuf = buffer.substr(0, buffer.find(CRLF));
	return tmpBuf;
}

void Request::_mapingHeaders(std::string &buffer) {
	while (buffer.find(CRLF_END) != std::string::npos) {
		std::string tmpBuf = buffer.substr(0, buffer.find(CRLF));
		_mapHeaders.insert(std::pair<std::string, std::string>(_Key(tmpBuf), _Value(tmpBuf)));
		buffer.erase(0, buffer.find(CRLF) + 2);
	}
	buffer.erase(0, 2);
//	print_buffer(buffer);
}

void Request::_parsHeaders(std::string &buffer) {
	_parsFirstHeader(buffer.substr(0, buffer.find(CRLF)));
//	erase first string in req
	buffer.erase(0, buffer.find(CRLF) + 2);
	_mapingHeaders(buffer);
//	printf("%lu\n", _mapHeaders.size());
//	printMap();
}

void Request::parsBody(std::string &buffer) {
	_body += buffer.substr(0);
}


bool Request::_checkEndHeaders(std::string &buffer) {
	if (buffer.find(CRLF_END) == std::string::npos)
		return false;
	else
		return true;
}

bool Request::_checkContentLength() {
	if ( _mapHeaders.find("Content-Length") == _mapHeaders.end() ) {
		return false;
	} else {
		return true;
	}
}

size_t Request::_findNth(const std::string & str , unsigned int N, const std::string & find) {
	if (0 == N) { return std::string::npos; }
	size_t pos, from = 0;
	unsigned i = 0;
	while (i < N) {
		pos = str.find(find, from);
		if (std::string::npos == pos) { break; }
		from = pos + 1;
		++i;
	}
	return pos;
}

void Request::parsRequest(std::string & buffer) {
	if (_checkEndHeaders(buffer)) {
		_header.append(buffer.begin(), buffer.begin() + buffer.find(CRLF_END));
		_parsHeaders(buffer);
		if (_checkContentLength())
			parsBody(buffer);

		// printMap();
	}
	else if (_checkContentLength()) {
//		_body = reinterpret_cast<unsigned char *>((new (unsigned char))[atoi(_mapHeaders["Content-Length"].c_str())]);
		parsBody(buffer);

	}
	else if (_checkContentLength())
			parsBody(buffer);
	else {
	
	}
}

std::string Request::getHeader() const {
	return _header;
}

std::string	Request::getMethod() const { return _method; }
std::string Request::getUri() const { return _uri; }
std::string Request::getProto() const { return _proto; }
std::string Request::getBody() const { return _body; }
std::string Request::getValueMapHeader(std::string key) {
    if (_mapHeaders.find(key) == _mapHeaders.end())
        return "";
    return _mapHeaders.at(key);
}

void Request::setUri(const std::string &uri)
{
	_uri = uri;
}
