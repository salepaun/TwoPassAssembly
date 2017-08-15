#include "RelocationEnty.h"

RelocationEnty::RelocationEnty(std::shared_ptr<Section> section, uint16_t offset, relocation_type type, uint16_t index)
	: _section(section)
	, _offset(offset)
	, _type(type)
	, _index(index)
{
}
