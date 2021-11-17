#ifndef REQUEST_BUFFER_HPP
# define REQUEST_BUFFER_HPP
# include <cstdint>
# include <vector>
# include <string>
# include <iostream>

# define START_LINE		0
# define HEADER_LINE	1
# define BODY_LINE		2

class RequestBuffer {

public:

	RequestBuffer(std::uint32_t);
	~RequestBuffer() {}

	bool	saveRequestData();
	void	showState() const ;
	char	*getBuffer() const ;

private:

	void	saveStartLine(std::string);
	void	saveHeaderLine(std::string);
	void	saveBodyPart(std::string);

	std::vector<std::string>	_reqHeaders;
	std::uint32_t const			_maxBodySize;
	std::uint8_t				_callCount;
	std::string					_reqStartLine;
	std::string					_reqBodyLines;
	std::string					_tmpBuffer;
	bool						_isReqDone;

};

#endif
