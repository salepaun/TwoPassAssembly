#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Table.h"

class RelocationEnty;

class Section
{
public:
	Section(std::string name, uint32_t index, uint32_t locctr = 0, bool org = false);

	void incLocationCounter(uint32_t amount);
	void resetLocationCounter();

	void appendData(uint32_t value, uint8_t size, bool firstWord = true);
	void saveAndResetLocctr();

	void insertRealocationEntry(std::shared_ptr<RelocationEnty> &entry);

public: // getters

	std::string getName() const;
	uint32_t getLocationCounter() const;
	uint32_t getBeginLocationCounter() const;
	std::vector<uint8_t> getData() const;
	uint32_t getIndex() const;
	Table<RelocationEnty>& getTable();

	std::string to_string() const;
	std::string to_string_data() const;
	std::string to_string_table() const;

public: // operators

	friend std::ostream& operator<<(std::ostream& os, const Section& section);

private:
	std::string _name;
	uint32_t _begin_locctr;
	uint32_t _locctr;
	uint32_t _end_locctr;
	std::vector<uint8_t> _data;
	uint32_t _index;
	std::string _flags;
	Table<RelocationEnty> _table;
};

