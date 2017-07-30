#include "Mnemonic.h"
#include "macros.h"

std::unordered_map<uint8_t, uint8_t> Mnemonic::_map = { {REGDIR,4},{REGIND,4},{REGINDPOM,8} ,{IMMED,8},{MEMDIR,8} };


Mnemonic::Mnemonic(std::string & name, uint8_t code, uint8_t modes)
	: _name(name)
	, _code(code)
	, _modes(modes)
{
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
