#include "Http.hpp"

Http::Http() {
    _request = "";
    _response = "";
    _method = "";
    _uri = "";
    _http_version = "";
    _headers = std::map<std::string, std::string>();
    _body = "";
    memset(_buffer, 0, sizeof(_buffer));
}