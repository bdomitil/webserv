
#include "../../includes/MainIncludes.hpp"

Response :: Response(Request &request, std::map<int, std::string> errorPages) : _response(""), _body(nullptr), _errorPages(errorPages) {

	t_fileInfo file;

	_contentType = "text/html";
	if ((_statusCode  = request.getErrorStatus()) == 0){
		_bodySize = 0;
		_reqHeaders = request.getHeaders();
		_url = request.getUrl(_statusCode);
	}
	if (_statusCode < 399) {
		urlInfo(_url, &file,  _FILE);
		if (file.fType == DDIR)
			file.fStatus = 404;
		if ((file.fStatus < 200 || file.fStatus > 299) && _statusCode != 301){
			_statusCode = file.fStatus;
			_url = getErrorPage();
			_contentType = "text/html";
		}
		else if (_statusCode != 301) {
			_bodySize = file.fLength;
			_contentType = file.fExtension;
		}
	}
	else
		_url = getErrorPage();
	std::cerr << "URL: " << _url << std::endl;
	_leftBytes = 0;
	_inProc = false;
}

std::string	Response::getResponse(void) const {
	return _response;
}

string Response :: getErrorPage() {

	for (map <int, string> :: iterator i = _errorPages.begin(); i != _errorPages.end(); i++) {
		if (i->first == _statusCode){
			t_fileInfo file;
			urlInfo(i->second, &file, _FILE);
			if (file.fStatus == 200){
				_bodySize = file.fLength;
				_contentType = file.fExtension;
				return i->second;
			}
		}
	}
	char *def_page = (gen_def_page(_statusCode, _bodySize));
	delete def_page;
	return ("ERROR");
}

string Response :: makeStatusLine(){

	_statusLine = "HTTP/1.1 " + ft_itoa(_statusCode) + " " + error_map()[_statusCode] + CRLF;
	return (_statusLine);
}

std::string Response :: makeHeaders() {
	const std::time_t current_time = std::time(0);
	_headers += "Server: SuperServer 1.1" + string(CRLF);
	_headers += "Date: " + string(ctime(&current_time));
	if (_statusCode == 301)
		_headers += "Location: " + _url + string(CRLF);
	if (_statusCode < 300 || _statusCode > 399){
		_headers += "Content-Type: " + _contentType + string(CRLF);
		_headers += "Accept-Ranges: bytes" + string(CRLF);
	}
	_headers += "Set-Cookie: lastsess=" + string(ctime(&current_time));
	_headers += "Content-Length: " + ft_itoa(_bodySize) + string(CRLF);
	if (_reqHeaders["Connection"].size())
		_headers += "Connection: " + _reqHeaders["Connection"] + string(CRLF);
	else
		_headers += "Connection: close" + string(CRLF);
	_headers += string(CRLF);
	return(_headers);
}

char *Response :: makeBody(int &readSize) {

	char c;
	if (_inProc) {
		if (_url != "ERROR") {
			_body = new char[SEND_BUFFER_SIZZ];
			memset(_body, 0, SEND_BUFFER_SIZZ);
			_FILE.read(_body, SEND_BUFFER_SIZZ);
			readSize = _FILE.gcount();
			if (readSize == 0)
				_FILE.close();
		}
		else {
			_body = gen_def_page(_statusCode, _bodySize);
			readSize = _bodySize;
		}
	}
	return (_body);
}

void Response :: sendRes(int socket){

	int		res = 0;

	if (!_inProc){		
		_response.append(makeStatusLine());
		_response.append(makeHeaders());
		_leftBytes = _bodySize;
		res = send(socket, _response.c_str(), _response.length(), 0);
		if (res == -1)
			throw ErrorException("ERROR SENDING DATA");
		if (DEBUG)
			std::cout << "\n\n" << _response << std::endl << std::endl;
		_response = string();
		_inProc = true;

	}
	else if (_FILE.is_open() || _statusCode != 200) {
		int to_send, pos, tries;
		res = 0, pos = 0, tries = 0;
		makeBody(to_send);
		try {
			while (pos != to_send) {
				res = send(socket, &(_body[pos]) , to_send, 0);
				pos += res;
				if (tries++ == 8) {
					_leftBytes = 0;
					throw ErrorException("TOO MANY ATTEMPTS TO SEND DATA");
				}
			}
			if (res == -1)
				throw ErrorException("ERROR SENDING DATA");
			_leftBytes -= res;
		}
		catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
		}
		delete []    _body;
	}

	// std::cerr << "LEFT AFTER SEND\n" << _response << std::endl;
	//std::cout << MAGENTA ">>>>RESPONSE<<<<" RESET << std::endl <<  _response << std::endl;

	if (_leftBytes < 1) {
		_inProc = false;
		_leftBytes = false;
	}


}
