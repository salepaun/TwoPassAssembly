#include "RelocationEnty.h"
#include "OperationalCodeTable.h"
#include "macros.h"
#include "math_expression.h"
#include "Mnemonic.h"
#include "util.h"
#include "Symbol.h"
#include <algorithm>
#include <unordered_map>
#include "RelocTable.h"
#include "Section.h"

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
	auto op = instruction[instruction.size() - 1];

	if (std::regex_match(op, util::register_regex))
		return REGDIR;
	if (std::regex_match(op, util::regind_regex))
		return REGIND;
	if (std::regex_match(op, util::regindpom_regex))
		return REGINDPOM;
	if (std::regex_match(op, util::immed_regex))
		return IMMED;

	return MEMDIR;
}

uint8_t OperationalCodeTable::getAddressModes(std::string op)
{
	auto mneumonic = OperationalCodeTable::get(op);
	return mneumonic->getModes();
}

uint8_t OperationalCodeTable::checkInstruction(std::string op,std::vector<std::string> line)
{
	auto mneumonic = std::find_if(std::begin(_map), std::end(_map), [&](auto inst) {return inst.first == op; });

	auto instruction = OperationalCodeTable::getInstruction(op, line);

	auto address_mode = OperationalCodeTable::getAddressMode(instruction);

	if(address_mode & OperationalCodeTable::getAddressModes(op))
	{
		return Mnemonic::getInstructionSize(address_mode);
	}

	return -1;
}

std::vector<std::string> OperationalCodeTable::getInstruction(std::string op, std::vector<std::string> line)
{
	auto iterator = std::find(std::begin(line), std::end(line), op);

	auto instruction = std::vector<std::string>(line.size() - std::distance(std::begin(line), iterator));

	std::copy(iterator, std::end(line), std::begin(instruction));

	return  instruction;
}

uint32_t OperationalCodeTable::constructInstruction(std::vector<std::string> inst)
{
	auto mneumonic = OperationalCodeTable::get(inst[0]);
	return mneumonic->constructInstruction(inst);
}

uint32_t OperationalCodeTable::constructSecondWord(std::vector<std::string> inst, std::shared_ptr<Section> currentSection)
{
	auto op = inst[inst.size() - 1];
	uint32_t ret = 0;
	auto dolar = op[0] == '$';
	if (dolar)op = op.substr(1);
	std::shared_ptr<RelocationEnty> entry = std::shared_ptr<RelocationEnty>(nullptr);
	switch (OPTAB::getAddressMode(inst))
	{
	case MEMDIR:
	{
		auto symbol = SYMTAB::table.find(op);
		auto section = symbol->getSection();
		if (dolar)
		{
			if (currentSection == section)
			{
				ret = symbol->getOffset() - currentSection->getLocationCounter() - 4 + currentSection->getBeginLocationCounter(); // nema relokacionog zapisa
			}
			else
				if (symbol->isGlobal())
				{
					ret = -4;
					entry = std::make_shared<RelocationEnty>(currentSection->getLocationCounter(), relocation_type::R_386_PC32, symbol->getIndex());
				}
				else
				{
					ret = symbol->getOffset() - 4;
					entry = std::make_shared<RelocationEnty>(currentSection->getLocationCounter(), relocation_type::R_386_PC32, section->getIndex());
				}
		}
		else
		{
			
			// apsolutni
			if (symbol->isGlobal())
				entry = std::make_shared<RelocationEnty>(currentSection->getLocationCounter(), relocation_type::R_386_32, symbol->getIndex());
			else
			{
				entry = std::make_shared<RelocationEnty>(currentSection->getLocationCounter(), relocation_type::R_386_32, section->getIndex());
				ret = symbol->getOffset();
			}
		}
		break;
	}
		case REGINDPOM:
		{
			auto pos = op.find_first_of("+-");
			auto operand = op.substr(pos + 1);
			operand.pop_back();
			auto value = util::readValue(operand, currentSection);
			ret = value;
			if (op.find("-") != std::string::npos)ret = -ret;
			break;
		}
	case IMMED:
	{
		auto operand = op.substr(1);
		return util::readValue(operand, currentSection);
	}
	default: 
		break;
	}
	if (entry) currentSection->insertRealocationEntry(entry);
	return ret;
}

void OperationalCodeTable::init()
{
	// Instrukcije za kontrolu toka
	std::string instruction = "INT";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x0, REGDIR,JMP_INST));
	instruction = "JMP";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x2, MEMDIR | REGIND | REGINDPOM, JMP_INST));
	instruction = "CALL";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x3, MEMDIR | REGIND | REGINDPOM, JMP_INST));
	instruction = "RET";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x1, 0, JMP_INST));
	instruction = "JZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x4, MEMDIR | REGIND | REGINDPOM, JMP_INST));
	instruction = "JNZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x5, MEMDIR | REGIND | REGINDPOM, JMP_INST));
	instruction = "JGZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x6, MEMDIR | REGIND | REGINDPOM, JMP_INST));
	instruction = "JGEZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x7, MEMDIR | REGIND | REGINDPOM, JMP_INST));
	instruction = "JLZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x8, MEMDIR | REGIND | REGINDPOM, JMP_INST));
	instruction = "JLEZ";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x9, MEMDIR | REGIND | REGINDPOM, JMP_INST));

	// LOAD/STORE Instrukcije
	instruction = "LOAD";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x10, MEMDIR | REGIND | REGINDPOM | IMMED | REGDIR, LOAD_STORE_INST));
	instruction = "LOADUB";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x10, MEMDIR | REGIND | REGINDPOM | IMMED | REGDIR, LOAD_STORE_INST, UB));
	instruction = "LOADSB";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x10, MEMDIR | REGIND | REGINDPOM | IMMED | REGDIR, LOAD_STORE_INST, SB));
	instruction = "LOADUW";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x10, MEMDIR | REGIND | REGINDPOM | IMMED | REGDIR, LOAD_STORE_INST, UW));
	instruction = "LOADSW";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x10, MEMDIR | REGIND | REGINDPOM | IMMED | REGDIR, LOAD_STORE_INST, SW));
	instruction = "STORE";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x11, MEMDIR | REGIND | REGINDPOM | REGDIR, LOAD_STORE_INST));
	instruction = "STOREB";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x11, MEMDIR | REGIND | REGINDPOM | REGDIR, LOAD_STORE_INST, SB));
	instruction = "STOREW";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x11, MEMDIR | REGIND | REGINDPOM | REGDIR, LOAD_STORE_INST, SW));


	// Instrukcije za rad sa stekom
	instruction = "PUSH";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x20, REGDIR, STACK_INST));
	instruction = "POP";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x21, REGDIR, STACK_INST));

	//Aritmeticke i logicke instrukcije
	instruction = "ADD";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x30, REGDIR, ALU_INST));
	instruction = "SUB";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x31, REGDIR, ALU_INST));
	instruction = "MUL";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x32, REGDIR, ALU_INST));
	instruction = "DIV";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x33, REGDIR, ALU_INST));
	instruction = "MOD";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x34, REGDIR, ALU_INST));
	instruction = "AND";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x35, REGDIR, ALU_INST));
	instruction = "OR";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x36, REGDIR, ALU_INST));
	instruction = "XOR";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x37, REGDIR, ALU_INST));
	instruction = "NOT";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x38, REGDIR, ALU_INST));
	instruction = "ASL";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x39, REGDIR, ALU_INST));
	instruction = "ASR";
	_map.insert_or_assign(instruction, std::make_shared<Mnemonic>(instruction, 0x3A, REGDIR, ALU_INST));
}


