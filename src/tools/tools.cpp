#include "../../includes/MainIncludes.hpp"

std::string	ft_itoa(int x)
{
	stringstream	buff;
	std::string		tmp;
	buff << x;
	buff >> tmp;

	return (tmp);
}

string getExtension(string fPath)
{
	std:: map<string, string> MIME;
	string	ext = "application/octet-stream";


	MIME.insert(pair<string, string>("mp3", "audio/mpeg"));
	MIME.insert(pair<string, string>("weba", "audio/webm"));
	MIME.insert(pair<string, string>("aac", "audio/aac"));

	MIME.insert(pair<string, string>("gif", "image/gif"));
	MIME.insert(pair<string, string>("jpeg", "image/jpeg"));
	MIME.insert(pair<string, string>("png", "image/png"));
	MIME.insert(pair<string, string>("jpg", "image/jpeg"));
	MIME.insert(pair<string, string>("tiff", "image/tiff"));
	MIME.insert(pair<string, string>("webp", "image/webp"));
	MIME.insert(pair<string, string>("ico", "image/x-icon"));

	MIME.insert(pair<string, string>("js", "application/javascript"));
	MIME.insert(pair<string, string>("json", "application/json"));
	MIME.insert(pair<string, string>("pdf", "application/pdf"));
	MIME.insert(pair<string, string>("ps", "application/postscript"));

	MIME.insert(pair<string, string>("css", "text/css"));
	MIME.insert(pair<string, string>("html", "text/html"));
	MIME.insert(pair<string, string>("csv", "text/csv"));
	MIME.insert(pair<string, string>("php", "text/php"));

	if (fPath.find_last_of('.') != std:: string :: npos)
	{
		string find = fPath.substr(fPath.find_last_of('.') + 1);
		for (map<string, string> :: iterator i = MIME.begin(); i != MIME.end(); i++)
		{
			if (find == (*i).first)
			{
				ext = (*i).second;
				break ;
			}
		}
	}
	return (ext);
}

bool	urlInfo(string fPath,t_fileInfo *fStruct){
	struct stat buff;
	int res;

	res = stat(fPath.c_str(), &buff);
	if (fStruct != nullptr)
	{
		fStruct->fType = static_cast<fileType>(res);
		if (fStruct->fType == NONEXIST)
			return(false);
		fStruct->fLength = buff.st_size;
		fStruct->fExtension = getExtension(fPath);
	}
	else if (res < 1)
		return (false);
	return (true);
}



char	*gen_def_page(int statusCode, uint64_t &bodySize){

	string line;
	char *buff = new char[200];
	memset(buff, 0, 200);
	line = "<html>\n";
	strncpy(&(buff[strlen(buff)]), line.c_str(), line.size());
	line = "<head><title>" + ft_itoa(statusCode) += " Not Found</title></head> \n";
	strncpy(&(buff[strlen(buff)]), line.c_str(), line.size());
	line = "<body>\n";
	strncpy(&(buff[strlen(buff)]), line.c_str(), line.size());
	line = "<center><h1>" + ft_itoa(statusCode)  + " ERROR! </h1></center>\n";
	strncpy(&(buff[strlen(buff)]), line.c_str(), line.size());
	line = "<hr><center>SUPER SERVER TEAM</center>\n";
	strncpy(&(buff[strlen(buff)]), line.c_str(), line.size());
	line = "</body>\n";
	strncpy(&(buff[strlen(buff)]), line.c_str(), line.size());
	line = "</html>\n";
	strncpy(&(buff[strlen(buff)]), line.c_str(), line.size());
	bodySize = strlen(buff);
	return (buff);
}