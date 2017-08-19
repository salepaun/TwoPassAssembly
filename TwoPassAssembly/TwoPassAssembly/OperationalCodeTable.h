#pragma once
#include <unordered_map>
#include <memory>
#include "SymbolTable.h"

class Mnemonic;
class Section;

class OperationalCodeTable
{
public:
	static void init();

	static bool find(std::string op);
	static std::shared_ptr<Mnemonic> get(std::string op);

	static uint8_t getAddressMode(std::vector<std::string>& instruction);
	static uint8_t getAddressModes(std::string op);
	static uint8_t checkInstruction(std::string op, std::vector<std::string> line);
	static std::vector<std::string> getInstruction(std::string op, std::vector<std::string> line);
	static uint32_t constructInstruction(std::vector<std::string> inst);
	static uint32_t constructSecondWord(std::vector<std::string> inst, std::shared_ptr<Section> currentSection);
private:
	static std::unordered_map<std::string, std::shared_ptr<Mnemonic>> _map;
};

typedef OperationalCodeTable OPTAB;