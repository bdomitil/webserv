#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "MainIncludes.hpp"
using namespace std;

class Client
{
private:
	t_sockaddr_in			_sockaddr;
	int 					_fdSock;
	char					_ip[32];
	int 					_srvSocket;
	Client();

public :
	Client(int srvSocket);
	~Client();
	Client(const Client &copy); //doesn't copy sockaddr_in struct
	Client& operator= (const Client &second); //doesn't copy sockaddr_in struct
	t_sockaddr_in& getSockAddr(void) {return (_sockaddr);};
	int getSocket(void) const {return (_fdSock);}
	int		createSocket(void);
	bool					_toServe;
	bool					_isRead;
	bool	toServe(){return _toServe;} //   tells whether the client's answer is ready
	bool	isRead(){return _isRead;} // tells whether the client got new info

};

#endif