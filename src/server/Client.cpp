#include "../../includes/MainIncludes.hpp"

Client :: Client(int srvSocket, std::multimap<std::string, Location> const &locations) :
	_request(locations), _response(nullptr) {

	setenv("TZ", "/usr/share/zoneinfo/Europe/Moscow", 1);
	_session_start = std::time(0);
	tm *tmp = std::localtime(&_session_start);
	_session_start = std::mktime(tmp);
	// tmp->tm_hour += 30;
	tmp->tm_min += 1;
	_session_end = std::mktime(tmp);
	_srvSocket = srvSocket;
	isClosed = false;
	_toServe = false;
	_isRead = false;
	createSocket();
}


// Client :: Client(const Client &copy) : _reqBuff(2048){
// 	*this = copy;
// }

Client :: ~Client()
{
	delete _response;
	if (DEBUG) {
		unsigned int code;
		std::cout << MAGENTA "Client : " << _ip << " (";
		std::cout << _fdSock << ((close(_fdSock) == -1) ? " not closed)" : " closed)") << std::endl;
	}
	close(_fdSock);
	//Destruct if needed
}

// Client& Client :: operator=(const Client &copy) {
// 	this->_fdSock = copy.getSocket();
// 	return (*this);
// }

int Client :: createSocket(void) {
	int reuseaddr = 1;
	socklen_t sockelen = sizeof(_sockaddr);
	_fdSock = accept(_srvSocket, (sockaddr *)&_sockaddr, &sockelen);
	if (_fdSock == -1)
		throw (ErrorException(strerror(errno)));
	if (inet_ntop(AF_INET, (const char*)&_sockaddr.sin_addr.s_addr , (char *)&_ip, 32) == nullptr)
		throw(ErrorException(strerror(errno)));
	if (setsockopt(_fdSock ,SOL_SOCKET ,SO_REUSEADDR , &reuseaddr,sizeof(int)) == -1)  //allow to reuse socket after crash
		throw(ErrorException("Error setting sockopt"));
	if (fcntl(_fdSock, F_SETFL, O_NONBLOCK) == -1)  //unblocking listening socket
		throw(ErrorException("Error using fcntl"));
	return (_fdSock);
}

bool Client :: readRequest(void) {
	_session_end =  increase_session_time();
	ssize_t res = recv(_fdSock, this->_request.getBuffer(), RECV_BUFFER_SIZE, 0);
	if (res == 0) {
		isClosed = true;
		return (false);
	}
	else if (res == -1)
		throw ErrorException(502, "ERROR RECV DATA");
	_isRead = true;
	try {
		_toServe = this->_request.saveRequestData(res);
	}
	catch (ErrorException &e) {
		_toServe = true;
		_request.setErrorStatus(e.getStatus());
	}
	return (_toServe);
}

void Client :: response(std::map<int, std::string> &errorPages) {
	// std::cout << "TRYING TO RESPONSE FOR CLIENT " << this->_ip << std::endl;
	try {
		if (!_response)
			_response = new Response(_request, errorPages);
		_response->sendRes(_fdSock);
		if (_response->isSent()){
			_toServe = false;
			delete _response;
			_response = nullptr;
		}
	}
	catch(const std::exception& e) {
		//std::cerr << e.what() << std::endl;
		_toServe = false;
		isClosed = true;
	}
}

bool Client :: SessionIsOver(){
	uint32_t tmp;

	if (isClosed)
		return (true);
	if (_session_end <= _session_start){
		std::cerr << BLUE << "Session of " << _ip << " recuesting " << _request.getUrl(tmp)  << " is timed out" << RESET <<std::endl;
		close(_fdSock);
		return (true);
	}
	_session_start = std::time(0);
	return (false);
}
