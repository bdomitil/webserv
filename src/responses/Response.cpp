
#include "../../includes/MainIncludes.hpp"

Response :: Response(Request &request) : _response(""), _body(nullptr) {

	t_fileInfo file;

	if (urlInfo(request.getUrl(_statusCode), &file))
	{
		_bodySize = file.fLength;
		_contentType = file.fExtension;
	}
	else if (_statusCode != 301)
		_statusCode = 404;
	_url = request.getUrl(_statusCode);
	std::cerr << "URL: " << _url << std::endl;
	_leftBytes = 0;
	_inProc = false;
}

std::string	Response::getResponse(void) const {
	return _response;
}

string Response :: makeStatusLine(){
	if (_statusCode == 200)
		_statusLine = "HTTP/1.1 " + ft_itoa(_statusCode) + " " + "OK" + "\n";
	else
		_statusLine = "HTTP/1.1 " + ft_itoa(_statusCode) + " " + "ERROR" + "\n";
	return (_statusLine);
}

std::string Response :: makeHeaders(){
	const std::time_t current_time = std::time(0);
	_tmpHead += "Server: ~Server()" + string(CRLF);
	_tmpHead += "Date: " + string(ctime(&current_time));
	if (_statusCode == 301)
		_tmpHead += "Location: " + _url + string(CRLF);
	_tmpHead += "Content-Type: " + _contentType + string(CRLF);
	_tmpHead += "Content-Length: " + ft_itoa(_bodySize) + string(CRLF);
	_tmpHead += "Accept-Ranges: bytes" + string(CRLF);
	_tmpHead += "Connection: close" + string(CRLF);
	return(_tmpHead);
}

char *Response :: makeBody(int &readSize){

	if (!_inProc)
	{
		_FILE.open(_url);
		if (!_FILE.is_open())
			throw ErrorException("ERROR OPENNING URL");
		_response.append("\r\n");
	}
	else
	{
		_body = new char[SEND_BUFFER_SIZZ];
		memset(_body, 0, SEND_BUFFER_SIZZ);
		_FILE.read(_body, SEND_BUFFER_SIZZ);
		readSize = _FILE.gcount();
		if (readSize == 0)
			_FILE.close();
	}
	return (_body);
}

void Response :: sendRes(int socket){

	int		res = 0;

	if (!_inProc){
		_response.append(makeStatusLine());
		_response.append(makeHeaders());
		makeBody(res);
		_leftBytes = _bodySize;
		res = send(socket, _response.c_str(), _response.length(), 0);
		if (res == -1)
			throw ErrorException("ERROR SENDING DATA");
		_response = string();
		_inProc = true;
	}
	else if (_FILE.is_open())
	{
		try
		{
			res = send(socket, makeBody(res) , res, 0);
			if (res == -1)
				throw ErrorException("ERROR SENDING DATA");
			_leftBytes -= res;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
		delete []    _body;
	}

	// std::cerr << "LEFT AFTER SEND\n" << _response << std::endl;
	//std::cout << MAGENTA ">>>>RESPONSE<<<<" RESET << std::endl <<  _response << std::endl;

	if (_leftBytes < 1)
	{
		_inProc = false;
		_leftBytes = false;
	}

}
