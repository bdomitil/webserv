#include "../../includes/MainIncludes.hpp"

//#####             CANNONICAL PART

Server :: Server(const t_server &ServerSetting)
{
	_name = ServerSetting.name;
	_host = ServerSetting.host;
	_port = ServerSetting.port;
	_locations = ServerSetting.locations;
	_errorPages = ServerSetting.errorPages;
	_serverSettings = t_server(ServerSetting);
}

Server :: Server(const Server &copy)
{
	*this = copy;
}

Server& Server :: operator=(const Server &copy)
{
	*this = Server(copy.getSettings());
	return (*this);
}

t_server Server :: getSettings(void) const
{
	return (this->_serverSettings);
}

// ##              SERVER.RUN()

int Server :: createSocket(void)
{
	int srsocket, reuseaddr = 1;
	_sockaddr = getSockAddr();
	if (inet_pton(AF_INET, (const char*)&_host, &(_sockaddr.sin_addr.s_addr)) < 0)
			throwException(strerror(errno));
	_sockaddr.sin_family = PF_INET;
	_sockaddr.sin_port = htonl(_port);
	srsocket = socket(PF_INET, SOCK_STREAM, 0); //SOCK_STREAM - tcp proto, if tcp/udp - third param = 0
	if (srsocket == -1)
			throwException("Unable to create socket");
	if (setsockopt(srsocket ,SOL_SOCKET ,SO_REUSEADDR , &reuseaddr,sizeof(int)) == -1)  //allow to reuse socket after crash
		throwException("Error setting sockopt");
	if (fcntl(srsocket, F_SETFL, O_NONBLOCK) == -1)
		throwException("Error using fcntl");
	return (srsocket);
}

void Server:: Run(void)
{
	_fdSock =  createSocket();
	if (bind(_fdSock, (struct sockaddr*)&_sockaddr, sizeof(_sockaddr)) == -1)
		throwException("Error binding socket : " + static_cast<string>(strerror(errno)));
	if (listen(_fdSock, 10) == -1)
		throwException("Error listening socket : " + static_cast<string>(strerror(errno)));
}





















void Server :: throwException(string msg) const
{
	SrvErrorExpeption error;
	msg += "\n";
	error.errorMsg = (const char *)&msg;
	throw error;
} 
