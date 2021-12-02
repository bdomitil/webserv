#include "../../includes/MainIncludes.hpp"

Cgi :: Cgi(Request &request,const  std::map<std::string, std::string> &cgiPath) : _request(request), _cgiPath(cgiPath){
	std::string target;
	std::string root = _request.getLocation()->getRoot();
	std::string url = _request.getUrl(target);

	url = url.substr(root.size(), std::string::npos);
	url = url.substr(0, url.size() - target.size());
	if (root[root.size() - 1] == '/')
		root.erase(root.size() -1);
	_reqHeaders = _request.getHeaders();
	_reqHeaders.insert(std::pair<string, string>("QUERY_STRING", _request.getQueryString()));
	_reqHeaders.insert(std::pair<string, string>("REQUEST_METHOD", _request.getMethod()));
	_reqHeaders.insert(std::pair<string, string>("PATH_INFO", url));
	_reqHeaders.insert(std::pair<string, string>("PATH_TRANSLATED", root + url));
}



const char*** Cgi :: makeData_for_exec(std::string &path, std::string &addHeaders){
	const char **env = new const char*[_reqHeaders.size() + 1];
	const char **args = new const char*[2];
	const char ***to_ret =  new const char**[2];

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
