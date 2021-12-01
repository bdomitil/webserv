//
// Created by Retro Joella on 11/14/21.
//

#ifndef WEBSERV_SETTINGSSERVER_HPP
#define WEBSERV_SETTINGSSERVER_HPP

#include "MainIncludes.hpp"

typedef struct	s_server {
	std::string						name;
	std::string						host;
	int								port;
	std::string 					logFile;
	std::map<int, std::string>		errorPages;
	std::map<std::string, Location>	locations;
}	t_server;

#endif //WEBSERV_SETTINGSSERVER_HPP
