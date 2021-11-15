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
	t_server						_serverSettings;
	Server();

public:
	Server(const t_server &ServSetting);
	t_server getSettings(void) const;
	Server(const Server &copy);
	Server& operator= (const Server &second);
};

#endif //WEBSERV_SERVER_HPP
