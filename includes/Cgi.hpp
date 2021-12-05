#ifndef WEBSERV_CGI_HPP
#define WEBSERV_CGI_HPP
#include "MainIncludes.hpp"
#define INPUT 0
#define OUTPUT 1

class Cgi {

public:

	Cgi(Request &_request, const std::multimap<std::string, std::string> &cgis, std::ifstream &FILE);

	int *initCGI(int cgiNum);
	std::vector<char **>	makeDataForExec(std::string &path,
											std::map <std::string,
											std::string> &headers);
	~Cgi() {}

private:

	Cgi();
	void	free_execData(const char ***execData);
	void	changeAndCloseFd(int pos, int cgiNum);
	void	runCgi(void);
	void	runCGIHelper(int firstReadFromFD,
						int lastSendToFD, int cgiNum);

	Request									&_request;
	std::multimap<std::string, std::string>	_cgis;
	std::map <std::string, std::string>		_reqHeaders;
	std::string								_url;
	std::string								_fileExt;
	std::ifstream							&_FILE;
	int										_cgNum;
	pid_t									_cgiHelperPid;
	int										_mainFds[2];
	int										(*_pipeFds)[2];


};


#endif
