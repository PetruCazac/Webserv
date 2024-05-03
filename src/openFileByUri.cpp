
#include "../headers/getMethod.hpp"
#include <iostream>
#include <map>
#include <string>
#include <stdio.h>

FILE *openFileByUri(const std::string &uri) {
	std::string path;
	std::string query;
	size_t questionMark = uri.find('?');
	if (questionMark == std::string::npos)
		path = uri;
	else {
		path = uri.substr(0, questionMark);
		query = uri.substr(questionMark + 1);
	}
	// std::cout << "Path: " << path << "; query: " << query << std::endl;
	if (path == "/")
		return fopen("test/index.html", "r");
	// else if (MimeTypeDetector)
		// return run_cgi(path, query);
	else {
		std::string mimeType = MimeTypeDetector::getInstance().getMimeType(path);
		return fopen(path.c_str(), "r");
		// return fopen("test.html", "r");

	}
	return NULL;
}

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
	// _mimeTypes[".php"] = "?";
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
