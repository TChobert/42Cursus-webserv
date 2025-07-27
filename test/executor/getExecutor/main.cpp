#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>  // pour system()
#include <string>
#include <vector>

#include "GetExecutor.hpp"
#include "webserv.hpp"
#include "Executor.hpp"

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

void testCgiFakeRead() {
    Conversation conv;

    // Simule un CGI qui aurait été lu en entier
    conv.cgiOutput =
        "Status: 200\r\n"
        "Content-Type: text/html\r\n"
        "Set-Cookie: id=xyz\r\n"
        "\r\n"
        "<html><body><p>Hello CGI</p></body></html>";

    CGIHandler::parseCgiOutput(conv);
    Executor::updateResponseData(conv);

    std::cout << "[Test CGI Fake Read] ";
    if (conv.resp.status == OK &&
        conv.resp.contentType == "text/html" &&
        conv.resp.body.find("Hello CGI") != std::string::npos &&
        !conv.resp.setCookies.empty())
    {
        std::cout << "PASS" << std::endl;
    }
    else
    {
        std::cout << "FAIL: "
                  << "status=" << conv.resp.status
                  << ", contentType=" << conv.resp.contentType
                  << ", body=" << conv.resp.body
                  << std::endl;
    }
}

void testCgiStatus404() {
    Conversation conv;
    conv.cgiOutput =
        "Status: 404\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<body>Not Found</body>";

    CGIHandler::parseCgiOutput(conv);

    std::cout << "[Test Status 404] ";
    if (conv.resp.status == 404 &&
        conv.resp.contentType == "text/html" &&
        conv.resp.body.find("Not Found") != std::string::npos)
        std::cout << "PASS\n";
    else
        std::cout << "FAIL: status=" << conv.resp.status << ", contentType=" << conv.resp.contentType << ", body=" << conv.resp.body << "\n";
}

void testCgiNoStatusHeader() { //attention au constructeur de la reponse.. pas bonne init de statusCode, qui devrait etre a 0
    Conversation conv;
	std::cout << "Status initial avant parsing 1.0: " << conv.resp.status << std::endl;

    conv.cgiOutput =
        "Content-Type: text/plain\r\n"
        "\r\n"
        "Hello world!";

	std::cout << "Status initial avant parsing: " << conv.resp.status << std::endl;
    CGIHandler::parseCgiOutput(conv);
	std::cout << "Status after parsing: " << conv.resp.status << "\n";

    std::cout << "[Test No Status Header] ";
    if (conv.resp.status == OK &&
        conv.resp.contentType == "text/plain" &&
        conv.resp.body == "Hello world!")
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

void testCgiMalformedHeader() { //a voir si on veut ca.. ?? ou si erreur 500 ?
    Conversation conv;
    conv.cgiOutput =
        "Content-Type text/html\r\n"
        "Set-Cookie=sessionid=123\r\n"
        "\r\n"
        "OK!";

    CGIHandler::parseCgiOutput(conv);

    std::cout << "[Test Malformed Header] ";
    if (conv.resp.setCookies.empty()) // pas de cookie ajouté
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

void testCgiHeaderSpacing() {
    Conversation conv;
    conv.cgiOutput =
        "Content-Type    :   text/html\r\n"
        "Status : 500\r\n"
        "\r\n"
        "<body>Error</body>";

    CGIHandler::parseCgiOutput(conv);

    std::cout << "[Test Header Spacing] ";
    if (conv.resp.status == 500 &&
        conv.resp.contentType == "text/html" &&
        conv.resp.body.find("Error") != std::string::npos)
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

void testCgiDoubleSetCookie() {
    Conversation conv;
    conv.cgiOutput =
        "Set-Cookie: a=1\r\n"
        "Set-Cookie: b=2\r\n"
        "\r\n"
        "OK";

    CGIHandler::parseCgiOutput(conv);

    std::cout << "[Test Double Set-Cookie] ";
    if (conv.resp.setCookies.size() == 2 &&
        conv.resp.setCookies[0] == "a=1" &&
        conv.resp.setCookies[1] == "b=2")
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
}

void testCgiNoCRLFSeparator() {
    Conversation conv;
    conv.cgiOutput =
        "Status: 200\r\n"
        "Content-Type: text/plain\r\n"
        "No double CRLF";

    CGIHandler::parseCgiOutput(conv);

    std::cout << "[Test Missing CRLF] ";
    if (conv.resp.status == INTERNAL_SERVER_ERROR)
        std::cout << "PASS\n";
    else
        std::cout << "FAIL: status=" << conv.resp.status << "\n";
}

void testCgiEmptyOutput() {
    Conversation conv;
    conv.cgiOutput = "";

    CGIHandler::parseCgiOutput(conv);

    std::cout << "[Test Empty Output] ";
    if (conv.resp.status == INTERNAL_SERVER_ERROR)
        std::cout << "PASS\n";
    else
        std::cout << "FAIL\n";
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

	// Test CGI
	testCgiFakeRead();
	testCgiStatus404();
    testCgiNoStatusHeader();
    testCgiMalformedHeader();
    testCgiHeaderSpacing();
    testCgiDoubleSetCookie();
    testCgiNoCRLFSeparator();
    testCgiEmptyOutput();

    return 0;
}
