
#include "UtilsHttp.hpp"
#include "HttpResponse.hpp"
#include <iostream>
#include <map>
#include <string>
#include <stdio.h>

MimeTypeDetector::MimeTypeDetector() {
	_mimeTypes[".html"] = "text/html";
	_mimeTypes[".htm"] = "text/html";
	_mimeTypes[".txt"] = "text/plain";
	_mimeTypes[".css"] = "text/css";
	_mimeTypes[".jpg"] = "image/jpeg";
	_mimeTypes[".jpeg"] = "image/jpeg";
	_mimeTypes[".png"] = "image/png";
	_mimeTypes[".gif"] = "image/gif";
	_mimeTypes[".ico"] = "image/x-icon";
	_noneType = "application/octet-stream";
}

MimeTypeDetector &MimeTypeDetector::getInstance() {
	static MimeTypeDetector mimeDetector;
	return mimeDetector;
}

const std::string &MimeTypeDetector::getMimeType(const std::string &fileName) const {
	size_t dot = fileName.find(".");
	if (dot == std::string::npos)
		return _noneType;
	std::string extention = fileName.substr(dot);
	for (std::map<std::string, std::string>::const_iterator it = _mimeTypes.begin(); it != _mimeTypes.end(); it++) {
		if (extention == it->first)
			return it->second;
	}
	return _noneType;
}

StatusCodeMap::StatusCodeMap() {
	statusCodes[200] = "OK"; // +
	statusCodes[201] = "Created";
	statusCodes[202] = "Accepted";
	statusCodes[204] = "No Content";
	statusCodes[304] = "Not Modified";
	statusCodes[400] = "Bad Request";
	statusCodes[401] = "Unauthorized";
	statusCodes[403] = "Forbidden";
	statusCodes[404] = "Not Found"; // +
	statusCodes[405] = "Method Not Allowed";
	statusCodes[409] = "Conflict";
	statusCodes[410] = "Gone";
	statusCodes[500] = "Internal Server Error";
	statusCodes[501] = "Not Implemented"; // +
	statusCodes[502] = "Bad Gateway";
	statusCodes[503] = "Service Unavailable";
	statusCodes[505] = "HTTP Version Not Supported"; // +
}

StatusCodeMap &StatusCodeMap::getInstance() {
	static StatusCodeMap statusCodeMap;
	return statusCodeMap;
}

const std::string &StatusCodeMap::getStatusCodeDescription(const int code) {
	for (std::map<int, std::string>::iterator it = statusCodes.begin(); it != statusCodes.end(); it++) {
		if (code == it->first)
			return it->second;
	}
	throw MethodsException(MethodsException::CODE_NOT_EXIST);
}
