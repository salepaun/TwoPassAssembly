#include <vector>
#include "util.h"
#include "macros.h"
#include "SectionTable.h"
#include "SymbolTable.h"
#include "Definition.h"
#include "OperationalCodeTable.h"
#include "Section.h"
#include "Symbol.h"
#include <fstream>
#include "math_expression.h"


std::vector<std::string> util::tokenizeString(const std::string& str, const std::string& delimiters)
{
	std::vector<std::string> tokens;
	if (str.empty())return tokens;
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{  // Found a token, add it to the vector.
		std::string sub = str.substr(lastPos, pos - lastPos);
		if (sub.find(";") != std::string::npos) return tokens;
		if (sub.find("[") != std::string::npos)
		{
			auto first = str.find_first_of("[");
			auto last = str.find_last_of("]");
			tokens.push_back(str.substr(first, last - first + 1));
			return tokens;
		}
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
	return tokens;
}

bool util::readFile(std::string fileName, std::vector<std::vector<std::string>>& instructions)
{
	std::ifstream infile;
	infile.open(fileName);
	std::string delimeters = " ,\t";
	std::string line;
	while (std::getline(infile, line))
	{
		auto tokens = util::tokenizeString(line, delimeters);
		if (tokens.empty())continue;
		instructions.push_back(tokens);
	}
	infile.close();
	if (instructions.size() == 0) { PRINT("Empty file"); return false; }
	return true;

}

uint16_t util::handleOrgDirective(std::vector<std::string>& line, SECTAB & sectab)
{
	auto value = false;
	for (auto &word : line)
	{
		if(value) return uint16_t(strtoul(word.c_str(), nullptr, 16));
		if (word == "ORG") value = true;
	}
	return 0;
}

uint32_t util::defValue(std::vector<std::string>& line, SYMTAB &symtab)
{
	auto i = 0;
	for (;;)
		if (line[i++] == "DEF")
			break;
	std::string expression = "";
	while (i < line.size())
		expression += line[i++];

	while (expression.size() > 0)
	{
		auto pair = math_expression::getNext(expression);
		switch (pair.second)
		{
		case math_type::number:
			math_expression::_queue.push_back(pair);
			break;
		case math_type::variable:
			math_expression::_queue.push_back(pair);
			break;
		case math_type::operation:
			while (math_expression::shouldPop(pair.first))
				math_expression::pushFromStack();

			math_expression::_operation_stack.push(pair);
			break;
		case math_type::left_bracket:
			math_expression::_operation_stack.push(pair);
			break;
		case math_type::right_bracket:
			while (!math_expression::isLeftBracketAtTop())
				math_expression::pushFromStack();
			math_expression::_operation_stack.pop();
			break;
		}
	}

	while (!math_expression::_operation_stack.empty())
		math_expression::pushFromStack();

	for (auto &pair : math_expression::_queue)
		PRINT(pair.first);

	return math_expression::calculatePostfix(symtab);
}

bool util::firstPass(SYMTAB &symtab, SECTAB &sectab, std::vector<std::vector<std::string>> &instructions)
{
	auto currentSection = std::shared_ptr<Section>(nullptr);
	
	uint16_t value = 0;
	bool def_flag = false;
	for (auto& line : instructions)
	{
		uint8_t inst = 0;
		if (std::find(std::begin(line), std::end(line), "DEF") != std::end(line))
			def_flag = true;
		for (auto& word : line)
		{
			if (word == ".end")
				return true;

			if (word == ".global") break;

			if (word == "ORG")
			{
				value = util::handleOrgDirective(line, sectab);
				continue;
			}
			// Check if is label
			if (word.find(":") != std::string::npos)
			{
				if (inst & INSTRUCTION) { PRINT("Label must come at the begining of the line"); return false; }
				if (!currentSection) { PRINT("Current section is nullptr"); return false; }

				auto label = word.substr(0, word.length() - 1);
				//std::string print = "labela " + label + " se nalazi u sekciji " + currentSection->getName();
				//PRINT(print);
				auto symbol = std::make_shared<Symbol>(label, currentSection, currentSection->getLocationCounter());
				symbol->setIndex(uint16_t(symtab.size()));
				symtab.insert(symbol);
				continue;
			}

			// Check if is a section
			if (word[0] == '.')
			{
				auto name = word;
				auto section = std::make_shared<Section>(name, symtab.size(), value);
				value = 0;
				
				sectab.insert(section);
				currentSection = section;

				auto symbol = std::make_shared<Symbol>(name, currentSection, currentSection->getLocationCounter(),true);
				symbol->setIndex(uint16_t(symtab.size()));
				symtab.insert(symbol);
				//std::string print = "nova sekcija " + name;
				//PRINT(print);
				break;
			}

			// Check if definition
			if (Definition::contains(word))
			{
				auto index = std::distance(
					std::begin(line), std::find(
						std::begin(line), std::end(line), word
					)
				);
				auto amount = Definition::getSize(word) * (line.size() - index - 1);
				currentSection->incLocationCounter(uint16_t(amount));
				break;
			}

			// Check if instruction
			if (OPTAB::find(word))
			{
				if (inst & INSTRUCTION) { PRINT("Instruction mneumonic already found in this line"); return false; }
				inst &= INSTRUCTION;
				auto bytes = OPTAB::checkInstruction(word, line);
				if (bytes == -1) { PRINT("Error ocured"); return false; }
				currentSection->incLocationCounter(bytes);
				break;
			}

			if (def_flag)
			{
				auto symbol_value = util::defValue(line, symtab);
				auto symbol = std::make_shared<Symbol>(word, currentSection, symbol_value, false, true);
				symbol->setIndex(uint16_t(symtab.size()));
				symtab.insert(symbol);
				break;
			}
		}
	}
	return true;
}

bool util::secondPass(SYMTAB & symtab, SECTAB & sectab, std::vector<std::vector<std::string>>& instructions)
{
	for (auto &sec : sectab)
		sec->saveAndResetLocctr();

	auto currentSection = std::shared_ptr<Section>(nullptr);
	for (auto& line : instructions)
	{
		uint8_t inst = 0;

		for (auto& word : line)
		{
			if (word == ".end")
				return true;

			if (word == ".global")
			{
				util::updateGlobalFlag(line, symtab);
				break;
			}

			if (word == "ORG")
				break;

			// Check if is label
			if (word.find(":") != std::string::npos)
				continue;

			// Check if is a section
			if (word[0] == '.')
			{
				auto name = word;
				currentSection = sectab.find(word);
				break;
			}

			// Check if definition
			if (Definition::contains(word))
			{
				auto index = std::distance(
					std::begin(line), std::find(
						std::begin(line), std::end(line), word
					)
				);
				auto amount = Definition::getSize(word) * (line.size() - index - 1);
				currentSection->incLocationCounter(uint16_t(amount));
				break;
			}

			// Check if instruction
			if (OPTAB::find(word))
			{
				if (inst & INSTRUCTION) { PRINT("Instruction mneumonic already found in this line"); return false; }
				inst &= INSTRUCTION;
				auto bytes = OPTAB::checkInstruction(word, line);
				if (bytes == -1) { PRINT("Error ocured"); return false; }
				currentSection->incLocationCounter(bytes);
				break;
			}
		}
	}
	return true;
}

void util::updateGlobalFlag(std::vector<std::string>& line, SYMTAB &symtab)
{
	auto global = false;
	for (auto &word2 : line)
	{
		if (global)
		{
			auto symbol = symtab.find(word2);
			symbol->setGlobal(true);
		}
		if (word2 == ".global")global = true;
	}
}

std::regex util::register_regex ("^(R([0-9]||1[0-5])||SP||PC)$");
std::regex util::regindpom_regex("^\\[(R([0-9]||1[0-5])||PC||SP) (\\+||\\-) [a-zA-z0-9]+\\]$");
std::regex util::regind_regex	("^\\[(R([0-9]||1[0-5])||PC||SP)\\]$");
std::regex util::immed_regex	("#([0-9A-F]+||[a-zA-z][a-zA-Z0-9]*)");
std::regex util::memdir_regex	("^[a-zA-Z][a-zA-Z0-9]+$");