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
	static std:: map<string, string> MIME;
	string	ext = "application/octet-stream";

	if (!MIME.size()){
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
	}

	if (fPath.find_last_of('.') != std:: string :: npos)
	{
		string find = fPath.substr(fPath.find_last_of('.') + 1);
		for (map<string, string> :: iterator i = MIME.begin(); i != MIME.end(); i++)
		{
			if (find == i->first || find == strUpper(i->first))
			{
				ext = i->second;
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

int  checkCgi(const std::multimap<std::string, std::string>& Cgi, std::string fPath){
	struct stat	buff;
	int			res = 0;
	std::string ext = "." + fPath.substr(fPath.find_last_of('.') + 1);

	std::multimap<std::string, std::string>::const_iterator i = Cgi.begin();
	for (; i != Cgi.end(); i++){
		if (ext == i->first){
		stat(i->second.c_str(), &buff);
			if (res == -1 || !(buff.st_mode & S_IXUSR))
				return (-1);
			res++;
		}
	}
	return (res);
}


char	*gen_def_page(uint32_t &statusCode,
					uint64_t &bodySize,
					const char *path,
					const Location *location) {

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
	else if (!(def_page = filesListing(std::string(path) , bodySize, statusCode, location)))
		def_page = gen_def_page(statusCode, bodySize, nullptr, location);
	return (def_page);
}

std::time_t increase_session_time(){
	time_t curr = std::time(0);
	std::tm *tmp  = localtime(&curr);
	tmp->tm_hour += 1;
	curr = std::mktime(tmp);
	return (curr);
}

void free_execData(const char ***execData) {
	delete execData[0][0];
	delete execData[0][1];
	delete execData[0];
	for (int i = 0; execData[1][i] != nullptr; i++){
		delete execData[1][i];
	}
	delete execData[1];
	delete execData;
}

std::map <int, std::string> &error_map() {
	static  std::map <int, std::string> error_map;
	if (!error_map.size()){
		error_map.insert(std::pair<int, std::string>(200, " Ok"));
		error_map.insert(std::pair<int, std::string>(201, " Created"));
		error_map.insert(std::pair<int, std::string>(204, " No Content"));
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

static std::string	buildPathToFile(std::string const &fullPath,
									const Location *location,
									std::string fileName) {

	std::string	resultPath;
	std::string	tmp;
	std::size_t	pos;

	pos = fullPath.rfind(location->root);
	if (fileName == "."
		or fileName == ".." or pos == std::string::npos)
		return ".";

	tmp = fullPath.substr(pos + location->root.length());
	if (!tmp.length())
		tmp = "/";
	pos = tmp.find(location->path);
	if (pos == std::string::npos)
		return ".";
	if (tmp ==  "/")
		return fileName;
	return (tmp.substr(pos) + "/" + fileName);
}

char	*filesListing(std::string const &path,
					uint64_t &bodySize, uint32_t &statusCode,
					const Location *location) {

	std::string		htmlBody;
	DIR				*dirPtr;
	struct dirent	*dirent;
	std::string		pathToFile;

	if (!location)
		return nullptr;
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
		pathToFile = buildPathToFile(path, location, dirent->d_name);
		if (pathToFile != ".") {
			htmlBody += "<div><a href=\"" + pathToFile + "\"><h2>"
				+ dirent->d_name + "</a></h2>\n";
		}
		dirent = readdir(dirPtr);
	}
	closedir(dirPtr);
	htmlBody += "</body>\n</html>\n";
	bodySize = htmlBody.length();
	statusCode = 200;
	return strdup(htmlBody.c_str());
}

void	killChilds(pid_t *pid, int childNum) {

	for (int i = 0; i < childNum; i++)
		kill(pid[i], SIGKILL);
	return;
}

off_t getFdLen(int fd){
	struct stat buff;
	off_t	len = 0;
	if (fstat(fd, &buff) == -1)
		len = 0;
	else
		len = buff.st_size;
	return (len);
}

void	waitChild(int x){
	int status;
	waitpid(-1, &status , WNOHANG);
}

bool	isCharWhiteSpace(unsigned char c) {
	return std::isspace(c);
}

std::size_t	skipWhiteSpaces(std::string const &str, std::size_t start) {

	if (start >= str.length())
		return str.length();
	while (start < str.length() and isCharWhiteSpace(str[start]))
		start++;
	return start;
}

std::uint8_t	isDirOrFile(const char *path) {

	struct stat	s;

	if (stat(path, &s) == -1)
		return NOT_FOUND;
	if (s.st_mode & S_IFDIR)
		return DIR_MODE;
	if (s.st_mode & S_IFREG)
		return FILE_MODE;
	return UNKNOWN_MODE;
}

std::string strUpper(const std::string &str){
	std::string to_ret = str;
	for (std::string ::iterator i = to_ret.begin(); i != to_ret.end() ; i++){
		*i = toupper(*i);
	}
	return to_ret;
}

std::string putDelete(Request &request, uint32_t &statusCode){

	std::string _url = request.getUrl(statusCode);
	if (statusCode == 1 )
		return (_url);
	else if (request.getMethod() == "PUT"){
		char *home = getenv("HOME");
		int pos = _url.find(request.getLocation()->path);
		if (home)
			_url =  std::string(home) + "/Downloads" + request.getLocation()->path + _url.substr(_url.find_last_of('/') + 1);
		else
			_url =  "/var/www/Downloads" + _url.substr(pos);
		std::ofstream newFile(_url);
		if (!newFile.is_open())
			statusCode = 204;
		else{
			std::string const &body(request.getBody());
			newFile.write(body.c_str(), body.size());
			newFile.close();
			statusCode = 201;
		}
	}
	else if (request.getMethod() == "DELETE"){
		if (access(_url.c_str() , W_OK) == -1 || remove(_url.c_str()) == -1)
			statusCode = 403;
		else
			statusCode = 204;
		_url = _url.substr(_url.find(request.getLocation()->getRoot()));
	}
	return _url;
}
