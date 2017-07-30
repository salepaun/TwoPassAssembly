#include "util.h"
#include "macros.h"
#include <fstream>
#include <string>
#include "OperationalCodeTable.h"
#include "Table.h"
#include "SymbolTable.h"
#include "Section.h"
#include "Symbol.h"
#include "SectionTable.h"
#include "Definition.h"

// argv[1] = input file
// argv[2] = output file
int main(int argc, char* argv[])
{
	if (argc != 3) { PRINT("Bad arguments"); }

	//PRINT(std::regex_match("[R0 + a12]", util::regindpom_regex));

	std::vector<std::vector<std::string>> instructions;

	std::ifstream infile;
	infile.open(argv[1]);
	std::string delimeters = " ,";
	std::string line;
	while(std::getline(infile,line))
	{
		auto tokens = util::tokenizeString(line, delimeters);
		if (tokens.empty())continue;
		instructions.push_back(tokens);
	}
	infile.close();
	if (instructions.size() == 0) { PRINT("Empty file"); return 0; }

	OPTAB::init();
	SYMTAB symtab;
	SECTAB sectab;
	auto currentSection = std::shared_ptr<Section>(nullptr);

	for(auto& line :instructions)
	{
		uint8_t inst = 0;
		for(auto& word: line)
		{
			if(word == ".end")
			{
				// TODO kraj prvog prolaza
				return 0;
			}

			if (word == ".global") break;

			// Check if is label
			if (word.find(":") != std::string::npos)
			{
				if (inst & INSTRUCTION) { PRINT("Label must come at the begining of the line"); return -1; }
				if (!currentSection) { PRINT("Current section is nullptr"); return -1; }
				
				auto label = word.substr(0, word.length() - 1);
				auto symbol = std::make_shared<Symbol>(label, currentSection, currentSection->getLocationCounter());
				symbol->setIndex((uint16_t)symtab.size());
				symtab.insert(symbol);
				continue;
			}

			// Check if is a section
			if(word[0]=='.')
			{
				auto name = word.substr(1);
				auto section = std::make_shared<Section>(name);
				sectab.insert(section);
				currentSection = section;
				continue;
			}

			// Check if definition
			if(Definition::contains(word))
			{
				auto index = std::distance(
					std::begin(line), std::find(
						std::begin(line), std::end(line), word
					)
				);
				auto amount = Definition::getSize(word) * (line.size() - index - 1);
				currentSection->incLocationCounter(amount);
				continue;
			}

			// Check if instruction
			if(OPTAB::find(word))
			{
				if (inst& INSTRUCTION > 0) { PRINT("Instruction mneumonic already found in this line"); return -1; }
				inst &= INSTRUCTION;
				auto bytes = OPTAB::checkInstruction(word, line);
				if (bytes == -1) { PRINT("Error ocured"); return -1; }
				currentSection->incLocationCounter(bytes);
			}
		}
		PRINT("");
	}
	
	return 0;
}