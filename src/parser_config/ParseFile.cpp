#include "../../includes/MainIncludes.hpp"

//===============PRINT_RES================
void	printLocations(std::multimap<std::string, Location> locations) {
	std::cout << "\ncount LOCATION " << locations.size() << std::endl;
	for (map<string, Location>::iterator  i = locations.begin(); i != locations.end() ; ++i) {
		std::cout << "\n";
		std::cout << "Location: " << i->first << std::endl;
		std::cout << "ROOT\t" << i->second.getRoot() << std::endl;
		std::cout << "CGI\t\t" << std::endl;
		for (map<string, string>::iterator j = i->second.cgi.begin(); j != i->second.cgi.end() ; ++j) {
			std::cout << j->first << " -> " << j->second << std::endl;
		}
		std::cout << "INDEX\t" << i->second.getIndex() << std::endl;
		std::cout << "AUTOI\t" << i->second.getAutoIndex() << std::endl;
		std::cout << "LIMIT\t" << i->second.getLimit() << std::endl;
		std::cout << "RETURN\t" << i->second.getRedire().first << "<=>" << i->second.getRedire().second << std::endl;
		std::map<std::string, bool> tmpMap = i->second.methods;
		for (map<string, bool>::iterator j = tmpMap.begin(); j != tmpMap.end() ; ++j) {
			std::cout << "\t\t" << j->first << " -> " << j->second << std::endl;
		}
	}
}

void	printServ(t_server serv) {
	std::cout << "====SERVER_SETTING====" << std::endl;
	std::cout << serv.name << std::endl;
	std::cout << serv.host << std::endl;
	std::cout << serv.port << std::endl;
	std::cout << serv.logFile << std::endl;
	std::map<int, std::string> tmpMap = serv.errorPages;
	for (map<int, std::string> :: iterator i  = tmpMap.begin(); i != tmpMap.end() ; ++i) {
		std::cout << i->first << " -> " << i->second << std::endl;
	}
	printLocations(serv.locations);
}

void	printServers(std::vector<t_server> servers) {
	for (vector<t_server> :: iterator i = servers.begin(); i != servers.end() ; ++i) {
		printServ(*i);
	}
}

//============INIT_STRUCTS=======
void initServer(t_server &t) {
	t.name = "";
	t.host = "";
	t.port = 0;
	t.logFile = "";
}

void	initLocation(Location &l) {
	l.root = "";
	std::multimap<std::string, std::string> tmpMap = l.cgi;
	for (map<std::string, std::string> :: iterator i  = tmpMap.begin(); i != tmpMap.end() ; ++i) {
		std::cout << i->first << " -> " << i->second << std::endl;
	}
	l.redirect = std::pair<int, std::string>(0, "");
	l.index = "";
	l.autoindex = "off";
	l.limit_client_body_size = 0;
	l.methods["GET"] = true;
	l.methods["POST"] = true;
	l.methods["DELETE"] = true;
	l.methods["PUT"] = true;
	l.methods["HEAD"] = true;
}

//============UTILS==============

void	trimSpaces(std::string &value) {
	value.erase(value.begin(),
				std::find_if(value.begin(), value.end(),
							 std::bind1st(std::not_equal_to<char>(), ' ')));
	value.erase(value.begin(),
				std::find_if(value.begin(), value.end(),
							 std::bind1st(std::not_equal_to<char>(), '\t')));
}

size_t	findNth(const std::string & str , unsigned int N, const std::string & find) {
	if (0 == N) { return std::string::npos; }
	size_t pos, from = 0;
	unsigned i = 0;
	while (i < N) {
		pos = str.find(find, from);
		if (std::string::npos == pos) { break; }
		from = pos + 1;
		++i;
	}
	return pos;
}

std::string key(std::string &str, char delim) {
	return str.substr(0, str.find(delim));
}

std::string value(std::string &str, char delim) {
	return str.substr(str.find(delim) + 1);
}

void	mapingErrorPage(std::map<int, std::string> &ep, std::string &str, char delim) {
	ep.insert(std::pair<int, std::string>(atoi(key(str, delim).c_str()), value(str, delim)));
}

void	mapingCGI(std::multimap<std::string, std::string> &ep, std::string &str, char delim) {
	ep.insert(std::pair<std::string, std::string>(key(str, delim).c_str(), value(str, delim)));
}

void	mapingMethods(std::map<std::string, bool> &methods, std::string &str) {
	if (str.find("GET") == std::string::npos) {
		methods["GET"] = false;
	}
	if (str.find("POST") == std::string::npos) {
		methods["POST"] = false;
	}
	if (str.find("DELETE") == std::string::npos) {
		methods["DELETE"] = false;
	}
	if (str.find("PUT") == std::string::npos) {
		methods["PUT"] = false;
	}
	if (str.find("HEAD") == std::string::npos) {
		methods["HEAD"] = false;
	}

}

