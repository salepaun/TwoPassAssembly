#pragma once
#include <vector>
#include <regex>
#include "SectionTable.h"
#include "SymbolTable.h"


class util {
public:

	static std::vector<std::string> tokenizeString(const std::string& str, const std::string& delimiters);

public: // first pass

	static bool firstPass(SYMTAB &symtab, SECTAB &sectab, std::vector<std::vector<std::string>> &instructions);

	static bool readFile(std::string fileName, std::vector<std::vector<std::string>> &instructions);

	static uint16_t handleOrgDirective(std::vector<std::string>& line, SECTAB &sectab);

	static uint32_t defValue(std::vector<std::string>& line, SYMTAB &symtab);

public: // second pass

	static bool secondPass(SYMTAB &symtab, SECTAB &sectab, std::vector<std::vector<std::string>> &instruction);

	static void updateGlobalFlag(std::vector<std::string> &line, SYMTAB &symtab);

public: // regex

	static std::regex register_regex;
	static std::regex regindpom_regex;
	static std::regex regind_regex;
	static std::regex immed_regex;
	static std::regex memdir_regex;

};


