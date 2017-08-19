#include "Symbol.h"
#include "Section.h"
#include <sstream>

Symbol::Symbol(std::string label, std::shared_ptr<Section> section, uint16_t offset, bool isSection, bool isDef)
	: _label(label)
	, _section(section)
	, _offset(offset)
	, _global(false)
	, _is_section(isSection)
	, _def_symbol(isDef)
	, _index(0)
{
}

void Symbol::setIndex(uint32_t index)
{
	_index = index;
}

void Symbol::setGlobal(bool global)
{
	_global = global;
}

void Symbol::setOffset(uint32_t offset)
{
	_offset = offset;
}

std::string Symbol::getName() const
{
	return _label;
}

std::shared_ptr<Section> Symbol::getSection() const
{
	return _section;
}

uint32_t Symbol::getOffset() const
{
	return _offset;
}

bool Symbol::isGlobal() const
{
	return _global;
}

bool Symbol::isSection() const
{
	return _is_section;
}

bool Symbol::isDefSymbol() const
{
	return _def_symbol;
}

uint32_t Symbol::getIndex() const
{
	return _index;
}

std::string Symbol::to_string() const
{
	
	std::stringstream stream;
	stream << "0x" << std::uppercase << std::hex << _offset;
	std::string value(stream.str());
	std::string index = _def_symbol ? "-1" : std::to_string(_section->getIndex());

	return "SYM " + std::to_string(_index) + " " + _label + " " + index + " " + value + " " + (_global ? "G" : "L");
}

std::ostream & operator<<(std::ostream & os, const Symbol &symbol)
{
	if (!symbol.isSection())
		os << symbol.to_string() << std::endl;
	return os;
}
