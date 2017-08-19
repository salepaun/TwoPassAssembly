#pragma once
#include <string>
#include <memory>

class Section;

class Symbol
{
public:

	Symbol(std::string label, std::shared_ptr<Section> section, uint16_t offset, bool isSection = false, bool isDef = false);

public: // setters

	void setIndex(uint32_t index);
	void setGlobal(bool global);
	void setOffset(uint32_t offset);

public: // getters
	
	std::string getName() const;
	std::shared_ptr<Section> getSection() const;
	uint32_t getOffset() const;
	bool isGlobal() const;
	bool isSection() const;
	bool isDefSymbol() const;
	uint32_t getIndex() const;

	std::string to_string() const;

public: // operators

	friend std::ostream& operator<<(std::ostream& os, const Symbol& symbol);

private: // private member variables

	std::string _label;
	std::shared_ptr<Section> _section;
	uint32_t _offset;
	bool _global;
	bool _is_section;
	bool _def_symbol;
	uint32_t _index;
};

