//
// Created by Retro Joella on 11/14/21.
//

#ifndef WEBSERV_MAININCLUDES_HPP
# define WEBSERV_MAININCLUDES_HPP

#define RECV_BUFFER_SIZE 2048
#define SEND_BUFFER_SIZZ 2048

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
#include <stdio.h>

#include "Response.hpp"
#include "Request.hpp"
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
std::string	ft_itoa(int x);

#endif //WEBSERV_MAININCLUDES_HPP
