#include "../../inc/executor/PostExecutor.hpp"
#include "../../inc/executor/Executor.hpp"
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

int main() {
	test_successful_upload();
	test_upload_disabled();
	test_missing_upload_dir();
	return 0;
}
