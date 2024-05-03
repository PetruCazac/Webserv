#ifndef OPENBYURI_HPP
#define OPENBYURI_HPP

#include <string>
#include <map>

#include "HttpRequest.hpp"
#include "Config.hpp"

class MimeTypeDetector {
private:
	std::map<std::string, std::string> _mimeTypes;
	const std::string _noneType = "application/octet-stream";

	MimeTypeDetector();
	MimeTypeDetector(const MimeTypeDetector &other);
	MimeTypeDetector &operator=(const MimeTypeDetector &other);

public:
	static MimeTypeDetector &getInstance();
	const std::string &getMimeType(const std::string &fileName) const;
};

struct MethodsException {
	enum MethodErrors {
		ERR
	};

	MethodErrors err;

	explicit MethodsException(MethodErrors err) :
		err(err) { }

	const char *what() const throw() {
		switch (err) {
			case ERR: return "cannot open file by uri"; break;
			default: return "unknown error"; break;
		}
	}
};

FILE *openFileByUri(const std::string &uri, std::vector<ServerDirectives> server);

#endif