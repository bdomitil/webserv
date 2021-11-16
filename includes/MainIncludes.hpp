//
// Created by Retro Joella on 11/14/21.
//

#ifndef WEBSERV_MAININCLUDES_HPP
# define WEBSERV_MAININCLUDES_HPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <fstream>

#include "Location.hpp"
#include "SettingsServer.hpp"
#include "Server.hpp"
#include "Client.hpp"

//#include "ParseFile.hpp"
int		startParser(char *fileName, std::vector<t_server> &servers);
void	printLocations(std::map<std::string, Location> locations);
void	printServ(t_server serv);

#endif //WEBSERV_MAININCLUDES_HPP
