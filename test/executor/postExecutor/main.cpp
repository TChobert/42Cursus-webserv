#include "../../inc/executor/PostExecutor.hpp"
#include "../../inc/executor/Executor.hpp"
#include "../../inc/executor/CGIHandler.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>

void resetUploadDir() {
	system("rm -rf test_upload_dir && mkdir test_upload_dir");
}

void test_successful_upload() {
	resetUploadDir();

	Conversation conv;
	conv.req.body = "Test content";
	conv.req.uploadFileName = "test.txt";
	conv.location = new locationConfig();
	conv.location->uploadEnabled = true;
	conv.location->uploadStore = "test_upload_dir";

	PostExecutor::handlePost(conv);

	// On simule la reprise du write
	while (conv.state == WRITE_EXEC && conv.eState == WRITE_EXEC_POST_BODY) {
		PostExecutor::resumePostWriteBodyToFile(conv);
	}

	assert(conv.resp.status == CREATED);

	std::ifstream file("test_upload_dir/test.txt");
	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	assert(content == "Test content");

	std::cout << "✅ test_successful_upload passed\n";
	delete conv.location;
}

void test_upload_disabled() {
	Conversation conv;
	conv.req.body = "x";
	conv.req.uploadFileName = "test2.txt";
	conv.location = new locationConfig();
	conv.location->uploadEnabled = false;

	PostExecutor::handlePost(conv);

	assert(conv.resp.status == FORBIDDEN);
	std::cout << "✅ test_upload_disabled passed\n";
	delete conv.location;
}

void test_missing_upload_dir() {
	Conversation conv;
	conv.req.body = "x";
	conv.req.uploadFileName = "x.txt";
	conv.location = new locationConfig();
	conv.location->uploadEnabled = true;
	conv.location->uploadStore = "";

	PostExecutor::handlePost(conv);

	assert(conv.resp.status == INTERNAL_SERVER_ERROR);
	std::cout << "✅ test_missing_upload_dir passed\n";
	delete conv.location;
}

void test_post_cgi_fake_output() {
    Conversation conv;
    conv.req.method = "POST";
    conv.cgiOutput =
        "Status: 201\r\n"
    	"Content-Type: application/json\r\n"
    	"\r\n"
    	"{ \"message\": \"Created via CGI\" }";

    CGIHandler::parseCgiOutput(conv);
    Executor::updateResponseData(conv);

    std::cout << "[POST CGI Fake Output] ";
    if (conv.resp.status == 201 &&
        conv.resp.contentType == "application/json" &&
        conv.resp.body.find("Created via CGI") != std::string::npos)
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

void test_post_cgi_exec() {
    Conversation conv;
    conv.req.method = "POST";
    conv.req.body = "Hello from POST body";
    conv.req.pathOnDisk = "./test_cgi_post.sh";  // <- doit exister

    // Préparer la location
    locationConfig* loc = new locationConfig();
    loc->cgiHandlers[".sh"] = cgiHandler("./test_cgi_post.sh");  // <--- Important
    conv.location = loc;

    PostExecutor::handlePost(conv);  // prépare pipe et fork

    // phase d'écriture dans le pipe
    while (conv.state == WRITE_EXEC && conv.eState == WRITE_EXEC_POST_CGI) {
        PostExecutor::resumePostWriteBodyToCGI(conv);
    }

    // phase de lecture du résultat
    while (conv.state == READ_EXEC && conv.eState == READ_EXEC_POST_CGI) {
        PostExecutor::resumePostReadCGI(conv);
    }

    Executor::updateResponseData(conv);

    std::cout << "[Test POST CGI exec] ";
    if (conv.resp.status == 200 &&
        conv.resp.body.find("Hello from POST body") != std::string::npos)
        std::cout << "PASS\n";
    else
        std::cout << "FAIL: status=" << conv.resp.status << ", body=" << conv.resp.body << "\n";

    delete conv.location;
}

int main() {
	test_successful_upload();
	test_upload_disabled();
	test_missing_upload_dir();

	std::cout << "\n=== POST CGI Tests ===\n";
	test_post_cgi_fake_output();
	test_post_cgi_exec();
	return 0;
}
