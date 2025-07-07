#include <iostream>
#include <cassert>
#include "../../inc/webserv.hpp"

using namespace std;
void test_valid_start_line() {
    Parser parser;
    Conversation conv;
    conv.buf = "GET /index.html HTTP/1.1\r\n"
                "Host:   host.com    \r\n"
                "\r\n";

    parser.parse(conv);

    assert(conv.req.method == "GET");
    assert(conv.req.uri == "/index.html");
    assert(conv.req.version.first == 1);
    assert(conv.req.version.second == 1);
    assert(conv.state == VALIDATE);
    assert(conv.pState == BODY);
    std::cout << "test_valid_start_line passed\n";
}

void test_huge_start_line_method() {
    Parser parser;
    Conversation conv;
    conv.buf = std::string(startLineMax + 1, 'A') + " / HTTP/1.1\r\n";

    parser.parse(conv);

    assert(conv.state == RESPONSE);
    assert(conv.resp.statusCode == 501);
    assert(conv.resp.shouldClose);
    std::cout << "test_huge_start_line_method passed\n";
}

void test_header_parsing() {
    Parser parser;
    Conversation conv;
    conv.buf = "GET / HTTP/1.1\r\nHost: example.com\r\nContent-Length: 4\r\n\r\n";

    parser.parse(conv);

    assert(conv.req.header["host"] == "example.com");
    assert(conv.req.header["content-length"] == "4");
    assert(conv.pState == BODY);
    std::cout << "test_header_parsing passed\n";
}

void test_body_parsing() {
    Parser parser;
    Conversation conv;
    conv.buf = "GET / HTTP/1.1\r\nContent-Length: 5\r\n\r\nHello";

    parser.parse(conv); // HEADER → BODY

    conv.state = PARSE_BODY; // simulate state set by server loop
    conv.bodyLeft = 5;

    parser.parse(conv); // BODY parsing

    assert(conv.req.body == "Hello");
    assert(conv.state == EXEC);
    assert(conv.pState == HEADER);
    std::cout << "test_body_parsing passed\n";
}

int main() {
    test_valid_start_line();
    test_huge_start_line_method();
    test_header_parsing();
    test_body_parsing();

    std::cout << "All tests passed successfully.\n";
    return 0;
}
