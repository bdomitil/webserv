#ifndef WEBSERV_CGI_HPP
#define WEBSERV_CGI_HPP
#include "MainIncludes.hpp"

class Cgi {

public:
		uint32_t editResponce(std::stringstream &st);
		Cgi(Request &_request,  std::map<std::string, std::string> &cgiPath , std::string &method);
private:
		Cgi();
		Request &_request;
		std::map<std::string, std::string> _cgiPath;
		std::string _method;
};


#endif