#include "../../includes/MainIncludes.hpp"

Cgi :: Cgi(Request &request, const std::multimap<std::string, std::string> &cgis, std::ifstream &FILE)
: _request(request), _cgis(cgis), _FILE(FILE) {
	std::string target;
	std::string root = _request.getLocation()->getRoot();
	std::string url = _request.getUrl(target);

 // TODO stlip request's body and add to _reqHeaders
	_url = url;
	_fileExt = "." + _url.substr(_url.find_last_of('.') + 1);
	url = url.substr(root.size());
	url = url.substr(0, url.size() - target.size());
	if (root[root.size() - 1] == '/')
		root.erase(root.size() -1);
	_reqHeaders = _request.getHeaders();
	_reqHeaders.insert(std::pair<string, string>("QUERY_STRING", _request.getQueryString()));
	_reqHeaders.insert(std::pair<string, string>("REQUEST_METHOD", _request.getMethod()));
	_reqHeaders.insert(std::pair<string, string>("PATH_INFO", _url));
	_reqHeaders.insert(std::pair<string, string>("PATH_TRANSLATED", root + _url));
	_reqHeaders.insert(std::pair<string, string>("SERVER_PROTOCOL", "HTTP/1.1"));
	return;
}

std::vector<char **>	Cgi::makeDataForExec(std::string &path,
											std::map <std::string, std::string> &headers) {
	char **env = new char*[_reqHeaders.size() + 1];
	char **args = new char*[2];
	std::vector<char **>	to_ret;

	env[_reqHeaders.size()] = nullptr;
	args[1] = nullptr;
	args[0] = strdup(path.c_str());
	std::map <std::string, std::string>::iterator i = _reqHeaders.begin();
	for (int j = 0; i != _reqHeaders.end(); i++, j++)
		env[j] = strdup((i->first + "=" + i->second).c_str());
	to_ret.push_back(args);
	to_ret.push_back(env);
	return to_ret;
}

void	Cgi::free_execData(const char ***execData) {
	delete execData[0][0];
	delete execData[0][1];
	delete execData[0];

	for (int i = 0; execData[1][i] != nullptr; i++)
		delete execData[1][i];

	delete execData[1];
	delete execData;
}

void	Cgi::runCgi(std::string cgiPath) {

	std::vector<char **>	execveArgs;

	execveArgs = makeDataForExec(cgiPath, _reqHeaders);
	execve(execveArgs[0][0], execveArgs[0], execveArgs[1]);
	exit(EXIT_FAILURE);
}

void	Cgi::changeAndCloseFd(int pos, int cgiNum) {

	if (!pos) {
		if (dup2(_mainFds[0], STDIN_FILENO) == -1){
			exit(EXIT_FAILURE);
		}
	}
	if (pos > 0) {
		if (dup2(_pipeFds[pos - 1][0], STDIN_FILENO) == -1)
			exit(EXIT_FAILURE);
		close(_pipeFds[pos - 1][0]), close(_pipeFds[pos - 1][1]);
	}
	if (pos + 1 < cgiNum) {
		if (dup2(_pipeFds[pos][1], STDOUT_FILENO) == -1)
			exit(EXIT_FAILURE);
		close(_pipeFds[pos][0]), close(_pipeFds[pos][1]);
	}
	if (pos + 1 == cgiNum) {
		if (dup2(_mainFds[1], STDOUT_FILENO) == -1)
			exit(EXIT_FAILURE);
	}
	close(_mainFds[0]), close(_mainFds[1]);
	return;
}

void	Cgi::runCGIHelper(int *firstReadFromFD,
						int *lastSendToFD, int cgiNum) {

	int		iter;
	pid_t	cgiPids[cgiNum];
	int		status;
	int		ret;

	ret = 0;
	_pipeFds = new int[cgiNum][2];

	_mainFds[0] = firstReadFromFD[0];
	_mainFds[1] = lastSendToFD[1];

	std::multimap<std::string, std::string> :: iterator i = _cgis.begin();
	iter = -1;
	while (++iter < cgiNum && i != _cgis.end()) {
		if (iter + 1 < cgiNum) {
			if (pipe(_pipeFds[iter]) == -1)
				exit(EXIT_FAILURE);
		}
		cgiPids[iter] = fork();
		if (cgiPids[iter] == -1) {
			killChilds(cgiPids, iter);
			exit(EXIT_FAILURE);
		}
		if (!cgiPids[iter]) {
			changeAndCloseFd(iter, cgiNum);
			close(firstReadFromFD[1]);
			close(lastSendToFD[0]);
			runCgi(i->second);
		}
		if (iter)
			close(_pipeFds[iter - 1][0]), close(_pipeFds[iter - 1][1]);
		i++;
	}
	close(firstReadFromFD[0]);
	close(firstReadFromFD[1]);
	close(lastSendToFD[0]);
	close(lastSendToFD[1]);


	for (int i = 0; i < cgiNum; i++) {
		waitpid(cgiPids[i], &status, 0);
		std::cout << "PROC #" << i << " EXITED WITH CODE " << status << std::endl;
		if (status != 0)
			ret = status;
	}
	if (!ret)
		exit(EXIT_SUCCESS);
	exit(EXIT_FAILURE);
}

/*	cgiFds[0] - fd to read data from the last cgi script
	cgiFds[1] - fd to send data to the first cgi script	*/
int	*Cgi::initCGI(int cgiNum) {

	int	*cgiFds;
	int	mainFD[2][2];

	if (cgiNum < 1)
		throw ErrorException(502, "cgiNum incorrect");
	if (pipe(mainFD[0]) == -1 or pipe(mainFD[1]) == -1)
		throw ErrorException(502, "pipes");
	_cgiHelperPid = fork();
	if (_cgiHelperPid == -1)
		throw ErrorException(502, "fork");
	if (!_cgiHelperPid)
		runCGIHelper(mainFD[0], mainFD[1], cgiNum);

	close(mainFD[0][0]);
	close(mainFD[1][1]);

	cgiFds = new int[2];
	cgiFds[1] = mainFD[0][1];
	cgiFds[0] = mainFD[1][0];

	return cgiFds;
}
