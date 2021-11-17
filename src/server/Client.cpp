#include "../../includes/MainIncludes.hpp"

Client :: Client(int srvSocket, uint32_t body_size) : _reqBuff(body_size)
{
	_srvSocket = srvSocket;
	createSocket();
}

// Client :: Client(const Client &copy) : _reqBuff(2048){
// 	*this = copy;
// }

Client :: ~Client()
{
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

bool Client :: readRequest(void)
{
	#include <fstream>
	int res = recv(_fdSock, _reqBuff.getBuffer(), RECV_BUFFER_SIZE, 0);
	if (res == 0)
	{
		_isClosed = true;
		return (false);
	}
	_isRead = true;
	_toServe = _reqBuff.saveRequestData();
	return (_toServe); 
}

void Client :: response()
{
	std::cerr << "TRYING TO RESPONSE FOR CLIENT" << std::endl;
}