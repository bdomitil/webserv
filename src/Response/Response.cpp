
#include "../../includes/MainIncludes.hpp"

Response :: Response(Request &request, std::map<int, std::string> errorPages)
: _response(""), _body(nullptr),
_errorPages(errorPages), _reqLocation(nullptr), _cgiPtr(nullptr) {

	t_fileInfo file;

	_contentType = "text/html";
	if ((_statusCode  = request.getErrorStatus()) == 0){
		_bodySize = 0;
		_reqHeaders = request.getHeaders();
		_url = request.getUrl(_statusCode);
		_reqLocation = request.getLocation();
		_autoindex = _statusCode == 1;
	}
	if (_statusCode < 399 && _statusCode != 1) {
		urlInfo(_url, &file,  _FILE);
		if (file.fType == DDIR)
			file.fStatus = 404;
		if ((file.fStatus < 200 || file.fStatus > 299) && _statusCode != 301){
			_statusCode = file.fStatus;
			_url = getErrorPage();
			_contentType = "text/html";
		}
		else if (_statusCode != 301) {
			int cgNum;
			if ((cgNum = checkCgi(request.getLocation()->getCgi(), _url)) > 0){
				_cgiPtr = new Cgi(request, request.getLocation()->getCgi(), _FILE);
				_contentType = file.fExtension;
				try {
					_cgiFd =  _cgiPtr->initCGI(cgNum, helper);
				}
				catch(ErrorException &e) {
					std::cerr << e.what() << " due to " << strerror(errno) << std::endl;
					_statusCode = 502;
				}
				_bodySize = file.fLength;
				_contentType = file.fExtension;
				_leftBytes = _bodySize;
			}
			else if (cgNum == -1){
				_statusCode = 502;
				_url = getErrorPage();
			}
			else {
				_bodySize = file.fLength;
				_contentType = file.fExtension;
			}
		}
	}
	else
		_url = getErrorPage();
	_inProc = false;
}

Response :: ~Response(){
	if (_cgiPtr)
		delete _cgiPtr;
}

std::string	Response::getResponse(void) const {
	return _response;
}

string Response :: getErrorPage() {
	char *def_page;

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
	if (_autoindex)
		def_page = (gen_def_page(_statusCode, _bodySize, _url.c_str(), _reqLocation));
	else{
		def_page = (gen_def_page(_statusCode, _bodySize, nullptr, _reqLocation));
		_url = "ERROR";
	}
	delete def_page;
	return (_url);
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

	if (_inProc || _cgiPtr) {
		if (_url != "ERROR" && !_autoindex) {
			_body = new char[SEND_BUFFER_SIZZ];
			memset(_body, 0, SEND_BUFFER_SIZZ);
			if (_cgiPtr && _cgiPtr->isReadable()){
				readSize = read(_cgiFd[0], _body, SEND_BUFFER_SIZZ);
				if (readSize == 0 || readSize == -1)
					_cgiPtr->toRead(false);
			}
			else{
					_FILE.read(_body, SEND_BUFFER_SIZZ);
					readSize = _FILE.gcount();
			}
			if (_FILE.eof())
				_FILE.close();
		}
		else if (_autoindex) {
			_body = gen_def_page(_statusCode, _bodySize, _url.c_str(), _reqLocation);
			readSize = _bodySize;
		}
		else {
			_body = gen_def_page(_statusCode, _bodySize, _url.c_str(), _reqLocation);
			readSize = _bodySize;
		}
	}
	return (_body);
}

void Response :: sendRes(int socket){

	int		res = 0;

	if (_inProc == false && (!_cgiPtr || _cgiPtr->isReadable())){
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
	else if (_FILE.is_open() || _statusCode != 200 || _autoindex || _cgiPtr->isReadable()) {
		int to_send, pos, tries;
		res = 0, pos = 0, tries = 0;
		makeBody(to_send);
		try {
			while (pos != to_send) {
				if (_cgiPtr && !_cgiPtr->isReadable())
					res = write(_cgiFd[1], &(_body[pos]), to_send);
				else
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
		delete [] _body;
	}
	if (_leftBytes < 1 && !_cgiPtr) {
		_inProc = false;
		_leftBytes = false;
		if (_cgiPtr)
			delete _cgiPtr;
		_cgiPtr = nullptr;
	}
	else if (_leftBytes < 1 && _cgiPtr) {
		if (!_cgiPtr->isReadable()){
			close(_cgiFd[1]);
			waitpid(helper, 0, 0);
			_bodySize = getFdLen(_cgiFd[0]);
			_leftBytes = _bodySize;
			if (_leftBytes == -1){
				_statusCode = 500;
				delete _cgiPtr;
				_cgiPtr = nullptr;
				_inProc = false;
				close(_cgiFd[0]);
			}
			else
				_cgiPtr->toRead(true);
		}
		else{
			close(_cgiFd[0]);
			delete _cgiPtr;
			_cgiPtr = nullptr;
			_inProc = false;
		}
	}
}
