#pragma once
#include <string>
#include <memory>

class Section;

class Symbol
{
public:

	Symbol(std::string label, std::shared_ptr<Section> section, uint16_t offset);

public: // setters

	void setIndex(uint16_t index);
	void setGlobal(bool global);

public: // getters
	
	std::string getLabel() const;
	std::shared_ptr<Section> getSection() const;
	uint16_t getOffset() const;
	bool isGlobal() const;
	uint16_t getIndex() const;

private: // private member variables

	std::string _label;
	std::shared_ptr<Section> _section;
	uint16_t _offset;
	bool _global;
	uint16_t _index;
};

