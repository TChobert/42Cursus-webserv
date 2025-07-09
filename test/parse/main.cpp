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

    parser.execute(conv);

    assert(conv.req.method == "GET");
    assert(conv.req.uri == "/index.html");
    assert(conv.req.version.first == 1);
    assert(conv.req.version.second == 1);
    assert(conv.state == VALIDATE);
    assert(conv.pState == MAYBE_BODY);
    std::cout << "test_valid_start_line passed\n";
}

void test_huge_start_line_method() {
    Parser parser;
    Conversation conv;
    conv.buf = std::string(startLineMax + 1, 'A') + " / HTTP/1.1\r\n";

    parser.execute(conv);

    assert(conv.state == RESPONSE);
    assert(conv.resp.status == 501);
    assert(conv.resp.shouldClose);
    std::cout << "test_huge_start_line_method passed\n";
}

void test_header_parsing() {
    Parser parser;
    Conversation conv;
    conv.buf = "GET / HTTP/1.1\r\nHost: example.com\r\nContent-Length: 4\r\n\r\n";

    parser.execute(conv);

    assert(conv.req.header["host"] == "example.com");
    assert(conv.req.header["content-length"] == "4");
    assert(conv.pState == MAYBE_BODY);
    std::cout << "test_header_parsing passed\n";
}

void test_body_parsing() {
    Parser parser;
    Conversation conv;
    conv.buf = "GET / HTTP/1.1\r\nContent-Length: 5\r\n\r\nHello";

    parser.execute(conv); // HEADER → BODY

    conv.state = PARSE_BODY; // simulate state set by server loop
    conv.req.bodyLeft = 5;

    parser.execute(conv); // BODY parsing

    assert(conv.req.body == "Hello");
    assert(conv.state == EXEC);
    assert(conv.pState == START);
    std::cout << "test_body_parsing passed\n";
}

void test_chunked_body_parsing() {
    Parser parser;
    Conversation conv;
    conv.buf =
        "POST /upload HTTP/1.1\r\n"
        "Transfer-Encoding: chunked\r\n\r\n"
        "4\r\nWiki\r\n"
        "5\r\npedia\r\n"
        "0\r\n\r\n";

    parser.execute(conv);  // Parse START → HEADER

    conv.state = PARSE_BODY; // Simulate transition to body phase
    parser.execute(conv);      // HEADER → MAYBE_BODY → BODY

    assert(conv.req.body == "Wikipedia");
    assert(conv.state == EXEC);
    assert(conv.pState == START);
    std::cout << "test_chunked_body_parsing passed\n";
}


void test_multiple_requests_with_skip_body() {
    Parser parser;
    Conversation conv;

    conv.buf =
        "POST /first HTTP/1.1\r\n"
        "Content-Length: 10\r\n\r\n1234567890"
        "GET /second HTTP/1.1\r\n"
        "Host: host.com\r\n\r\n";

    parser.execute(conv); // Parse header
    conv.req.bodyLeft = 10;
    conv.state = PARSE_HEADER;
    parser.execute(conv); // Will flip to SKIP_BODY on PARSE_HEADER + BODY

    assert(conv.req.method == "GET");
    assert(conv.req.uri == "/second");
    assert(conv.req.header["host"] == "host.com");
    std::cout << "test_multiple_requests_with_skip_body passed\n";
}

int main() {
    test_valid_start_line();
    test_huge_start_line_method();
    test_header_parsing();
    test_body_parsing();
    test_chunked_body_parsing();
    test_multiple_requests_with_skip_body();

    std::cout << "All tests passed successfully.\n";
    return 0;
}
