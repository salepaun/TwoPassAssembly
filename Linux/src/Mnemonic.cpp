#include "Mnemonic.h"
#include "macros.h"
#include "OperationalCodeTable.h"
#include <regex>
#include "util.h"

std::unordered_map<uint8_t, uint8_t> Mnemonic::_map = { {REGDIR,4},{REGIND,4},{REGINDPOM,8} ,{IMMED,8},{MEMDIR,8} };


Mnemonic::Mnemonic(std::string & name, uint8_t code, uint8_t modes, uint8_t type, uint8_t inst_type)
	: _name(name)
	, _code(code)
	, _modes(modes)
	, _type(type)
	, _inst_type(inst_type)
{
}

uint32_t Mnemonic::constructInstruction(std::vector<std::string> inst) const
{
	auto a = 0;
	uint32_t ret = _code << 24;
	if (_code == 0x01) return ret; // RET
	auto addr_mode = OPTAB::getAddressMode(inst);
	switch (addr_mode)
	{
	case REGDIR:
		ret |= ADDR_REGDIR << 21;
		break;
	case REGIND:
		ret |= ADDR_REGIND << 21;
		break;
	case REGINDPOM:
		ret |= ADDR_REGINDPOM << 21;
		break;
	case IMMED:
		ret |= ADDR_IMMED << 21;
		break;
	case MEMDIR:
		ret |= ADDR_MEMDIR << 21;
		break;
	}

	switch (_type)
	{
	case ALU_INST:
		ret |= Mnemonic::getRegisterCode(inst[1]) << 16;
		ret |= Mnemonic::getRegisterCode(inst[2]) << 11;
		if(inst.size()==4)
		ret |= Mnemonic::getRegisterCode(inst[3]) << 6;
		break;
	case JMP_INST:
		switch (inst.size())
		{
		case 2:
			if(std::regex_match(inst[1],util::register_regex))
			{
				ret |= Mnemonic::getRegisterCode(inst[1]) << 16;
			}
			else
				if (std::regex_match(inst[1], util::regind_regex))
				{
					ret |= Mnemonic::getRegisterIndCode(inst[1]) << 16;
				}
				else
					if (std::regex_match(inst[1], util::regindpom_regex))
					{
						ret |= Mnemonic::getRegisterIndCode(inst[1]) << 16;

					}	
			break;
		case 3:
			ret |= Mnemonic::getRegisterCode(inst[1]) << 11;
			if (std::regex_match(inst[2], util::register_regex))
			{
				ret |= Mnemonic::getRegisterCode(inst[2]) << 16;
			}
			else
				if (std::regex_match(inst[2], util::regind_regex))
				{
					ret |= Mnemonic::getRegisterIndCode(inst[2]) << 16;
				}
				else
					if (std::regex_match(inst[2], util::regindpom_regex))
					{
						ret |= Mnemonic::getRegisterIndCode(inst[2]) << 16;
					}
					else 
						if (std::regex_match(inst[2], util::memdir_regex) && (inst[2].find("$") != std::string::npos))
						{
							ret &= ~(7<<21);
							ret |= ADDR_REGINDPOM<<21;
							ret &= ~(31 << 11);
							ret |= 0x11 << 16;
							ret |= Mnemonic::getRegisterCode(inst[1]) << 11;
						}
			break;
		}
		break;
	case STACK_INST:
		ret |= Mnemonic::getRegisterCode(inst[1]) << 16;
		break;
	case LOAD_STORE_INST:
		ret |= Mnemonic::getRegisterCode(inst[1]) << 11;
		ret |= this->getInstType() << 3;
		if (std::regex_match(inst[2], util::register_regex))
		{
			ret |= Mnemonic::getRegisterCode(inst[2]) << 16;
		}
		else
			if (std::regex_match(inst[2], util::regind_regex))
			{
				ret |= Mnemonic::getRegisterIndCode(inst[2]) << 16;
			}
			else
				if (std::regex_match(inst[2], util::regindpom_regex))
				{
					ret |= Mnemonic::getRegisterIndCode(inst[2]) << 16;
				}
				else
					if (std::regex_match(inst[2], util::memdir_regex))
					{
						if (inst[2].find("$") != std::string::npos)
						{
							ret &= ~(7 << 21);
							ret |= ADDR_REGINDPOM << 21;
							ret &= ~(31 << 16);
							ret |= 0x11 << 16;
						}//ret |= Mnemonic::getRegisterCode(inst[1]) << 11;
					}
		break;
	}
	return ret;
}

bool Mnemonic::isValid(uint8_t mode) const
{
	return (_modes & mode) > 0;
}

uint8_t Mnemonic::getCode() const
{
	return _code;
}

std::string Mnemonic::getName() const
{
	return _name;
}

uint8_t Mnemonic::getModes() const
{
	return _modes;
}

uint8_t Mnemonic::getType() const
{
	return _type;
}

uint8_t Mnemonic::getInstType() const
{
	return _inst_type;
}

uint8_t Mnemonic::getInstructionSize(uint8_t mode)
{
	switch (mode)
	{
	case REGDIR:
	case REGIND:
		return 4;
	case IMMED:
	case MEMDIR:
	case REGINDPOM:
		return 8;
	}
	return 0;
}

uint8_t Mnemonic::getRegisterCode(std::string reg)
{
	if(reg[0]=='R')
		return uint8_t(strtoul(reg.substr(1).c_str(), nullptr, 10));
	
	if (reg[0] == 'S') return 0x10;
	
	return 0x11;
}

uint8_t Mnemonic::getRegisterIndCode(std::string reg)
{
	
	auto pos = reg.find_first_of(" +-/*]");
	if (pos != std::string::npos)
		reg = reg.substr(1, pos);

	if (reg[0] == 'R')
		return uint8_t(strtoul(reg.substr(1).c_str(), nullptr, 10));

	if (reg[0] == 'S') return 0x10;

	return 0x11;
}
