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

	if (!util::readFile(argv[1], instructions))
		return -1;

	OPTAB::init();
	SYMTAB symtab;
	SECTAB sectab;
	
	if (!util::firstPass(symtab, sectab, instructions))
		return -1;

	if (!util::secondPass(symtab, sectab, instructions))
		return -1;

	std::cout << sectab;
	std::cout << symtab;

	return 0;
}