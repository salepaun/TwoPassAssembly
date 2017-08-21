#pragma once
#include <memory>
#include <string>

enum class relocation_type { R_386_32, R_386_PC32 };

class RelocationEnty
{
public:

	RelocationEnty(uint32_t offset, relocation_type type, uint32_t index);

public: // operators

	friend std::ostream& operator<<(std::ostream& os, const RelocationEnty& entry);
	std::string to_string() const;

private:
	uint32_t _offset;
	relocation_type _type;
	uint32_t _index;
};

