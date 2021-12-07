#ifndef WEBSERV_CLIENT_HPP
#define WEBSERV_CLIENT_HPP
#include "MainIncludes.hpp"
using namespace std;

class Client
{
public :
	bool					_toServe;
	bool					isClosed;
	bool					_isRead;

private:
	t_sockaddr_in			_sockaddr;
	int 					_fdSock;
	char					_ip[32];
	int 					_srvSocket;
	Request					_request;
	Response				*_response;
	std::time_t				_session_start;
	std::time_t				_session_end;

public :
	Client(int srvSocket, std::multimap<std::string, Location> const &locations);
	~Client();
	t_sockaddr_in& getSockAddr(void){return (_sockaddr);}
	bool	toServe(){return _toServe;}
	bool	isRead(){return _isRead;}
	int 	getSocket(void){return _fdSock;}
	int 	getSrvSocket(void){return _srvSocket;}
	void	response(std::map<int, std::string> &errorPages);
	int		createSocket(void);
	bool	readRequest();
	bool	SessionIsOver();


};

#endif
