#include "../../includes/MainIncludes.hpp"

Client :: Client(int srvSocket, std::map<std::string, Location> const &locations) :
	_request(locations), _response(nullptr) {

	_srvSocket = srvSocket;
	_isClosed = false;
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
	ssize_t res = recv(_fdSock, this->_request.getBuffer(), RECV_BUFFER_SIZE, 0);
	if (res == 0) {
		_isClosed = true;
		return (false);
	}
	_isRead = true;
	_toServe = this->_request.saveRequestData(res);
	return (_toServe);
}

bool Client :: response() {
	if (!_response)
		_response = new Response(_request);
	std::cerr << "TRYING TO RESPONSE FOR CLIENT" << std::endl;
	try {
		_response->sendRes(_fdSock);
		if (_response->isSent()) {
			_toServe = false;
			return (true);
		}
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return (false);
}
