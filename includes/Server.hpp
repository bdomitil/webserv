//
// Created by Retro Joella on 11/14/21.
//

#ifndef WEBSERV_SERVER_HPP
#define WEBSERV_SERVER_HPP
#define RECV_BUFFER_SIZE 512
#define SEND_BUFFER_SIZZ 2048

using namespace std;
#include "MainIncludes.hpp"


// TODO make foo for checking if socket is one of servers socket  foo(int socket) { while (Servers){if socket == Servers[i].socket return true} return false}
typedef struct sockaddr_in t_sockaddr_in;
typedef struct timeval t_time;
class Server {

private:

	Server();
	
	std::string						_name;
	std::string						_host;
	int								_port;
	std::map<int, std::string>		_errorPages;
	std::map<std::string, Location>	_locations;
	std::string 					_logFile;
	t_server						_serverSettings;
	t_sockaddr_in					_sockaddr;
	int 							_fdSock;
	string							_logfile;
	int							createSocket(void);

public:
	Server(const t_server &ServSetting);
	t_server getSettings(void) const;
	Server(const Server &copy); //doesn't copy sockaddr_in struct
	~Server();
	Server& operator= (const Server &second); //doesn't copy sockaddr_in struct
	void Run(void);
	t_sockaddr_in& getSockAddr(void) {return (_sockaddr);};
	int& 	getSocket(void) {return (_fdSock);}
	void	throwException(string msg)const;
	bool 	_isrunning; // change it later


};

#endif //WEBSERV_SERVER_HPP






class ErrorException : public std::exception {
	public :
		const char* errorMsg;
		ErrorException(string msg){errorMsg = msg.c_str();};
		virtual const char* what(void) const throw (){return (this->errorMsg);};
};
