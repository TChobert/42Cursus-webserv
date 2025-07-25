#include "../../inc/executor/DeleteExecutor.hpp"
#include "../../inc/executor/Executor.hpp"
#include "../../inc/executor/ResourceChecker.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>

void resetTestDir() {
    system("rm -rf test_delete_dir && mkdir test_delete_dir");
}

void createFile(const std::string& path, const std::string& content = "") {
    std::ofstream ofs(path.c_str());
    ofs << content;
    ofs.close();
}

void createEmptyDir(const std::string& path) {
    mkdir(path.c_str(), 0755);
}

void createNonEmptyDir(const std::string& path) {
    mkdir(path.c_str(), 0755);
    createFile(path + "/file.txt", "data");
}

void test_delete_file_success() {
    resetTestDir();
    std::string filepath = "test_delete_dir/file.txt";
    createFile(filepath, "Hello");

    Conversation conv;
    conv.req.pathOnDisk = filepath;
    conv.location = new locationConfig();
    conv.location->allowedMethods.push_back("DELETE");

    DeleteExecutor::handleDelete(conv);
    assert(conv.resp.status == 204);
    assert(access(filepath.c_str(), F_OK) == -1); // file should be gone

    std::cout << "✅ test_delete_file_success passed\n";
    delete conv.location;
}

void test_delete_dir_empty_success() {
    resetTestDir();
    std::string dirpath = "test_delete_dir/emptydir";
    createEmptyDir(dirpath);

    Conversation conv;
    conv.req.pathOnDisk = dirpath;
    conv.location = new locationConfig();
    conv.location->allowedMethods.push_back("DELETE");

    DeleteExecutor::handleDelete(conv);
    assert(conv.resp.status == 204);
    assert(access(dirpath.c_str(), F_OK) == -1); // dir should be gone

    std::cout << "✅ test_delete_dir_empty_success passed\n";
    delete conv.location;
}

void test_delete_dir_non_empty_forbidden() {
    resetTestDir();
    std::string dirpath = "test_delete_dir/nonemptydir";
    createNonEmptyDir(dirpath);

    Conversation conv;
    conv.req.pathOnDisk = dirpath;
    conv.location = new locationConfig();
    conv.location->allowedMethods.push_back("DELETE");

    DeleteExecutor::handleDelete(conv);
    assert(conv.resp.status == 403);
    assert(access(dirpath.c_str(), F_OK) == 0); // dir should still exist

    std::cout << "✅ test_delete_dir_non_empty_forbidden passed\n";
    delete conv.location;
}

void test_delete_file_not_found() {
    resetTestDir();
    std::string filepath = "test_delete_dir/nofile.txt";

    Conversation conv;
    conv.req.pathOnDisk = filepath;
    conv.location = new locationConfig();
    conv.location->allowedMethods.push_back("DELETE");

    DeleteExecutor::handleDelete(conv);
    assert(conv.resp.status == 404);

    std::cout << "✅ test_delete_file_not_found passed\n";
    delete conv.location;
}

void test_delete_method_not_allowed() {
    resetTestDir();
    std::string filepath = "test_delete_dir/file.txt";
    createFile(filepath);

    Conversation conv;
    conv.req.pathOnDisk = filepath;
    conv.location = new locationConfig();
    // Ne pas ajouter "DELETE" dans allowedMethods, donc DELETE non autorisé

    DeleteExecutor::handleDelete(conv);
    assert(conv.resp.status == 405);

    std::cout << "✅ test_delete_method_not_allowed passed\n";
    delete conv.location;
}

int main() {
    test_delete_file_success();
    test_delete_dir_empty_success();
    test_delete_dir_non_empty_forbidden();
    test_delete_file_not_found();
    test_delete_method_not_allowed();
    return 0;
}
