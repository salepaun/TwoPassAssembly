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

void Symbol::setIndex(uint16_t index)
{
	_index = index;
}

void Symbol::setGlobal(bool global)
{
	_global = global;
}

std::string Symbol::getName() const
{
	return _label;
}

std::shared_ptr<Section> Symbol::getSection() const
{
	return _section;
}

uint16_t Symbol::getOffset() const
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

uint16_t Symbol::getIndex() const
{
	return _index;
}

std::string Symbol::to_string() const
{

	std::stringstream stream;
	stream << "0x" << std::hex << _offset;
	std::string value(stream.str());

	return "SYM " + std::to_string(_index) + " " + _label + " " + std::to_string(_section->getIndex()) + " " + value + " " + (_global ? "G" : "L");
}

std::ostream & operator<<(std::ostream & os, const Symbol &symbol)
{
	if (!symbol.isSection())
		os << symbol.to_string() << std::endl;
	return os;
}
