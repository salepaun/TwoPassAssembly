#include "OperationalCodeTable.h"
#include "macros.h"
#include "Mnemonic.h"
#include <algorithm>
#include <unordered_map>

std::unordered_map<std::string, std::shared_ptr<Mnemonic>> OperationalCodeTable::_map;

bool OperationalCodeTable::find(std::string op)
{
	return _map.find(op) != std::end(_map);
}

std::shared_ptr<Mnemonic> OperationalCodeTable::get(std::string op)
{
	return _map.find(op)->second;
}

uint8_t OperationalCodeTable::getAddressMode(std::vector<std::string>& instruction)
{
	// TODO finish
	switch (instruction.size())
	{
	case 1: // RET
		return 0;
	case 2: // INT, JMP, CALL, PUSH, POP

	case 3:
		break;
	default:
		return -1;
	}
	return -1;
}

uint8_t OperationalCodeTable::getAddressModes(std::string op)
{
	auto mneumonic = OperationalCodeTable::get(op);
	return mneumonic->getModes();
}

uint8_t OperationalCodeTable::checkInstruction(std::string op,std::vector<std::string> line)
{
	auto mneumonic = std::find_if(std::begin(_map), std::end(_map), [&](auto inst) {return inst.first == op; });

	auto iterator = std::find(std::begin(line), std::end(line), op);

	auto instruction = std::vector<std::string>(line.size());

	std::copy(iterator, std::end(line), std::begin(instruction));

	auto address_mode = OperationalCodeTable::getAddressMode(instruction);

	if(address_mode & OperationalCodeTable::getAddressModes(op))
	{
		return Mnemonic::getInstructionSize(address_mode);
	}

	return -1;
}

void OperationalCodeTable::init()
{
	// Instrukcije za kontrolu toka
	std::string instruction = "INT";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x0, REGDIR));
	instruction = "JMP";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x2, MEMDIR | REGIND | REGINDPOM));
	instruction = "CALL";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x3, MEMDIR | REGIND | REGINDPOM));
	instruction = "RET";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x1, 0));
	instruction = "JZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x4, MEMDIR | REGIND | REGINDPOM));
	instruction = "JNZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x5, MEMDIR | REGIND | REGINDPOM));
	instruction = "JGZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x6, MEMDIR | REGIND | REGINDPOM));
	instruction = "JGEZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x7, MEMDIR | REGIND | REGINDPOM));
	instruction = "JLZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x8, MEMDIR | REGIND | REGINDPOM));
	instruction = "JLEZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x9, MEMDIR | REGIND | REGINDPOM));

	// LOAD/STORE Instrukcije
	instruction = "LOAD";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x10, MEMDIR | REGIND | REGINDPOM | IMMED | REGDIR));
	instruction = "STORE";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x11, MEMDIR | REGIND | REGINDPOM | REGDIR));

	// Instrukcije za rad sa stekom
	instruction = "PUSH";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x20, REGDIR));
	instruction = "POP";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x21, REGDIR));

	//Aritmeticke i logicke instrukcije
	instruction = "ADD";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x30, REGDIR));
	instruction = "SUB";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x31, REGDIR));
	instruction = "MUL";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x32, REGDIR));
	instruction = "DIV";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x33, REGDIR));
	instruction = "MOD";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x34, REGDIR));
	instruction = "AND";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x35, REGDIR));
	instruction = "OR";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x36, REGDIR));
	instruction = "XOR";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x37, REGDIR));
	instruction = "NOT";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x38, REGDIR));
	instruction = "ASL";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x39, REGDIR));
	instruction = "ASR";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x3A, REGDIR));
}


