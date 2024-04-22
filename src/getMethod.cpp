
// include this file in Make and webserv

#include "../headers/getMethod.hpp"
#include <iostream>
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
	std::cout << "Path: " << path << "; query: " << query << std::endl;
	if (path == "/")
		return fopen("test/index.html", "r");
	// else {

	// }
	return NULL;
}
