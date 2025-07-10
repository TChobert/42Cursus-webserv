#include <iostream>
#include <cassert>
#include <string>

#include "ResponseBuilder.hpp"
#include "Conversation.hpp"

void test_200_ok_with_body() {
    Conversation conv;
    conv.statusCode = 200;
    conv.resp.statusCode = 200;
    conv.resp.body = "<html><body>Hello World</body></html>";
    conv.resp.contentType = "text/html";

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    assert(conv.finalResponse.find("HTTP/1.1 200 OK") != std::string::npos);
    assert(conv.finalResponse.find("Content-Type: text/html") != std::string::npos);
    assert(conv.finalResponse.find("Content-Length: 37") != std::string::npos);
    assert(conv.finalResponse.find("Hello World") != std::string::npos);

    std::cout << "✔ test_200_ok_with_body passed\n\n";
}

void test_404_not_found_with_body() {
    Conversation conv;
    conv.statusCode = 404;
    conv.resp.statusCode = 404;
    conv.resp.body = "<html><body><h1>404 Not Found</h1></body></html>";
    conv.resp.contentType = "text/html";

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    assert(conv.finalResponse.find("404 Not Found") != std::string::npos);
    assert(conv.finalResponse.find("Content-Type: text/html") != std::string::npos);
    assert(conv.finalResponse.find("Content-Length:") != std::string::npos);
    assert(conv.finalResponse.find("<html>") != std::string::npos);

    std::cout << "✔ test_404_not_found_with_body passed\n\n";
}

void test_500_internal_server_error_body_provided() {
    Conversation conv;
    conv.statusCode = 500;
    conv.resp.statusCode = 500;
    conv.resp.body = "<html><body><h1>500</h1></body></html>";
    conv.resp.contentType = "text/html";

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    assert(conv.finalResponse.find("500 Internal Server Error") != std::string::npos);
    assert(conv.finalResponse.find("Content-Type: text/html") != std::string::npos);
    assert(conv.finalResponse.find("Content-Length:") != std::string::npos);

    std::cout << "✔ test_500_internal_server_error_body_provided passed\n\n";
}

void test_301_moved_permanently_with_location() {
    Conversation conv;
    conv.statusCode = 301;
    conv.resp.statusCode = 301;
    conv.resp.contentType = "text/html";
    conv.resp.body = "<html><body>Redirecting...</body></html>";
    conv.resp.location = "https://example.com/new-location";

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    assert(conv.finalResponse.find("HTTP/1.1 301 Moved Permanently") != std::string::npos);
    assert(conv.finalResponse.find("Location: https://example.com/new-location") != std::string::npos);
    assert(conv.finalResponse.find("Content-Length:") != std::string::npos);
    assert(conv.finalResponse.find("Redirecting") != std::string::npos);

    std::cout << "✔ test_301_moved_permanently_with_location passed\n\n";
}

void test_302_found_with_location() {
    Conversation conv;
    conv.statusCode = 302;
    conv.resp.statusCode = 302;
    conv.resp.contentType = "text/html";
    conv.resp.body = "<html><body>Found at new location</body></html>";
    conv.resp.location = "/temporary-redirect";

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    assert(conv.finalResponse.find("HTTP/1.1 302 Found") != std::string::npos);
    assert(conv.finalResponse.find("Location: /temporary-redirect") != std::string::npos);
    assert(conv.finalResponse.find("Found at new location") != std::string::npos);

    std::cout << "✔ test_302_found_with_location passed\n\n";
}

void test_418_teapot() {
    Conversation conv;
    conv.statusCode = 418;
    conv.resp.statusCode = 418;
    conv.resp.body = "I'm a teapot";
    conv.resp.contentType = "text/plain";

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    assert(conv.finalResponse.find("501 Not Implemented") != std::string::npos);

    std::cout << "✔ test_418_teapot passed\n\n";
}

void test_content_length_is_computed() {
    Conversation conv;
    conv.statusCode = 200;
    conv.resp.statusCode = 200;
    conv.resp.body = "abcd";
    conv.resp.contentType = "text/plain";

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    assert(conv.finalResponse.find("Content-Length: 4") != std::string::npos);
    std::cout << "✔ test_content_length_is_computed passed\n\n";
}

void test_empty_body_sets_content_length_zero() {
    Conversation conv;
    conv.statusCode = 204;
    conv.resp.statusCode = 204;
    conv.resp.body = "";
    conv.resp.contentType = "text/plain";

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    assert(conv.finalResponse.find("") != std::string::npos);
    std::cout << "✔ test_empty_body_sets_content_length_zero passed\n\n";
}

void test_body_with_binary_content_type() {
    Conversation conv;
    conv.statusCode = 200;
    conv.resp.statusCode = 200;
    conv.resp.body = "\x01\x02\x03";
    conv.resp.contentType = "application/octet-stream";

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    assert(conv.finalResponse.find("Content-Type: application/octet-stream") != std::string::npos);
    std::cout << "✔ test_body_with_binary_content_type passed\n\n";
}

void test_response_line_format() {
    Conversation conv;
    conv.statusCode = 201;
    conv.resp.statusCode = 201;
    conv.resp.body = "Created";
    conv.resp.contentType = "text/plain";

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    assert(conv.finalResponse.find("HTTP/1.1 201 Created\r\n") != std::string::npos);
    std::cout << "✔ test_response_line_format passed\n\n";
}

void test_multiple_execute_calls() {
    Conversation conv;
    conv.statusCode = 200;
    conv.resp.statusCode = 200;
    conv.resp.body = "First";
    conv.resp.contentType = "text/plain";

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    std::string firstResponse = conv.finalResponse;

    conv.resp.body = "Second";
    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.finalResponse << std::endl << std::endl;

    std::string secondResponse = conv.finalResponse;

    assert(firstResponse != secondResponse);
    assert(secondResponse.find("Second") != std::string::npos);
    std::cout << "✔ test_multiple_execute_calls passed\n\n";
}

// -------------------------
// 🧪 Main de test
// -------------------------

int main() {
    test_200_ok_with_body();
    test_404_not_found_with_body();
    test_500_internal_server_error_body_provided();
	test_302_found_with_location();
	test_301_moved_permanently_with_location();
    test_418_teapot();
    test_content_length_is_computed();
    test_empty_body_sets_content_length_zero();
    test_body_with_binary_content_type();
    test_response_line_format();
    test_multiple_execute_calls();

    std::cout << "\n✅ All ResponseBuilder tests passed successfully.\n";
    return 0;
}



