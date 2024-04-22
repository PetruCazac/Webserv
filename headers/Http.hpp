#ifndef HTTP_HPP
#define HTTP_HPP
#include "Configuration.hpp"

class Http {
    public:
        Http();
        ~Http();
        void parseRequest();
        void processRequest();
        void sendResponse();
    private:
        std::string _request;
        std::string _response;
        std::string _method;
        std::string _uri;
        std::string _http_version;
        std::map<std::string, std::string> _headers;
        std::string _body;
        char _buffer[10];
};
#endif