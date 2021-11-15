#include "../../includes/MainIncludes.hpp"

Server :: Server(const t_server &ServerSetting)
{
    _name = ServerSetting.name;
    _host = ServerSetting.host;
    _port = ServerSetting.port;
    _locations = ServerSetting.locations;
    _errorPages = ServerSetting.errorPages;
    _serverSettings = t_server(ServerSetting);
}

Server :: Server(const Server &copy)
{
    *this = copy;
}

Server& Server :: operator=(const Server &copy)
{
    *this = Server(copy.getSettings());
    return (*this);
}

t_server Server :: getSettings(void) const
{
    return (this->_serverSettings);
} 