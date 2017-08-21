#include "RelocationEnty.h"
#include <sstream>

RelocationEnty::RelocationEnty(uint32_t offset, relocation_type type, uint32_t index)
	: _offset(offset)
	, _type(type)
	, _index(index)
{
}

std::ostream & operator<<(std::ostream & os, const RelocationEnty & entry)
{
	os << entry.to_string();
	return os;
}

std::string RelocationEnty::to_string() const
{
	std::stringstream stream;
	std::string type = (_type == relocation_type::R_386_32) ? "A" : "R";
	stream << "0x" << std::uppercase << std::hex << _offset << " " << type << " " << std::dec << _index;
	auto s = stream.str();
	return s;
}