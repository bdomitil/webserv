#ifndef WEBSERV_CLIENT_HPP
#define WEBSERV_CLIENT_HPP
#include "MainIncludes.hpp"
using namespace std;

class Client
{
private:
	t_sockaddr_in			_sockaddr;
	int 					_fdSock;
	char					_ip[32];
	int 					_srvSocket;
	Request					_request;
	Response				_response;

public :
	Client(int srvSocket, std::map<std::string, Location> &locations);
	~Client();
	// Client(const Client &copy); //doesn't copy sockaddr_in struct
	// Client& operator= (const Client &second); //doesn't copy sockaddr_in struct
	t_sockaddr_in& getSockAddr(void) {return (_sockaddr);};
	int 	getSocket(void){ return int(_fdSock);};
	int		createSocket(void);
	bool					readRequest(); //handle exception whether body is too long
	bool					_toServe;
	bool					_isClosed;
	bool					_isRead;
	bool	toServe(){return _toServe;} //   tells whether the client's answer is ready
	bool	isRead(){return _isRead;} // tells whether the client got new info
	bool	isClosed(){return _isClosed;}  //tells whether the client closed his connection and we can delete it
	bool	response();


};

#endif
