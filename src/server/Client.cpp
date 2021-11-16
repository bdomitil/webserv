#include "../../includes/MainIncludes.hpp"

Client :: Client() {

}

Client :: Client(int srvSocket)
{
	_srvSocket = srvSocket;
	createSocket();
}

Client :: Client(const Client &copy) {
	*this = copy;
}

Client :: ~Client()
{
	//Destruct if needed
}

Client& Client :: operator=(const Client &copy) {
	this->_fdSock = copy.getSocket();
	return (*this);
}

int Client :: createSocket(void) {
	int reuseaddr = 1;
	socklen_t sockelen = sizeof(_sockaddr);
	_fdSock = accept(_srvSocket, (sockaddr *)&_sockaddr, &sockelen);
	if (_fdSock == -1)
		throw (ErrorException(strerror(errno)));
	if (inet_ntop(AF_INET, (const char*)&_sockaddr.sin_addr.s_addr , (char *)&_ip, 32) < 0)
		throw(ErrorException(strerror(errno)));
	if (setsockopt(_fdSock ,SOL_SOCKET ,SO_REUSEADDR , &reuseaddr,sizeof(int)) == -1)  //allow to reuse socket after crash
		throw(ErrorException("Error setting sockopt"));
	if (fcntl(_fdSock, F_SETFL, O_NONBLOCK) == -1)  //unblocking listening socket
		throw(ErrorException("Error using fcntl"));
	return (_fdSock);
}