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
#include <cstdint>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sstream>

#include "Location.hpp"
#include "SettingsServer.hpp"
#include "RequestBuffer.hpp"
#include "Server.hpp"
#include "Client.hpp"

//#include "ParseFile.hpp"
int		startParser(char *fileName, std::vector<t_server> &servers);
void	printLocations(std::map<std::string, Location> locations);
void	printServ(t_server serv);
void	Start(vector<Server*> Servers);

#endif //WEBSERV_MAININCLUDES_HPP
