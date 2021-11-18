#include "../../includes/MainIncludes.hpp"

std::string	ft_itoa(int x)
{
	stringstream	buff;
	std::string		tmp;
	buff << x;
	buff >> tmp;
	
	return (tmp);
}