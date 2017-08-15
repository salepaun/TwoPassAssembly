#pragma once
#include <memory>

class Section;

enum class relocation_type { R_386_32, R_386_PC32 };

class RelocationEnty
{
public:

	RelocationEnty(std::shared_ptr<Section> section, uint16_t offset, relocation_type type, uint16_t index);


private:
	std::shared_ptr<Section> _section;
	uint16_t _offset;
	relocation_type _type;
	uint16_t _index;
};

