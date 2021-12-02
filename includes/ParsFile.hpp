//
// Created by Retro Joella on 11/14/21.
//

#ifndef WEBSERV_PARSFILE_HPP
#define WEBSERV_PARSFILE_HPP

#include "MainIncludes.hpp"

int		startParser(char *fileName, std::vector<t_server> &servers);
void	printLocations(std::map<std::string, Location> locations);
void	printServ(t_server serv);
void	printServers(std::vector<t_server> servers);

#endif //WEBSERV_PARSFILE_HPP
