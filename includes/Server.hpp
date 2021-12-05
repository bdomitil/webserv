//
// Created by Retro Joella on 11/14/21.
//

#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP
#define RECV_BUFFER_SIZE	2048
#define SEND_BUFFER_SIZZ	2046

using namespace std;
#include "MainIncludes.hpp"

typedef struct sockaddr_in t_sockaddr_in;
typedef struct timeval t_time;
class Server {

public:
	Server(const t_server &ServSetting);
	Server(const Server &copy);
	Server& operator=	(const Server &second);
	~Server();

private:

	Server();

	std::string						_name;
	std::string						_host;
	int								_port;
	std::map<int, std::string>		_errorPages;
	std::multimap<std::string, Location>	_locations;
	std::string 					_logFile;
	t_server						_serverSettings;
	t_sockaddr_in					_sockaddr;
	int 							_fdSock;
	string							_logfile;
	int								createSocket(void);

public:

	t_server const					&getSettings(void) const;
	t_sockaddr_in&					getSockAddr(void) 			{return (_sockaddr);};
	int								getSocket(void)				{return (_fdSock);}
	std::map<int, std::string>& 	getErrorPages(void)					{return _errorPages;}
	void							throwException(string msg)const;
	void							Run(void);
};

#endif //WEBSERV_SERVER_HPP
