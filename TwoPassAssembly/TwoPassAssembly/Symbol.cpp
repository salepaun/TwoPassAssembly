#include "Symbol.h"


Symbol::Symbol(std::string label, std::shared_ptr<Section> section, uint16_t offset) 
	: _label(label)
	, _section(section)
	, _offset(offset)
	, _global(false)
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

std::string Symbol::getLabel() const
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

uint16_t Symbol::getIndex() const
{
	return _index;
}