void	getValueLocation(std::string & str, Location & loc) {
	if (str.find("location:") != std::string::npos) {
		std::string value = str.substr(str.find("location:") + strlen("location:"));
		trimSpaces(value);
		loc.path = value;
	}
	if (str.find("limits_client_body_size:") != std::string::npos) {
		std::string value = str.substr(str.find("limits_client_body_size:") + strlen("limits_client_body_size:"));
		trimSpaces(value);
		loc.limit_client_body_size = atoi(value.c_str());
	}
	if (str.find("autoindex:") != std::string::npos) {
		std::string value = str.substr(str.find("autoindex:") + strlen("autoindex:"));
		trimSpaces(value);
		loc.autoindex = value;
	}
	if (str.find("index:") != std::string::npos) {
		std::string value = str.substr(str.find("index:") + strlen("index:"));
		trimSpaces(value);
		loc.index = value;
	}
	if (str.find("root:") != std::string::npos) {
		std::string value = str.substr(str.find("root:") + strlen("root:"));
		trimSpaces(value);
		loc.root = value;
	}
	if (str.find("path_cgi:") != std::string::npos) {
		str.erase(0, str.find(' ') + 1);
		while(str.length() != 0) {
			trimSpaces(str);
			std::string subString = str.substr(0, str.find(' '));
			str.erase(0, str.find(' '));
			trimSpaces(str);
			subString = subString + "=" + str;
			mapingCGI(loc.cgi, subString, '=');
			str  = "";
		}
	}
	if (str.find("allow_methods:") != std::string::npos) {
		str.erase(0, str.find(' ') + 1);
		mapingMethods(loc.methods, str);
	}
	if (str.find("return:") != std::string::npos) {
		std::string value = str.substr(str.find("return:") + strlen("return:"));
		trimSpaces(value);
		std::string extn = value.substr(0, value.find(' '));
		std::string path = value.substr(value.find(extn) + extn.length());
		trimSpaces(path);
		loc.redirect = std::pair<int, std::string>(atoi(extn.c_str()), path);
	}
}

void	eraseValueForLocation(std::string &settingLoc, std::multimap<std::string, Location> &locs) {
	Location l;
	initLocation(l);
	
	std::string pathLoc;
	
	settingLoc += '\n';
	while (settingLoc.length() != 0) {
		std::string str = settingLoc.substr(0, settingLoc.find('\n'));
		getValueLocation(str, l);
		settingLoc.erase(0, settingLoc.find('\n') + 1);
	}
	locs.insert(std::pair<std::string, Location>(l.path, l));
}

void eraseValueForServer(std::string &str, t_server & server) {
	if (str.find("host:") != std::string::npos) {
		std::string value = str.substr(str.find("host:") + strlen("host:"));
		trimSpaces(value);
		server.host = value;
	}
	if (str.find("server_name:") != std::string::npos) {
		std::string value = str.substr(str.find("server_name:") + strlen("server_name:"));
		trimSpaces(value);
		server.name = value;
	}
	if (str.find("port:") != std::string::npos) {
		std::string value = str.substr(str.find("port:") + strlen("port:"));
		trimSpaces(value);
		server.port = atoi(value.c_str());
	}
	if (str.find("error_page:") != std::string::npos) {
		str.erase(0, str.find(' ') + 1);
		std::map<int, std::string> errorPage;
		while(str.length() != 0) {
			trimSpaces(str);
			std::string subString = str.substr(0, str.find(' '));
			str.erase(0, str.find(' '));
			mapingErrorPage(errorPage, subString, '=');
		}
		server.errorPages = errorPage;
	}
	if (str.find("log_file_path:") != std::string::npos) {
		std::string value = str.substr(str.find("log_file_path:") + strlen("log_file_path:"));
		trimSpaces(value);
		server.logFile = value;
		std::fstream openFile(value, std::ios::app);
//		todo: проверка ошибки на создание
		openFile.is_open();
	}
}

//==========PARSER=================
std::multimap<std::string, Location>	parsLocation(std::string &blockLoc) {
	std::multimap<std::string, Location> locs;
	std::string settingLocs = blockLoc.substr(0, blockLoc.find("server:"));
	blockLoc.erase(0, settingLocs.length());
	while (settingLocs.length() != 0) {
		std::string str = settingLocs.substr(0, settingLocs.find("\n\n"));
		settingLocs.erase(0, str.length() + 1);
		eraseValueForLocation(str, locs);
		if (settingLocs == "\n")
			break;
	}
	return locs;
}

void	parsServer(t_server & server, std::string &blockBuf) {
	std::string strServ = "server:\n";
	std::string settingServ;
	
	blockBuf.erase(0, strServ.length());
	settingServ = blockBuf.substr(0, blockBuf.find("\n\n"));
	settingServ	+= "\n";
	blockBuf.erase(0, settingServ.length() + 1);
	while (settingServ.length() != 0) {
		std::string str = settingServ.substr(0, settingServ.find('\n'));
		eraseValueForServer(str, server);
		settingServ.erase(0, settingServ.find('\n') + 1);
	}
	server.locations = parsLocation(blockBuf);
}

t_server parsBuf(std::string &buf) {
	t_server server;
	initServer(server);
	
	std::string	blockServer;
	size_t posSecondServ;
	if ((posSecondServ = findNth(buf, 2, "server:"))) {
		blockServer = buf.substr(0, posSecondServ);
		parsServer(server, blockServer);
		buf.erase(0, posSecondServ);
	}
	else {
		blockServer = buf.substr(0, buf.length());
		buf.erase(0, buf.length());
	}
	return (server);
}
int	startParser(char *fileName, std::vector<t_server> &servers) {
	std::ifstream	parsFile(fileName);
	std::string		buf;
	std::string		bufFile;

	if (parsFile.is_open()) {
		std::cout << "START\tpars" << std::endl;
		while (std::getline(parsFile, buf)) {
			bufFile += buf + "\n";
		}
		while (bufFile.find("server:") != std::string::npos) {
			t_server serv = parsBuf(bufFile);
			servers.push_back(serv);
		}
	}
	else if (!parsFile.is_open()) {
		return (1);
	}
	parsFile.close();
	return (0);
}
