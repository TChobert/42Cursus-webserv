#include <iostream>
#include <cassert>
#include <string>

#include "ResponseBuilder.hpp"
#include "Conversation.hpp"

// -------------------------
// 🌐 Réponses HTTP standards
// -------------------------

void test_200_ok_with_body() {
    Conversation conv;
    conv.statusCode = 200;
    conv.response.statusCode = 200;
    conv.response.body = "<html><body>Hello World</body></html>";
    conv.response.contentType = "text/html";

	std::cout << "Body length: " << conv.response.body.size() << std::endl;

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.final_response << std::endl << std::endl;

    assert(conv.final_response.find("HTTP/1.1 200 OK") != std::string::npos);
    assert(conv.final_response.find("Content-Type: text/html") != std::string::npos);
    assert(conv.final_response.find("Content-Length: 37") != std::string::npos);
    assert(conv.final_response.find("Hello World") != std::string::npos);

	std::cout << "✔ test_200_ok_with_body passed\n\n";
}

void test_404_not_found_empty_body() {
    Conversation conv;
    conv.statusCode = 404;
    conv.response.statusCode = 404;
    conv.response.body.clear();
    conv.response.contentType.clear();

    ResponseBuilder::execute(conv);

	std::cout << "Final response:\n" << conv.final_response << std::endl << std::endl;

    assert(conv.final_response.find("404 Not Found") != std::string::npos);
    assert(conv.final_response.find("Content-Type: text/plain") != std::string::npos);
    assert(conv.response.shouldClose == true);

	std::cout << "✔ test_404_not_found_empty_body passed\n\n";
}

void test_500_internal_server_error() {
    Conversation conv;
    conv.statusCode = 500;
    conv.response.statusCode = 500;
    conv.response.body.clear();
    conv.response.contentType.clear();

    ResponseBuilder::execute(conv);

    assert(conv.final_response.find("500 Internal Server Error") != std::string::npos);
    assert(conv.final_response.find("Content-Type: text/plain") != std::string::npos);
    assert(conv.response.shouldClose == true);

    std::cout << "✔ test_500_internal_server_error passed\n";
}

void test_418_teapot() {
    Conversation conv;
    conv.statusCode = 418;
    conv.response.statusCode = 418;
    conv.response.body = "I'm a teapot";
    conv.response.contentType = "text/plain";

    ResponseBuilder::execute(conv);

    assert(conv.final_response.find("418 I'm a teapot") != std::string::npos);
    assert(conv.final_response.find("I'm a teapot") != std::string::npos);

    std::cout << "✔ test_418_teapot passed\n";
}

// -------------------------
// 📑 Headers automatiques
// -------------------------

void test_missing_content_type_sets_default() {
    Conversation conv;
    conv.statusCode = 400;
    conv.response.statusCode = 400;
    conv.response.body = "Bad request";
    conv.response.contentType = "";

    ResponseBuilder::execute(conv);

    assert(conv.final_response.find("Content-Type: text/plain") != std::string::npos);
    std::cout << "✔ test_missing_content_type_sets_default passed\n";
}

void test_content_length_is_computed() {
    Conversation conv;
    conv.statusCode = 200;
    conv.response.statusCode = 200;
    conv.response.body = "abcd";
    conv.response.contentType = "text/plain";

    ResponseBuilder::execute(conv);

    assert(conv.final_response.find("Content-Length: 4") != std::string::npos);
    std::cout << "✔ test_content_length_is_computed passed\n";
}

void test_should_close_flag_set_on_error() {
    Conversation conv;
    conv.statusCode = 500;
    conv.response.statusCode = 500;

    ResponseBuilder::execute(conv);
    assert(conv.response.shouldClose == true);
    std::cout << "✔ test_should_close_flag_set_on_error passed\n";
}

void test_should_close_flag_not_set_on_200() {
    Conversation conv;
    conv.statusCode = 200;
    conv.response.statusCode = 200;

    ResponseBuilder::execute(conv);
    assert(conv.response.shouldClose == false);
    std::cout << "✔ test_should_close_flag_not_set_on_200 passed\n";
}

// -------------------------
// 🔁 Cas spécifiques
// -------------------------

void test_empty_body_sets_content_length_zero() {
    Conversation conv;
    conv.statusCode = 204;
    conv.response.statusCode = 204;
    conv.response.body = "";
    conv.response.contentType = "text/plain";

    ResponseBuilder::execute(conv);
    assert(conv.final_response.find("Content-Length: 0") != std::string::npos);
    std::cout << "✔ test_empty_body_sets_content_length_zero passed\n";
}

void test_body_with_binary_content_type() {
    Conversation conv;
    conv.statusCode = 200;
    conv.response.statusCode = 200;
    conv.response.body = "\x01\x02\x03";
    conv.response.contentType = "application/octet-stream";

    ResponseBuilder::execute(conv);
    assert(conv.final_response.find("Content-Type: application/octet-stream") != std::string::npos);
    std::cout << "✔ test_body_with_binary_content_type passed\n";
}

void test_response_line_format() {
    Conversation conv;
    conv.statusCode = 201;
    conv.response.statusCode = 201;
    conv.response.body = "Created";
    conv.response.contentType = "text/plain";

    ResponseBuilder::execute(conv);

    assert(conv.final_response.find("HTTP/1.1 201 Created\r\n") != std::string::npos);
    std::cout << "✔ test_response_line_format passed\n";
}

void test_multiple_execute_calls() {
    Conversation conv;
    conv.statusCode = 200;
    conv.response.statusCode = 200;
    conv.response.body = "First";
    conv.response.contentType = "text/plain";

    ResponseBuilder::execute(conv);
    std::string firstResponse = conv.final_response;

    conv.response.body = "Second";
    ResponseBuilder::execute(conv);
    std::string secondResponse = conv.final_response;

    assert(firstResponse != secondResponse);
    assert(secondResponse.find("Second") != std::string::npos);
    std::cout << "✔ test_multiple_execute_calls passed\n";
}

// -------------------------
// 🧪 Main de test
// -------------------------

int main() {
    // 🌐 Réponses HTTP
    test_200_ok_with_body();
    test_404_not_found_empty_body();
    test_500_internal_server_error();
    test_418_teapot();

    // 📑 Headers automatiques
    test_missing_content_type_sets_default();
    test_content_length_is_computed();
    test_should_close_flag_set_on_error();
    test_should_close_flag_not_set_on_200();

    // 🔁 Cas spécifiques
    test_empty_body_sets_content_length_zero();
    test_body_with_binary_content_type();
    test_response_line_format();
    test_multiple_execute_calls();

    std::cout << "\n✅ All ResponseBuilder tests passed successfully.\n";
    return 0;
}
