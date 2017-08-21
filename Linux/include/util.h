#pragma once
#include <vector>
#include <regex>
#include <string>
#include <memory>
#include "Table.h"

class Section;
class Symbol;

class util {
public:

	static std::vector<std::string> tokenizeString(const std::string& str, const std::string& delimiters);

	static bool readFile(std::string fileName, std::vector<std::vector<std::string>> &instructions);

	static void writeToFile(std::string fileName);

public: // first pass

	static bool firstPass(std::vector<std::vector<std::string>> &instructions);

	static uint32_t handleOrgDirective(std::vector<std::string> line);
	
	static uint32_t handleDupDirective(std::vector<std::string> line, std::shared_ptr<Section> &currentSection);

	static uint32_t defValue(std::vector<std::string> line);

	static uint32_t readValue(std::string value, std::shared_ptr<Section> currentSection = std::shared_ptr<Section>(nullptr));

public: // second pass

	static bool secondPass(std::vector<std::vector<std::string>> &instruction);

	static void updateGlobalFlag(std::vector<std::string> &line);

public: // regex

	static std::regex register_regex;
	static std::regex regindpom_regex;
	static std::regex regind_regex;
	static std::regex immed_regex;
	static std::regex memdir_regex;

	static Table<Section> sectab;
	static Table<Symbol> symtab;
};


