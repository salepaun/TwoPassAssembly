#include "Section.h"



Section::Section(std::string name)
	: _name(name)
	, _locctr(0)
{
}

void Section::incLocationCounter(uint16_t amount)
{
	_locctr += amount;
}

void Section::resetLocationCounter()
{
	_locctr = 0;
}

std::string Section::getName() const
{
	return _name;
}

uint16_t Section::getLocationCounter() const
{
	return _locctr;
}
