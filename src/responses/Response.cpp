#include "../../includes/MainIncludes.hpp"
#include <cstdlib>
Response :: Response(){

}

void Response :: makeStatusLine(std::string comment){
	_statusLine = "HTTP/1.1 " + ft_itoa(_statusCode) + " " + comment + "\n";
}

std::string Response :: send()
{
	
}