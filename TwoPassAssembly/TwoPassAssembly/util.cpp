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
#include "RelocationEnty.h"


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
	auto i = 0;
	while (std::getline(infile, line))
	{
		auto tokens = util::tokenizeString(line, delimeters);
		if (tokens.empty())continue;
		instructions.push_back(tokens);
		++i;
	}
	infile.close();
	if (instructions.size() == 0) { PRINT("Empty file"); return false; }
	return true;

}

void util::writeToFile(std::string fileName)
{
	std::ofstream outputFile(fileName);

	std::string output = "#TabelaSimbola\n";
	output += SECTAB::table.to_string();
	
	for (auto &symbol : SYMTAB::table)
		if (!symbol->isSection())
			output += symbol->to_string() + "\n";

	for(auto &section: SECTAB::table)
	{
		//output += section->to_string_table();
		auto table = section->getTable();
		if (table.size()) output += "#rel" + section->getName() + "\n";
		for(auto &entry:section->getTable())
		{
			output += entry->to_string() + "\n";
		}
		output += section->to_string_data() + "\n";
	}

	outputFile << output;

	outputFile.close();
}

uint32_t util::handleOrgDirective(std::vector<std::string> line)
{
	auto value = false;
	uint8_t i = 0;
	for (i = 0; i < line.size(); ++i)
	{
		if (line[i] == "ORG") break;
	}
	std::string value_str = "";
	for (++i; i < line.size(); ++i)
	{
		value_str += line[i];
	}

	return util::readValue(value_str);
}

uint32_t util::handleDupDirective(std::vector<std::string> line, std::shared_ptr<Section>& currentSection)
{
	auto value = false;
	uint8_t i = 0;
	for (i = 0; i < line.size(); ++i)
	{
		if (line[i] == "DUP") break;
	}
	std::string value_str = "";
	for (++i; i < line.size(); ++i)
	{
		value_str += line[i];
	}

	return util::readValue(value_str);
}

uint32_t util::defValue(std::vector<std::string> line)
{
	uint8_t i = 0;
	for (;;)
		if (line[i++] == "DEF")
			break;
	std::string expression = "";
	while (i < line.size())
		expression += line[i++];
	
	return util::readValue(expression);

}

