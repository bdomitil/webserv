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



t_server const	&Server :: getSettings(void) const {
	return (this->_serverSettings);
}



int Server :: createSocket(void) {

	int srsocket, reuseaddr = 1;
	int res = -2;

	_sockaddr = getSockAddr();
	if ((res = inet_pton(AF_INET, _host.c_str(), &(_sockaddr.sin_addr.s_addr))) < 1)
			throw(ErrorException(strerror(errno)));
	_sockaddr.sin_family = PF_INET;
	_sockaddr.sin_port = htons(_port);
	srsocket = socket(PF_INET, SOCK_STREAM, 0); //SOCK_STREAM - tcp proto, if tcp/udp - third param = 0
	if (srsocket == -1)
	{
		std::cerr << "Unable to create socket" << std::endl;
		throw(this);
	}
	if (setsockopt(srsocket ,SOL_SOCKET ,SO_REUSEADDR , &reuseaddr,sizeof(int)) == -1)  //allow to reuse socket after crash
	{
		std::cerr << "Error setting sockopt" << std::endl;
		throw(this);
	}
	if (fcntl(srsocket, F_SETFL, O_NONBLOCK) == -1)  //unblocking listening socket
	{
		std::cerr << "Error using fcntl" << std::endl;
		throw(this);
	}
	return (srsocket);
}



void Server :: Run(void) {
	_fdSock =  createSocket();
	if (bind(_fdSock, (struct sockaddr*)&_sockaddr, sizeof(_sockaddr)) == -1)
	{
		std::cerr << "Error binding socket : " << strerror(errno) << std::endl;
		throw (this);//binding socket
	}
	if (listen(_fdSock, 100) == -1)  //make core listen and wait connections
	{
		std::cerr << "Error listening socket : " << strerror(errno) << std::endl;
		throw(this);
	}
}



void Start(vector<Server*> Servers)
{
	fd_set readfd, writefd;
	vector<int> readFd, writeFd;
	map <int,Client*>Clients;
	Client *newCl;
	int max_fd = 0;

	//#########################################//
	//#########################################//
	//______________RUN SERVERS_______________#//
	//#########################################//
	//#########################################//

	for (size_t i = 0; i < Servers.size(); i++)
	{
		try
		{
			Servers[i]->Run();
			Servers[i]->_isrunning  = true;
		}
		catch (Server *serv)
		{
			vector<Server*>::iterator i = Servers.begin();
			while (*i != serv)
				i++;
			Servers.erase(i);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}

	while ( !Servers.empty() )
	{
		int new_fd, select_res = 0;
		t_time timeout;

		timeout.tv_usec = 50000;
		timeout.tv_sec = 2;
		FD_ZERO(&writefd);
		FD_ZERO(&readfd);
		readFd.clear();
		writeFd.clear();


		//#########################################//
		//#########################################//
		//___ADD SERVERS AND CLIENTS TO READ FDS__#//
		//#########################################//
		//#########################################//

		for (size_t i = 0; i < Servers.size(); i++)  //Add server fd to set for listeing connections
		{
			readFd.insert(readFd.end(), Servers[i]->getSocket());
			FD_SET(Servers[i]->getSocket(), &readfd);
			if (max_fd < Servers[i]->getSocket())
				max_fd = Servers[i]->getSocket();
		}
		for (map <int, Client*> :: iterator i = Clients.begin(); i != Clients.end(); i++) // Add client's fd into set for reading its request
		{
			readFd.insert(readFd.begin(), (*i).first);
			FD_SET((*i).first, &readfd);
			if ((*i).second->toServe())
				FD_SET((*i).first, &writefd);
			if (max_fd < (*i).first)
				max_fd = (*i).first;
		}

		select_res = select(max_fd + 1, &readfd, &writefd, NULL, &timeout);
		if (select_res == -1)
		{
			std::cerr << strerror(errno) << std::endl;
			continue;
		}
		else if (select_res == 0 )
			continue ;

		//#########################################//
		//#########################################//
		//__CHECKING FOR NEW COONECTING_OR DATA___#//
		//#########################################//
		//#########################################//
		for (vector<int> :: iterator start = readFd.begin(); start != readFd.end() && select_res > 0; start++)
		{
			if (FD_ISSET(*start, &readfd)) { //check triggered read fd

				map<int, Client*> :: iterator cl  = Clients.find(*start);
				 if ( cl  != Clients.end()){   //if triggered fd is one of clients fd
					try
				 	{
						if ((*cl).second->readRequest())
							(*cl).second->response();  //if we got all his request then we start to prepare his response
						else if ((*cl).second->isClosed()){ //if client closes his connection we delete him from map
							Clients.erase(cl);
							delete (*cl).second;
						}
						select_res--;
				 	}
				 	catch(const std::exception& e)
				 	{
						std::cerr << e.what() << '\n';
				 	}
				 }
				 else {   //else if triggered fd is not clients we accept new connection
					for (size_t i = 0; i < Servers.size(); i++) {
						if (*start == Servers[i]->getSocket())
						{
							std::map <string, Location> const &loc = Servers[i]->getSettings().locations;
							Client *cl =  new Client(Servers[i]->getSocket(), loc);
							Clients.insert(std::pair<int, Client* >(cl->getSocket(), cl));
							select_res--;
						}
					}
				 }
			}
		}

		//#########################################//
		//#########################################//
		//__CHECKING FOR CLIENTS RESPPONSE READY__#//
		//#########################################//
		//#########################################//
		for (map<int, Client*> :: iterator i = Clients.begin(); Clients.size() && i != Clients.end(); i++)
		{
			try
			{
				if (FD_ISSET((*i).first, &writefd) &&  (*i).second->toServe())
					(*i).second->response();
				else if ((*i).second->isClosed()){ //if client closes his connection we delete him from map
						Clients.erase(i);
					delete (*i).second;
					i = Clients.begin();
				}

			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
	}

}

#endif
