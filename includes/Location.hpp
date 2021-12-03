//
// Created by Retro Joella on 11/14/21.
//

#ifndef WEBSERV_LOCATION_HPP
#define WEBSERV_LOCATION_HPP

#include "MainIncludes.hpp"

struct Location {
	std::string							path;
	std::map<std::string, bool>			methods;
	std::string							root;
	std::multimap<std::string, std::string> 	cgi;
	std::string							index;
	std::string							autoindex;
	int									limit_client_body_size;
	std::pair<int, std::string>			redirect;

	std::string	getRoot() const {
		return root;
	}
	const std::multimap<std::string, std::string>&	getCgi() const {
		return cgi;
	}
	std::string	getIndex() const {
		return index;
	}
	std::string	getAutoIndex() const {
		return autoindex;
	}
	int	getLimit() const {
		return limit_client_body_size;
	}
	std::map<std::string, bool>	getMethods() const {
		return methods;
	}
	const std::pair<int, std::string>&	getRedire() const {
		return redirect;
	}
};

#endif //WEBSERV_LOCATION_HPP
