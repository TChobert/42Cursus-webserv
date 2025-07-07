#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "ResponseBuilder.hpp"

int main()
{
    Conversation conv;

    // Test réponse 200 OK
    conv.statusCode = 200;
    conv.response.statusCode = 200;
    conv.response.body = "<html><body>Hello World</body></html>";
    conv.response.contentType = "text/html";

    ResponseBuilder::execute(conv);

	std::cout << conv.final_response << std::endl << std::endl;

    // Test réponse 404 Not Found
    conv.statusCode = 404;
    conv.response.statusCode = 404;
    conv.response.body.clear();
    conv.response.contentType.clear();

    ResponseBuilder::execute(conv);
    std::cout << conv.final_response << std::endl;

    return 0;
}
