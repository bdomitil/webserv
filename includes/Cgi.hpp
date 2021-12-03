#ifndef WEBSERV_CGI_HPP
#define WEBSERV_CGI_HPP
#include "MainIncludes.hpp"
#define INPUT 0
#define OUTPUT 1

class Cgi {

public:
		int* editResponce(uint64_t &contentLen, std::string &contentType, int cgNum);
		Cgi(Request &_request, const std::multimap<std::string, std::string> &cgis, std::ifstream &FILE);
		char *const **makeData_for_exec(std::string &path, std::map <std::string, std::string> &headers);
		~Cgi();
private:
		Cgi();
		Request &_request;
		void free_execData(const char ***execData);
		std::multimap<std::string, std::string> _cgis;
		std::map <std::string, std::string> _reqHeaders;
		std::string		_url;
		std::string 	_fileExt;
		std::ifstream 	&_FILE;
		pid_t			*_pids;
		int				_cgiFd[2];
		int				_cgNum;
};


#endif