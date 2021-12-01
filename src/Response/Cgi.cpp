#include "../../includes/MainIncludes.hpp"

Cgi :: Cgi(Request &_request, std::map<std::string, std::string> &cgiPath , std::string &method) : _request(_request), _cgiPath(cgiPath), _method(method){

}