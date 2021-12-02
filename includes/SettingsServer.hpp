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

class ErrorException : public std::exception {

public :

	ErrorException(const char *msg) : errorMsg(msg), status(0) {}
	ErrorException(int st, const char *msg) : errorMsg(msg), status(st) {}
	int getStatus() const {return status;}

	virtual const char* what(void) const throw () {
		return (this->errorMsg);
	}

private:

	const char*	errorMsg;
	const int	status;

};


#endif //WEBSERV_SETTINGSSERVER_HPP
