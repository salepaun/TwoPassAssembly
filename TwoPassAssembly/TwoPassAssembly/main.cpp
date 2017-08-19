#include "util.h"
#include "macros.h"
#include <fstream>
#include "OperationalCodeTable.h"
#include "Section.h"



// argv[1] = input file
// argv[2] = output file
int main(int argc, char* argv[])
{
	if (argc != 3) { PRINT("Bad arguments");  return -1; }

	//PRINT(std::regex_match("$h", util::memdir_regex));

	std::vector<std::vector<std::string>> instructions;

	if (!util::readFile(argv[1], instructions))
		return -1;

	OPTAB::init();
	
	if (!util::firstPass(instructions))
		return -1;

	if (!util::secondPass(instructions))
		return -1;

	util::writeToFile(argv[2]);
	PRINT("Successful!");
	return 0;
}