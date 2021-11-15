//
// Created by Retro Joella on 11/15/21.
//

#include "../../includes/MainIncludes.hpp"

Server::Server() {

}

Server::Server(t_server s) {
	_name = s.name;
	_host = s.host;
	_port = s.port;
	_errorPages = s.errorPages;
	_locations = s.locations;
	_logFile = s.logFile;
	_serverSettings = s;
}

Server::~Server() {

}