#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>  // pour system()
#include <string>
#include <vector>

// Inclure tes headers
#include "GetExecutor.hpp"
#include "webserv.hpp"  // Contient locationConfig, Conversation, response, etc.

// Crée une locationConfig de test basique
locationConfig createDummyLocation()
{
    locationConfig loc;
    loc.name = "/test";
    loc.hasRoot = true;
    loc.root = ".";  // Racine locale
    loc.allowedMethods.push_back("GET");
    loc.index = "index.html";
    loc.autoIndex = false;
    loc.clientMaxBodySize = 1000000;
    loc.hasRedir = false;
    loc.uploadEnabled = false;
    return loc;
}

// Prépare la conversation pour un test donné
void setupConversation(Conversation& conv, const std::string& method, const std::string& pathOnDisk, bool autoIndex = false)
{
    conv.req.method = method;
    conv.req.pathOnDisk = pathOnDisk;
    conv.req.uri = pathOnDisk;  // Simplification

    static locationConfig loc;
    loc.autoIndex = autoIndex;
    loc.index = "";  // Pas d'index

    conv.location = &loc;

    // Reset response et états
    conv.resp = response();
    conv.state = PARSE;
    conv.eState = EXEC_START;
}

// Créé un dossier vide (supprime son contenu)
void setupEmptyDir(const std::string& dirName)
{
    struct stat st;
    if (stat(dirName.c_str(), &st) == -1) {
        mkdir(dirName.c_str(), 0755);
    }
    std::string cmd = "rm -rf " + dirName + "/*";
    system(cmd.c_str());
}

// Crée un dossier avec un fichier à l'intérieur
void setupDirWithFile(const std::string& dirName, const std::string& fileName)
{
    struct stat st;
    if (stat(dirName.c_str(), &st) == -1) {
        mkdir(dirName.c_str(), 0755);
    }
    std::ofstream file((dirName + "/" + fileName).c_str());
    file << "Hello Autoindex Test\n";
    file.close();
}

int main()
{
    Conversation conv;

    // --- Test 0 : dummy location tests
    locationConfig dummyLoc = createDummyLocation();
    conv.location = &dummyLoc;

    conv.req.method = "GET";
    conv.req.pathOnDisk = "test_page.html";
    GetExecutor::handleGet(conv);
    std::cout << "[Test 0] Dummy Location GET test_page.html: "
              << ((conv.resp.status == OK) ? "PASS" : ("FAIL (" + intToString(conv.resp.status) + ")")) << std::endl;

    conv.req.pathOnDisk = "forbidden_file.html";
    GetExecutor::handleGet(conv);
    std::cout << "[Test 0] Dummy Location GET forbidden_file.html: "
              << ((conv.resp.status == FORBIDDEN) ? "PASS" : ("FAIL (" + intToString(conv.resp.status) + ")")) << std::endl;

    // --- Test 1 : fichier existant (valide)
    setupConversation(conv, "GET", "test_page.html");
    GetExecutor::handleGet(conv);
    std::cout << "[Test 1] fichier existant: "
              << ((conv.resp.status == OK) ? "PASS" : ("FAIL (" + intToString(conv.resp.status) + ")")) << std::endl;

    // --- Test 2 : fichier inexistant (404 attendu)
    setupConversation(conv, "GET", "non_existent_file.html");
    GetExecutor::handleGet(conv);
    std::cout << "[Test 2] fichier inexistant: "
              << ((conv.resp.status == NOT_FOUND) ? "PASS" : ("FAIL (" + intToString(conv.resp.status) + ")")) << std::endl;

    // --- Test 3a : dossier autoindex VIDE
    setupEmptyDir("directory_test");
    setupConversation(conv, "GET", "directory_test", true);
    GetExecutor::handleGet(conv);
    std::cout << "[Test 3a] dossier autoindex VIDE : "
              << ((conv.resp.status == OK && conv.resp.contentType == "text/html" && !conv.resp.body.empty()) ? "PASS" : "FAIL") << std::endl;

    // --- Test 3b : dossier autoindex AVEC fichier
    setupDirWithFile("directory_test", "sample.txt");
    setupConversation(conv, "GET", "directory_test", true);
    GetExecutor::handleGet(conv);
    bool containsSample = conv.resp.body.find("sample.txt") != std::string::npos;
    std::cout << "[Test 3b] dossier autoindex AVEC fichier : "
              << ((conv.resp.status == OK && conv.resp.contentType == "text/html" && containsSample) ? "PASS" : "FAIL") << std::endl;

    // --- Test 4 : fichier interdit
    setupConversation(conv, "GET", "forbidden_file.html");
    GetExecutor::handleGet(conv);
    std::cout << "[Test 4] fichier interdit: "
              << ((conv.resp.status == FORBIDDEN) ? "PASS" : ("FAIL (" + intToString(conv.resp.status) + ")")) << std::endl;

    return 0;
}
