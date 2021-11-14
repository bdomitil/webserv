//
// Created by Retro Joella on 11/14/21.
//

#ifndef WEBSERV_LOCATION_HPP
#define WEBSERV_LOCATION_HPP

#include "MainIncludes.hpp"

struct Location {
	std::string					path;
	std::map<std::string, bool>	methods;
	std::string					root;
	std::pair<std::string, std::string> cgi;
	std::string					index;
	std::string					autoindex;
	int							limit_client_body_size;
	std::pair<int, std::string>	redirect;
//	void reset(){
//		methods = std::map<std::string, bool>();
//		root = std::string();
//		cgi = std::pair<std::string, std::string>();
//		index = std::string();
//		autoindex = std::string();
//	}
	
	std::string									getRoot() { return root; }
	const std::pair<std::string, std::string>&	getCgi() { return cgi; }
	std::string									getIndex() { return index; }
	std::string									getAutoIndex() { return autoindex; }
	int											getLimit() { return limit_client_body_size; }
	std::map<std::string, bool>					getMethods() { return methods; }
	const std::pair<int, std::string>&			getRedire() { return redirect; }
};

#endif //WEBSERV_LOCATION_HPP
