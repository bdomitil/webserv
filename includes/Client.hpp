#ifndef WEBSERV_CLIENT_HPP
#define WEBSERV_CLIENT_HPP
#include "MainIncludes.hpp"
using namespace std;

class Client
{
public:
	bool					_toServe;
	bool					_isClosed;
	bool					_isRead;

private:
	t_sockaddr_in			_sockaddr;
	int 					_fdSock;
	char					_ip[32];
	int 					_srvSocket;
	RequestBuffer			_reqBuff;

	Client(const Client &copy);
	Client& operator= (const Client &second);
	Client();

public :
	Client(int srvSocket);
	~Client();
	t_sockaddr_in&	getSockAddr(void) {return (_sockaddr);};
	int 			getSocket(void){ return int(_fdSock);};
	int				createSocket(void);
	bool			readRequest(); //handle exception whether body is too long
	bool			toServe(){return _toServe;} //   tells whether the client's answer is ready
	bool			isRead(){return _isRead;} // tells whether the client got new info
	bool			isClosed(){return _isClosed;}  //tells whether the client closed his connection and we can delete it 
	void			response();


};

#endif