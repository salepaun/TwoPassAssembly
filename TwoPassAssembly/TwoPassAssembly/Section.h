#pragma once
#include <string>
#include <vector>
#include <iostream>

class Section
{
public:
	Section(std::string name, uint16_t index, uint16_t locctr = 0);

	void incLocationCounter(uint16_t amount);
	void resetLocationCounter();

	void appendData(std::vector<char> data);
	void saveAndResetLocctr();

public: // getters

	std::string getName() const;
	uint16_t getLocationCounter() const;
	std::vector<uint8_t> getData() const;
	uint16_t getIndex() const;
	std::string to_string() const;

public: // operators

	friend std::ostream& operator<<(std::ostream& os, const Section& section);

private:
	std::string _name;
	uint16_t _begin_locctr;
	uint16_t _locctr;
	uint16_t _end_locctr;
	std::vector<uint8_t> _data;
	uint16_t _index;
};

