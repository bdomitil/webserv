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
	else if (res < 1) {
		fStruct->fStatus = 404;
		return (false);
	}
	return (true);
}



char	*gen_def_page(uint32_t &statusCode, uint64_t &bodySize, const char *path){
	
	char *def_page;
	if (!path){
		std::stringstream buff;
		buff << "<html>\n";
		buff << "<head><title>" + ft_itoa(statusCode) + error_map()[statusCode] + "</title>\n";
		buff << "<body>\n";
		buff << "<center><h1>" + ft_itoa(statusCode)  + error_map()[statusCode] + "</h1></center>\n";
		buff << "<hr><center>SUPER SERVER TEAM</center>\n";
		buff << "</body>\n";
		buff << "</html>\n";
		buff.seekg(0, buff.end);
		bodySize = buff.tellg();
		buff.seekg(0, buff.beg);
		def_page = new char[bodySize];
		buff.read(def_page, bodySize);
	}
	else if (!(def_page = filesListing(std::string(path) , bodySize, statusCode)))
		def_page = gen_def_page(statusCode, bodySize, nullptr);
	return (def_page);
}

std::time_t increase_session_time(){
	time_t curr = std::time(0);
	std::tm *tmp  = localtime(&curr);
	// tmp->tm_hour += 1;
	tmp->tm_sec += 10;
	curr = std::mktime(tmp);
	return (curr);
}

const char ***makeData_for_exec(std::string &path, std::map <std::string, std::string> &headers){
	const char **env = new const char*[headers.size() + 1];
	const char **args = new const char*[2];
	const char ***to_ret =  new const char**[2];

	env[headers.size()] = nullptr;
	args[1] = nullptr;
	args[0] = strdup(path.c_str());
	std::map <std::string, std::string> :: iterator i = headers.begin();
	for (int j = 0; i != headers.end(); i++, j++){
		env[j] = strdup((i->first + "=" + i->second).c_str());
	}
	to_ret[0] = args;
	to_ret[1] = env;
	return (to_ret);
}

void free_execData(const char ***execData){
	delete execData[0][0];
	delete execData[0][1];
	delete execData[0];
	for (int i = 0; execData[1][i] != nullptr; i++){
		delete execData[1][i];
	}
	delete execData[1];
	delete execData;
}
 std::map <int, std::string> &error_map(){
	static  std::map <int, std::string> error_map;
	if (!error_map.size()){
		error_map.insert(std::pair<int, std::string>(200, " Ok"));
		error_map.insert(std::pair<int, std::string>(201, " Created"));
		error_map.insert(std::pair<int, std::string>(301, " Moved Permanently"));
		error_map.insert(std::pair<int, std::string>(400, " Bad Request"));
		error_map.insert(std::pair<int, std::string>(403, " Forbidden"));
		error_map.insert(std::pair<int, std::string>(404, " Not Found"));
		error_map.insert(std::pair<int, std::string>(405, " Method Not Allowed"));
		error_map.insert(std::pair<int, std::string>(500, " Internal Server Error"));
		error_map.insert(std::pair<int, std::string>(502, " Bad Gateway"));
		error_map.insert(std::pair<int, std::string>(503, " Service Unavailable"));
	}
	return error_map;
}


char	*filesListing(std::string const &path, uint64_t &bodySize, uint32_t &statusCode) {

	std::string		htmlBody;
	DIR				*dirPtr;
	struct dirent	*dirent;
	std::string		tmp;

	dirPtr = opendir(path.c_str());
	if (!dirPtr) {
		statusCode = 403;
		return nullptr;
	}

	htmlBody = "<!DOCTYPE html>\n";
	htmlBody += "<html>\n";
	htmlBody += "<head><title>AutoIndexON</title></head>\n";
	htmlBody += "<body>\n<h1>Files in current directory</h1>\n";
	dirent = readdir(dirPtr);
	while (dirent) {
		tmp = dirent->d_name;
		htmlBody += "<a href=\"" + tmp + "\">" + tmp + "</a>\n";
		dirent = readdir(dirPtr);
	}
	closedir(dirPtr);
	htmlBody += "</body>\n</html>\n";
	bodySize = htmlBody.length();
	statusCode = 200;
	return strdup(htmlBody.c_str());
}