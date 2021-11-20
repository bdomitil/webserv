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
	MIME.insert(pair<string, string>("weba", "audio/webm")); MIME.insert(pair<string, string>("aac", "audio/aac"));

	MIME.insert(pair<string, string>("gif", "image/gif")); MIME.insert(pair<string, string>("jpeg", "image/jpeg"));
	MIME.insert(pair<string, string>("png", "image/png")); MIME.insert(pair<string, string>("jpg", "image/jpeg"));
	MIME.insert(pair<string, string>("image", "image/tiff")); MIME.insert(pair<string, string>("image", "image/webp"));

	MIME.insert(pair<string, string>("js", "application/javascript")); MIME.insert(pair<string, string>("json", "application/json"));
	MIME.insert(pair<string, string>("pdf", "application/pdf")); MIME.insert(pair<string, string>("application", "application/ps"));
	MIME.insert(pair<string, string>("css", "text/css"));MIME.insert(pair<string, string>("html", "text/html"));
	MIME.insert(pair<string, string>("csv", "text/csv"));MIME.insert(pair<string, string>("php", "text/php"));

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