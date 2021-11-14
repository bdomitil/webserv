#include "includes/MainIncludes.hpp"


int main(int ac, char **av) {
	std::vector<t_server> structServers;
	if (ac == 2) {
		if (!startParser(av[1], structServers)){
			std::vector<Server*> Servers;
						for (auto i = structServers.begin(); i != structServers.end() ; ++i) {
							printServ(*i);
						}
			for (size_t i = 0; i < structServers.size(); ++i) {
//				TODO: добавить конструктор в Server
//				Servers.push_back(new Server(structServers[i]));
			}
			
		}
		else {
			std::cout << "ERROR parse config" << std::endl;
			return (1);
		}
	}
	else {
		std::cout << "wrong arguments" << std::endl;
		return (1);
	}
	return (0);
}
