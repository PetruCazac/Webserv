
#include "HttpRequest.hpp"

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

FILE *openFileByUri(const std::string &uri);
