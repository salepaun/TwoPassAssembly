#include "Section.h"
#include <vector>
#include <sstream>


Section::Section(std::string name, uint16_t index, uint16_t locctr)
	: _name(name)
	, _index(index)
	, _begin_locctr(locctr)
	, _end_locctr(locctr)
	, _locctr(locctr)
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

void Section::appendData(std::vector<char> data)
{
	for (auto &byte : data)
		_data.push_back(byte);
	incLocationCounter((uint16_t) data.size());
}

void Section::saveAndResetLocctr()
{
	_end_locctr = _locctr;
	_locctr = 0;
	_data = std::vector<uint8_t>(_end_locctr);
}

std::string Section::getName() const
{
	return _name;
}

uint16_t Section::getLocationCounter() const
{
	return _locctr;
}

std::vector<uint8_t> Section::getData() const
{
	return _data;
}

uint16_t Section::getIndex() const
{
	return _index;
}

std::string Section::to_string() const
{
	std::stringstream stream;
	stream << "0x" << std::hex << _begin_locctr;
	auto begin = stream.str();

	stream.str(std::string());
	
	stream << "0x" << std::hex << _end_locctr;
	auto end = stream.str();

	return  "SEG " + std::to_string(_index) + " " + _name + " " + std::to_string(_index) + " " + begin + " " + end;
}

std::ostream & operator<<(std::ostream & os, const Section & section)
{
	return os << section.to_string() << std::endl;
}
