#include "Section.h"
#include <vector>
#include <sstream>
#include <iomanip>
#include "RelocationEnty.h"


Section::Section(std::string name, uint32_t index, uint32_t locctr, bool org)
	: _name(name)
	, _index(index)
	, _begin_locctr(locctr)
	, _end_locctr(locctr)
	, _locctr(locctr)
{
	if (name.find("text") != std::string::npos)
		_flags = "XPA";
	if (name.find("data") != std::string::npos)
		_flags = "WPA";
	if (name.find("rodata") != std::string::npos)
		_flags = "A";
	if (name.find("bss") != std::string::npos)
		_flags = "WA";
	if (org) _flags += "O";
}

void Section::incLocationCounter(uint32_t amount)
{
	_locctr += amount;
}

void Section::resetLocationCounter()
{
	_locctr = 0;
}

void Section::appendData(uint32_t value, uint8_t size, bool firstWord)
{
	if(firstWord)
	{
		auto shiftBy = 24;
		while(shiftBy>=0)
		{	
			auto byte = value >> shiftBy;
			_data.push_back(uint8_t(byte));
			shiftBy -= 8;
		}
	}
	else
	for (auto i = 0; i < size; ++i)
	{
		_data.push_back(uint8_t(value));
		value >>= 8;
	}

	_locctr += size;
}

void Section::saveAndResetLocctr()
{
	_end_locctr = _locctr;
	_locctr = _begin_locctr;
}

void Section::insertRealocationEntry(std::shared_ptr<RelocationEnty>& entry)
{
	_table.insert(entry);
}

std::string Section::getName() const
{
	return _name;
}

uint32_t Section::getLocationCounter() const
{
	return _locctr;
}

uint32_t Section::getBeginLocationCounter() const
{
	return _begin_locctr;
}

std::vector<uint8_t> Section::getData() const
{
	return _data;
}

uint32_t Section::getIndex() const
{
	return _index;
}

Table<RelocationEnty>& Section::getTable() 
{
	return _table;
}

std::string Section::to_string() const
{
	std::stringstream stream;
	stream << "0x" << std::uppercase << std::hex << _begin_locctr;
	auto begin = stream.str();

	stream.str(std::string());
	
	stream << "0x" << std::uppercase << std::hex << _end_locctr - _begin_locctr;
	auto end = stream.str();

	return  "SEG " + std::to_string(_index) + " " + _name + " " + std::to_string(_index) + " " + begin + " " + end + " " + _flags;
}

std::string Section::to_string_data() const
{
	//std::string ret = _name + "\n";
	
	//stream << _name << "\n";
	std::stringstream stream;
	stream<< _name << "\n";
	for (uint8_t i = 0; i < _data.size(); ++i)
	{
		
		stream << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << uint32_t(_data[i]);
		auto c = ((i + 1) % 16 == 0) ? "\n" : " ";
		if (i == (_data.size() - 1)) c = "";
		stream << c;
	}
	
	return  stream.str();
}

std::string Section::to_string_table() const
{
	std::stringstream stream;
	stream << "#rel" << _name << "\n";
	for (auto &entry : _table)
		stream << entry->to_string() << "\n";

	return stream.str();
}

std::ostream & operator<<(std::ostream & os, const Section & section)
{
	return os << section.to_string() << std::endl;
}
