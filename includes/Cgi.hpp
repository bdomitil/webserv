#ifndef WEBSERV_CGI_HPP
#define WEBSERV_CGI_HPP
#include "MainIncludes.hpp"

class Cgi {

public:
		uint32_t editResponce(std::stringstream &st);
		Cgi(Request &_request, const std::map<std::string, std::string> &cgiPath);
		const char ***makeData_for_exec(std::string &path, std::string &addHeaders);
private:
		Cgi();
		Request &_request;
		void free_execData(const char ***execData);
		const std::map<std::string, std::string> _cgiPath;
		std::map <std::string, std::string> _reqHeaders;
};


#endif