uint32_t util::readValue(std::string value, std::shared_ptr<Section> currentSection)
{
	if (value.find("?") != std::string::npos)return 0;

	if (math_expression::isExpression(value)) {
		while (value.size() > 0)
		{
			auto pair = math_expression::getNext(value);
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

		//for (auto &pair : math_expression::_queue)
		//PRINT(pair.first);

		return math_expression::calculatePostfix();
	}

	if (math_expression::isBinary(value))
	{
		return strtoul(value.substr(2).c_str(), nullptr, 2);
	}

	if (math_expression::isHexFormat(value))
	{
		return strtoul(value.substr(2).c_str(), nullptr, 16);
	}
	if(isdigit(value[0]))
		return strtoul(value.c_str(), nullptr, 10);
	auto symbol = SYMTAB::table.find(value);
	auto ret = 0;
	std::shared_ptr<RelocationEnty> entry;
	if (!symbol->isDefSymbol())
	{
		if (symbol->isGlobal())
			entry = std::make_shared<RelocationEnty>(currentSection->getLocationCounter(), relocation_type::R_386_32, symbol->getIndex());
		else
		{
			entry = std::make_shared<RelocationEnty>(currentSection->getLocationCounter(), relocation_type::R_386_32, symbol->getSection()->getIndex());
			ret = symbol->getOffset();
		}
	}
	else
		ret = symbol->getOffset();
	
	if(entry)
		currentSection->insertRealocationEntry(entry);

	return ret;
}

bool util::firstPass(std::vector<std::vector<std::string>> &instructions)
{
	auto currentSection = std::shared_ptr<Section>(nullptr);
	
	uint16_t value = 0;
	bool def_flag = false;
	bool dup_flag = false;
	bool org = false;
	//auto ofset = 0;
	for (auto& line : instructions)
	{
		uint8_t inst = 0;
		if (std::find(std::begin(line), std::end(line), "DEF") != std::end(line))
			def_flag = true;
		if (std::find(std::begin(line), std::end(line), "DUP") != std::end(line))
			dup_flag = true;
		for (auto& word : line)
		{
			//if (currentSection) ofset = currentSection->getLocationCounter() - currentSection->getBeginLocationCounter();
			if (word == ".end")
				return true;

			if (word == ".global") break;

			if (word == "ORG")
			{
				org = true;
				value = util::handleOrgDirective(line);
				break;
			}
			// Check if is label
			if (word.find(":") != std::string::npos)
			{
				if (inst & INSTRUCTION) { PRINT("Label must come at the begining of the line"); return false; }
				if (!currentSection) { PRINT("Current section is nullptr"); return false; }

				auto label = word.substr(0, word.length() - 1);
				//std::string print = "labela " + label + " se nalazi u sekciji " + currentSection->getName();
				//PRINT(print);
				auto symbol = std::make_shared<Symbol>(label, currentSection, currentSection->getLocationCounter()-currentSection->getBeginLocationCounter());
				symbol->setIndex(uint16_t(SYMTAB::table.size())+1);
				SYMTAB::table.insert(symbol);
				continue;
			}

			// Check if is a section
			if (word[0] == '.')
			{
				auto name = word;
				auto section = std::make_shared<Section>(name, SYMTAB::table.size() + 1, value, org);
				org = false;
				value = 0;
				
				SECTAB::table.insert(section);
				currentSection = section;

				auto symbol = std::make_shared<Symbol>(name, currentSection, currentSection->getLocationCounter(),true);
				symbol->setIndex(uint16_t(SYMTAB::table.size())+1);
				SYMTAB::table.insert(symbol);
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
				if (dup_flag)
				{
					currentSection->incLocationCounter(Definition::getSize(word) * util::readValue(line[index+1], currentSection));
					dup_flag = false;
					break;
				}
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
				if (word == "RET")bytes = 4;
				if (bytes == -1) { PRINT("Error ocured"); return false; }
				currentSection->incLocationCounter(bytes);
				break;
			}

			if (def_flag)
			{
				// auto symbol_value = util::defValue(line, symtab);
				auto symbol = std::make_shared<Symbol>(word, currentSection, 0, false, true);
				symbol->setIndex(uint16_t(SYMTAB::table.size())+1);
				SYMTAB::table.insert(symbol);
				def_flag = false;
				break;
			}

			
		}
	}
	return true;
}

bool util::secondPass(std::vector<std::vector<std::string>>& instructions)
{
	for (auto &sec : SECTAB::table)
		sec->saveAndResetLocctr();

	for (auto&line : instructions)
		updateGlobalFlag(line);

	auto currentSection = std::shared_ptr<Section>(nullptr);
	bool def_flag = false;
	bool dup_flag = false;
	for (auto& line : instructions)
	{
		auto inst = 0;
		if (std::find(std::begin(line), std::end(line), "DEF") != std::end(line))
			def_flag = true;
		if (std::find(std::begin(line), std::end(line), "DUP") != std::end(line))
			dup_flag = true;
		//if (currentSection)std::cout << std::hex << currentSection->getLocationCounter() << "   ->   " << line[0] << std::endl;
		inst = 0;
		for (auto& word : line)
		{
			if (word == ".end")
			{
				//std::cout << currentSection->to_string_data() << "\n" << currentSection->to_string_table() << "\n\n";
				return true;

			}
			if (word == ".global")
			{
				//util::updateGlobalFlag(line);
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
				//if (currentSection) 
				//	std::cout << currentSection->to_string_data() << "\n" << currentSection->to_string_table() << "\n\n";
				auto name = word;
				currentSection = SECTAB::table.find(word);
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
				
				if (dup_flag)
				{
					auto iterations = util::readValue(line[index+1] , currentSection);
					auto value = util::handleDupDirective(line, currentSection);
					for (uint32_t i = 0; i < iterations; ++i)
						currentSection->appendData(value, Definition::getSize(word), false);
					dup_flag = false;
					break;
				}
				auto amount = Definition::getSize(word) * (line.size() - index - 1);
				for (uint8_t i = index + 1; i < line.size(); ++i)
				{
					// TODO realokacija
					auto value = readValue(line[i],currentSection);
					currentSection->appendData(value, Definition::getSize(word),false);
				}
				break;
			}

			// Check if instruction
			if (OPTAB::find(word))
			{
				
				auto bytes = OPTAB::checkInstruction(word, line);
				auto instruction = OPTAB::getInstruction(word, line);
				auto first_32 = OPTAB::constructInstruction(instruction);
				currentSection->appendData(first_32, 4);

				if (bytes == 8) currentSection->appendData(OPTAB::constructSecondWord(instruction, currentSection), 4, false);
				break;
			}

			if (def_flag)
			{
				auto symbol_value = util::defValue(line);
				auto symbol = SYMTAB::table.find(word);
				symbol->setOffset(symbol_value);
				def_flag = false;
				break;
			}

			
		}
	}
	std::cout << currentSection->to_string_data() << "\n" << currentSection->to_string_table() << "\n\n";

	return true;
}

void util::updateGlobalFlag(std::vector<std::string>& line)
{
	auto global = false;
	for (auto &word2 : line)
	{
		if (global)
		{
			auto symbol = SYMTAB::table.find(word2);
			symbol->setGlobal(true);
		}
		if (word2 == ".global")global = true;
	}
}

std::regex util::register_regex ("^(R([0-9]||1[0-5])||SP||PC)$");
std::regex util::regindpom_regex("^\\[(R([0-9]||1[0-5])||PC||SP) *(\\+||\\-) *(0x||0b)?[a-zA-z0-9]+\\]$");
//std::regex util::regindpom_regex("^(\\[(R([0-9]||1[0-5])||PC||SP) ?(\\+||\\-) *\\]||\\$.*)$");
std::regex util::regind_regex	("^\\[(R([0-9]||1[0-5])||PC||SP)\\]$");
//std::regex util::immed_regex	("#([0-9A-F]+||[a-zA-z][a-zA-Z0-9]*)");
std::regex util::immed_regex("^#([0-9A-F]+||[a-zA-z][a-zA-Z0-9]*)");
std::regex util::memdir_regex	("^\\$?[a-zA-Z][a-zA-Z0-9]*$");