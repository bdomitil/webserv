//
// Created by Retro Joella on 11/14/21.
//

#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP


using namespace std;
typedef struct sockaddr_in t_sockaddr_in;
#include "MainIncludes.hpp"
#include <arpa/inet.h>
#include <errno.h>
#include <string>
#include <fcntl.h>

class Server {

private:
	std::string						_name;
	std::string						_host;
	int								_port;
	std::map<int, std::string>		_errorPages;
	std::map<std::string, Location>	_locations;
	t_server						_serverSettings;
	t_sockaddr_in					_sockaddr;
	int 							_fdSock;
	string							_logfile;
	Server();
	void	throwException(string msg)const ;
	int		createSocket(void);

public:
	Server(const t_server &ServSetting);
	t_server getSettings(void) const;
	Server(const Server &copy);
	~Server();
	Server& operator= (const Server &second);
	void Run(void);
	t_sockaddr_in& getSockAddr(void) {return (_sockaddr);};

	class SrvErrorExpeption : public std::exception {
		public :
			const char* errorMsg;
			virtual const char* what(void) const throw (){return (this->errorMsg);};
	};
};

#endif //WEBSERV_SERVER_HPP
