#include <iostream>
#include <cassert>
#include "Formatter.hpp"
#include "ConfigFileReader.hpp"
#include "CommentsRemover.hpp"

// void test_isEmptyLine() {
// 	Formatter f;

// 	assert(f.isEmptyLine("") == true);
// 	assert(f.isEmptyLine("    ") == true);
// 	assert(f.isEmptyLine("\t\t  ") == true);
// 	assert(f.isEmptyLine("a") == false);
// 	assert(f.isEmptyLine("  b") == false);

// 	std::cout << "✅ test_isEmptyLine passed" << std::endl;
// }

// void test_removeSpaces() {
// 	Formatter f;

// 	std::string input = "   hello\n\t\tworld\n  		foo bar\n";
// 	std::string expected = "hello\nworld\nfoo bar\n";
// 	assert(f.removeSpaces(input) == expected);

// 	std::cout << "✅ test_removeSpaces passed" << std::endl;
// }

// void test_removeEmptyLines() {
// 	Formatter f;

// 	std::string input = "line 1\n\n   \nline 2\n";
// 	std::string expected = "line 1\nline 2\n";
// 	assert(f.removeEmptyLines(input) == expected);

// 	std::cout << "✅ test_removeEmptyLines passed" << std::endl;
// }

// void test_splitContent() {
// 	Formatter f;

// 	std::string input = "line1\nline2\nline3\n";
// 	std::vector<std::string> expected;
// 	expected.push_back("line1");
// 	expected.push_back("line2");
// 	expected.push_back("line3");

// 	std::vector<std::string> result = f.splitContent(input);
// 	assert(result == expected);

// 	std::cout << "✅ test_splitContent passed" << std::endl;
// }

// void test_format() {
// 	Formatter f;

// 	std::string input =
// 		"\n"
// 		"   line1\n"
// 		" \t line2\n"
// 		"\n"
// 		"line3\n"
// 		"\n";
// 	std::vector<std::string> expected;
// 	expected.push_back("line1");
// 	expected.push_back("line2");
// 	expected.push_back("line3");

// 	std::vector<std::string> result = f.format(input);
// 	assert(result == expected);

// 	std::cout << "✅ test_format passed" << std::endl;
// }

// int main() {
// 	test_isEmptyLine();
// 	test_removeSpaces();
// 	test_removeEmptyLines();
// 	test_splitContent();
// 	test_format();

// 	std::cout << "\n🎉 All Formatter tests passed!\n" << std::endl;
// 	return 0;
// }

int	main(int ac, char **av) {

	if (ac !=2) {
		return (EXIT_FAILURE);
	}

	try {
		ConfigFileReader reader;
		std::string rawContent = reader.loadConfigContent(av[1]);

		CommentsRemover remover;
		std::string clearContent = remover.remove(rawContent);

		Formatter formatter;
		std::vector<std::string> formatted = formatter.format(clearContent);
		for (std::vector<std::string>::iterator it = formatted.begin(); it != formatted.end(); ++it) {
			std::cout << *it << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return (EXIT_SUCCESS);
}