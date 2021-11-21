
#include "../../includes/MainIncludes.hpp"

Response :: Response(Request &request){
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
	_leftBytes = 1;
	_inProc = false;
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

std::string Response :: makeBody(){
	string line;
	ifstream FILE(_url);

	_body += string(CRLF);
	if (!FILE.is_open())
		throw ErrorException("ERROR OPENNING URL");
	while (getline(FILE, line))
			_body += line + "\n";
	_body += string(CRLF);
	FILE.close();
	return (_body);
}


void Response :: sendRes(int socket){

	int		res;

	if (!_inProc){
		_inProc = true;
		_response.append(makeStatusLine());
		_response.append(makeHeaders());
		std::cout << _response << std::endl;
		_response.append(makeBody());
		_leftBytes = _response.length();
	}
	res = send(socket, _response.c_str(), _response.length(), 0);
	if (res == -1)
		throw ErrorException("ERROR SENDING DATA");

	// std::cerr << "LEFT AFTER SEND\n" << _response << std::endl;

	_response = _response.substr(res);
	_leftBytes -= res;
	if (_leftBytes < 1)
	{
		_inProc = false;
		_leftBytes = false;
	}

}
