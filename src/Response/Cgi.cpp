#include "../../includes/MainIncludes.hpp"

Cgi :: Cgi(Request &request,const  std::multimap<std::string, std::string> &cgis, std::ifstream &FILE) : _request(request), _cgis(cgis), _FILE(FILE){
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
	_reqHeaders.insert(std::pair<string, string>("PATH_INFO", url));
	_reqHeaders.insert(std::pair<string, string>("PATH_TRANSLATED", root + url));
	_reqHeaders.insert(std::pair<string, string>("SERVER_PROTOCOL", "HTTP/1.1"));
}



char *const ** Cgi :: makeData_for_exec(std::string &path, std::map <std::string, std::string> &headers){
	char **env = new char*[_reqHeaders.size() + 1];
	char **args = new char*[2];
	char *const **to_ret =  new  char*const*[2];

	env[_reqHeaders.size()] = nullptr;
	args[1] = nullptr;
	args[0] = strdup(path.c_str());
	std::map <std::string, std::string> :: iterator i = _reqHeaders.begin();
	for (int j = 0; i != _reqHeaders.end(); i++, j++){
		env[j] = strdup((i->first + "=" + i->second).c_str());
	}
	to_ret[0] = args;
	to_ret[1] = env;
	return (to_ret);
}

void Cgi :: free_execData(const char ***execData){
	delete execData[0][0];
	delete execData[0][1];
	delete execData[0];
	for (int i = 0; execData[1][i] != nullptr; i++){
		delete execData[1][i];
	}
	delete execData[1];
	delete execData;
}

Cgi :: ~Cgi(){
	delete[] _pids;
	for (int i = 0; i < _cgNum; i++)
		kill(_pids[i], SIGTERM);
}


int*  Cgi:: editResponce(uint64_t &contLen, string &contType, int cgNum){

	pid_t pid = 1;
	_cgNum = cgNum;
	int *pipes[cgNum];
	std::multimap<std::string, std::string> :: iterator i = _cgis.begin();
	pid_t helper;
	int j = 0;
	
	if (pipe(_cgiFd) == -1)
		throw ErrorException("PIPE ERROR");
	for (int k = 0; k < cgNum; k++){
		pipes[k] = new int[2];
		if (pipe(pipes[k]) == -1)
			throw ErrorException("PIPE ERROR");
	}
	helper = fork();
	_pids = new pid_t[cgNum];
	if (helper == 0){
		alarm(2); //TODO delete this time after tests
		while (i != _cgis.end() && j != cgNum){
		int res;
			if ((_pids[j] = fork()) == -1)
				throw ErrorException("FORK ERROR");
			if (_pids[j] == 0){
				if (j == 0){
					res = dup2(_cgiFd[INPUT], INPUT);
					close(pipes[j][INPUT]);
					std::cerr << _cgiFd[INPUT]  << " <- " << INPUT << std::endl;
				}
				else{
					res = dup2(pipes[j][INPUT], INPUT);
					std::cerr << pipes[j][INPUT]  << " <- " << INPUT << std::endl;
				}
				if (j + 1 == cgNum) {
					res = dup2(_cgiFd[OUTPUT], OUTPUT);
					close(pipes[j][OUTPUT]);
					std::cerr << _cgiFd[OUTPUT]  << " <- " << OUTPUT << std::endl;
				}
				else{
					res = dup2(pipes[j][OUTPUT], pipes[j + 1][INPUT]);
					std::cerr << pipes[j][OUTPUT]  << " <- " << pipes[j + 1][INPUT] << std::endl;
				}
				if (res == -1)
					throw ErrorException("CHILD DUP2 ERROR");
				char *const **execData = makeData_for_exec(_url, _reqHeaders);
				if (execve(i->second.c_str() , execData[0],execData[1]) == 1)
					throw ErrorException("CHILD EXECVE ERROR");
			}
			else {
				usleep(50);
				j++;
				i++;
			}
		}
		exit(0);
	}
	else
		waitpid(helper, 0, 0);
	return _cgiFd;
}
