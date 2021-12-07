#include "../../includes/MainIncludes.hpp"

Server :: Server(const t_server &ServerSetting) {
	_name = ServerSetting.name;
	_host = ServerSetting.host;
	_port = ServerSetting.port;
	_locations = ServerSetting.locations;
	_errorPages = ServerSetting.errorPages;
	_serverSettings = t_server(ServerSetting);
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
	srsocket = socket(PF_INET, SOCK_STREAM, 0);
	if (srsocket == -1) {
		std::cerr << "Unable to create socket" << std::endl;
		throw(this);
	}
	if (setsockopt(srsocket ,SOL_SOCKET ,SO_REUSEADDR , &reuseaddr,sizeof(int)) == -1) {
		std::cerr << "Error setting sockopt" << std::endl;
		throw(this);
	}
	if (fcntl(srsocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "Error using fcntl" << std::endl;
		throw(this);
	}
	return (srsocket);
}



void Server :: Run(void) {
	_fdSock =  createSocket();
	if (bind(_fdSock, (struct sockaddr*)&_sockaddr, sizeof(_sockaddr)) == -1) {
		std::cerr << "Error binding socket : " << strerror(errno) << std::endl;
		throw (this);
	}
	if (listen(_fdSock, 100) == -1) {
		std::cerr << "Error listening socket : " << strerror(errno) << std::endl;
		throw(this);
	}
}



void Start(vector<Server*> Serverss)
{
	fd_set readfd, writefd;
	vector<int> readFd, writeFd;
	map <int,Client*>Clients;
	map <int,Server*>Servers;

	for (size_t i = 0; i < Serverss.size(); i++) {
		try
		{
			Serverss[i]->Run();
			Servers.insert(pair<int, Server*> (Serverss[i]->getSocket(), Serverss[i]));
		}
		catch (Server *serv)
		{
			vector<Server*>::iterator i = Serverss.begin();
			while (*i != serv)
				i++;
			Serverss.erase(i);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}

	while ( !Servers.empty() ) {
		int max_fd, select_res = 0;
		t_time timeout;

		timeout.tv_usec = 50000;
		timeout.tv_sec = 2;
		FD_ZERO(&writefd);
		FD_ZERO(&readfd);
		readFd.clear();
		writeFd.clear();
		for (map<int, Server*>::iterator i = Servers.begin(); i != Servers.end(); i++) {
			readFd.insert(readFd.end(), i->first );
			FD_SET(i->first, &readfd);
			if (max_fd < i->first)
				max_fd = i->first;
		}
		for (map <int, Client*> :: iterator i = Clients.begin(); i != Clients.end();) {
			if (!(i->second->isClosed = i->second->SessionIsOver())){
				readFd.insert(readFd.begin(), i->first);
				FD_SET(i->first, &readfd);
			}
			else if (i->second->isClosed){
					Clients.erase(i);
					delete i->second;
					i = Clients.begin();
					continue ;
			}
			if (i->second->toServe())
				FD_SET(i->first, &writefd);
			if (max_fd < i->first)
				max_fd = i->first;
			i++;
		}
		select_res = select(max_fd + 1, &readfd, &writefd, NULL, &timeout);
		if (select_res == -1) {
			std::cerr << strerror(errno) << std::endl;
			continue;
		}
		else if (select_res == 0 )
			continue ;
		for (map<int, Client*> :: iterator i = Clients.begin(); Clients.size() && i != Clients.end(); i++) {
			try
			{
				if (FD_ISSET(i->first, &writefd) &&  i->second->toServe()){
					i->second->response(Servers[i->second->getSrvSocket()]->getErrorPages());
					select_res--;
				}
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		for (vector<int> :: iterator start = readFd.begin(); start != readFd.end() && select_res > 0; start++)
		{
			if (FD_ISSET(*start, &readfd)) {
				map<int, Client*> :: iterator cl  = Clients.find(*start);
				 if ( cl  != Clients.end()){
					try
				 	{
						cl->second->readRequest();
						select_res--;
				 	}
				 	catch(const std::exception& e)
				 	{
						std::cerr << e.what() << '\n';
				 	}
				 }
				 else {
					for (map<int, Server*>::iterator i = Servers.begin(); i != Servers.end(); i++) {
						if (*start == i->first) {
							std::multimap <std::string, Location> const &loc = i->second->getSettings().locations;
							Client *cl =  new Client(i->second->getSocket(), loc);
							Clients.insert(std::pair<int, Client* >(cl->getSocket(), cl));
							select_res--;
						}
					}
				 }
			}
		}
	}
}
