#ifndef SERVER_CPP
#define SERVER_CPP
#include "../../includes/MainIncludes.hpp"

//#####             CANNONICAL PART

Server :: Server(const t_server &ServerSetting) {
	_name = ServerSetting.name;
	_host = ServerSetting.host;
	_port = ServerSetting.port;
	_locations = ServerSetting.locations;
	_errorPages = ServerSetting.errorPages;
	_serverSettings = t_server(ServerSetting);
}

Server :: Server(const Server &copy) {
	*this = copy;
}

Server :: ~Server()
{
	//Destruct if needed
}

Server& Server :: operator=(const Server &copy) {
	*this = Server(copy.getSettings());
	return (*this);
}

t_server Server :: getSettings(void) const {
	return (this->_serverSettings);
}

// ##              SERVER.RUN()

int Server :: createSocket(void) {
	int srsocket, reuseaddr = 1;
	_sockaddr = getSockAddr();
	if (inet_pton(AF_INET, (const char*)&_host, &(_sockaddr.sin_addr.s_addr)) < 0)
			throw(ErrorException(strerror(errno)));
	_sockaddr.sin_family = PF_INET;
	_sockaddr.sin_port = htonl(_port);
	srsocket = socket(PF_INET, SOCK_STREAM, 0); //SOCK_STREAM - tcp proto, if tcp/udp - third param = 0
	if (srsocket == -1)
		throw(ErrorException("Unable to create socket"));
	if (setsockopt(srsocket ,SOL_SOCKET ,SO_REUSEADDR , &reuseaddr,sizeof(int)) == -1)  //allow to reuse socket after crash
		throw(ErrorException("Error setting sockopt"));
	if (fcntl(srsocket, F_SETFL, O_NONBLOCK) == -1)  //unblocking listening socket
		throw(ErrorException("Error using fcntl"));
	return (srsocket);
}

void Server :: Run(void) {
	_fdSock =  createSocket();
	if (bind(_fdSock, (struct sockaddr*)&_sockaddr, sizeof(_sockaddr)) == -1)  //binding socket 
		throw (ErrorException("Error binding socket : " + static_cast<string>(strerror(errno))));
	if (listen(_fdSock, 100) == -1)  //make core listen and wait connections
		throw(ErrorException("Error listening socket : " + static_cast<string>(strerror(errno))));
}



void Start(vector<Server*> Servers)
{
	fd_set readfd, writefd;
	vector<int> readFd, writeFd;

	int max_fd = 0;;
	for (size_t i; i < Servers.size(); i++)
	{
		try
		{
			Servers[i]->Run();
		}
		catch(const std::exception& e) // TODO if exception then delete Server from vector
		{
			std::cerr << e.what() << '\n';
		}
		

	}
	if (Servers.empty()) //if no listening servers exit
		return ;
	while (1)
	{
		int new_fd;
		int first_client_fd;
		t_time timeout;
		vector <Client*> Clients;
		FD_ZERO(&writefd);
		FD_ZERO(&readfd);
		for (size_t i = 0; i < Servers.size(); i++)  //Add server fd to set for listeing connections
		{
			readFd.insert(readFd.end(), Servers[i]->getSocket());
			if (max_fd < Servers[i]->getSocket())
				max_fd = Servers[i]->getSocket();		
		}
		for (size_t i = 0; i < Clients.size(); i++) // Add client's fd into set for reading its request
		{
			readFd.insert(readFd.end(), Clients[i]->getSocket());
			FD_SET(Servers[i]->getSocket(), &readfd);
			if (max_fd < Servers[i]->getSocket())
				max_fd = Servers[i]->getSocket();		
		}
		timeout.tv_sec = 1;
		timeout.tv_usec = 50000;
		int res = select(max_fd, &readfd, &writefd, NULL, &timeout);
		errno = 0;
		if (res < 1)
		{
			std::cerr << strerror(errno) << std::endl; 
			continue;
		}
		else if (res = 0)
			continue ;
		for (vector<int> :: iterator start = readFd.begin(); start != readFd.end(), res > 0; start++) //
		{
			if (FD_ISSET(*start, &readfd)) // TODO client has info to read or server to accept connection
			{
				//if server ip then accept and create new client
				//else
				// read from socket
			}
		}
		for (vector<Client*> :: iterator start = Clients.begin(); start != Clients.end(); start++)
		{
			if ((*start)->toServe())  //if client's answer is ready then send part by part
				// send((*start)->getSocket, ) 
				;
		}
		// for (vector<Server*>::iterator start = Servers.begin(); start != Servers.end(); start++)
		// {

		// }
		
		

	}
	
}

#endif
