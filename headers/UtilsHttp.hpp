
#ifndef UTILSHTTP_HPP
#define UTILSHTTP_HPP

#include <string>
#include <map>

#include "HttpRequest.hpp"
#include "Config.hpp"

class MimeTypeDetector {
private:
	std::map<std::string, std::string> _mimeTypes;
	std::string _noneType;

	MimeTypeDetector();
	MimeTypeDetector(const MimeTypeDetector &other);
	MimeTypeDetector &operator=(const MimeTypeDetector &other);

public:
	static MimeTypeDetector &getInstance();
	const std::string getExtension(const std::string& second) const;
	const std::string &getMimeType(const std::string &fileName) const;
};

class StatusCodeMap {
private:
	std::map<int, std::string> statusCodes;

	StatusCodeMap();
	StatusCodeMap(const StatusCodeMap &other);
	StatusCodeMap &operator=(const StatusCodeMap &other);

public:
	static StatusCodeMap &getInstance();
	const std::string &getStatusCodeDescription(const int code);
};

#endif // UTILSHTTP_HPP