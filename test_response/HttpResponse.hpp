#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <string>
#include <vector>
#include <map>

class HttpResponse
{
public:
    int statusCode;
    bool shouldClose;
    bool chunked;
    std::string body;
    std::string contentType;
    std::string location;
    std::vector<std::string> setCookies;
    std::vector<std::string> allowedMethods;
    std::map<std::string, std::string> headers;

    HttpResponse() : statusCode(200), shouldClose(false), chunked(false) {}
};

#endif
