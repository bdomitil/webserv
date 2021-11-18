#include "includes/MainIncludes.hpp"

int main(int argc, char *argv[]) {

	std::vector<t_server>	structServers;

	if (argc == 2) {
		if (!startParser(argv[1], structServers)) {
			std::vector<Server*> Servers;
			for (size_t i = 0; i < structServers.size(); ++i)
				Servers.push_back(new Server(structServers[i]));
			Start(Servers);
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
