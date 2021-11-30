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

	MIME.insert(pair<string, string>("avi", "video/x-msvideo"));
	MIME.insert(pair<string, string>("mpeg", "video/mpeg"));
	MIME.insert(pair<string, string>("ogv", "video/ogg"));
	MIME.insert(pair<string, string>("ts", "video/mp2t"));
	MIME.insert(pair<string, string>("webm", "video/webm"));
	MIME.insert(pair<string, string>("mp4", "video/mp4"));

	MIME.insert(pair<string, string>("gif", "image/gif"));
	MIME.insert(pair<string, string>("jpeg", "image/jpeg"));
	MIME.insert(pair<string, string>("png", "image/png"));
	MIME.insert(pair<string, string>("jpg", "image/jpeg"));
	MIME.insert(pair<string, string>("tiff", "image/tiff"));
	MIME.insert(pair<string, string>("webp", "image/webp"));
	MIME.insert(pair<string, string>("ico", "image/x-icon"));
	MIME.insert(pair<string, string>("bmp", "image/bmp"));

	MIME.insert(pair<string, string>("js", "application/javascript"));
	MIME.insert(pair<string, string>("json", "application/json"));
	MIME.insert(pair<string, string>("pdf", "application/pdf"));
	MIME.insert(pair<string, string>("ps", "application/postscript"));
	MIME.insert(pair<string, string>("doc", "application/msword"));

	MIME.insert(pair<string, string>("css", "text/css"));
	MIME.insert(pair<string, string>("html", "text/html"));
	MIME.insert(pair<string, string>("htm", "text/html"));
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

bool	urlInfo(string fPath,t_fileInfo *fStruct, std::ifstream &FILE){
	struct stat buff;
	int res;

	res = stat(fPath.c_str(), &buff);
	if (fStruct != nullptr && res != -1)
	{
		fStruct->fType = static_cast<fileType>(S_ISREG(buff.st_mode));
		if (fStruct->fType == NONEXIST)
			return(false);
		FILE.open(fPath);
		fStruct->fLength = buff.st_size;
		fStruct->fExtension = getExtension(fPath);
		if (FILE.is_open())
			fStruct->fStatus = 200;
		else {
				fStruct->fStatus = 403;
				FILE.close();
			}
	}
	else if (res < 1){
		fStruct->fStatus = 404;
		return (false);
	}
	return (true);
}



char	*gen_def_page(int statusCode, uint64_t &bodySize){

	std::stringstream buff;
	if (!g_errors.size()){
		g_errors.insert(std::pair<int, std::string>(200, " Ok"));
		g_errors.insert(std::pair<int, std::string>(201, " Created"));
		g_errors.insert(std::pair<int, std::string>(301, " Moved Permanently"));
		g_errors.insert(std::pair<int, std::string>(400, " Bad Request"));
		g_errors.insert(std::pair<int, std::string>(403, " Forbidden"));
		g_errors.insert(std::pair<int, std::string>(404, " Not Found"));
		g_errors.insert(std::pair<int, std::string>(405, " Method Not Allowed"));
		g_errors.insert(std::pair<int, std::string>(500, " Internal Server Error"));
		g_errors.insert(std::pair<int, std::string>(502, " Bad Gateway"));
		g_errors.insert(std::pair<int, std::string>(503, " Service Unavailable"));
	}
	buff << "<html>\n";
	buff << "<head><title>" + ft_itoa(statusCode) + g_errors[statusCode] + "</title>\n";
	buff << "<body>\n";
	buff << "<center><h1>" + ft_itoa(statusCode)  + g_errors[statusCode] + "</h1></center>\n";
	buff << "<hr><center>SUPER SERVER TEAM</center>\n";
	buff << "</body>\n";
	buff << "</html>\n";
	buff.seekg(0, buff.end);
	bodySize = buff.tellg();
	buff.seekg(0, buff.beg);
	char *def_page = new char[bodySize];
	buff.read(def_page, bodySize);
	return (def_page);
}

std::time_t increase_session_time(){
	time_t curr = std::time(0);
	std::tm *tmp  = localtime(&curr);
	tmp->tm_hour += 1;
	curr = std::mktime(tmp);
	return (curr);
}