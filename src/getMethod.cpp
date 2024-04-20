
// include this file in Make and webserv

#include <stdio.h>

// FILE *open_file_by_uri(const std::string &uri) {
// 	std::string [path, query] = uri.split('?');
// 	if (path == '/') {
// 		return open("index.html", "r");
// 	} else if (path.ends_with(".php")) {
// 		return run_php(path, query);
// 	} else {
// 		return open(path, "r");
// 	}
// }

// FILE *getMethod(s_httpRequest &httpRequest) {
// 	FILE *file = NULL;
// 	if (httpRequest.method != "GET") {
// 		std::cout << "not get method" << std::endl;
// 	}
// 	file = fopen()
// 	return file;
// }