//
// Created by Retro Joella on 11/14/21.
//

#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP

#include "MainIncludes.hpp"

class Server {

private:
	std::string						_name;
	std::string						_host;
	int								_port;
	std::map<int, std::string>		_errorPages;
	std::map<std::string, Location>	_locations;
	std::string 					_logFile;
	t_server						_serverSettings;
	
	
	
public:
	Server();
	Server(t_server s);
	~Server();
};

#endif //WEBSERV_SERVER_HPP
