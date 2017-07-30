#pragma once
#include <string>

class Section
{
public:
	Section(std::string name);

	void incLocationCounter(uint16_t amount);
	void resetLocationCounter();

public: // getters

	std::string getName() const;
	uint16_t getLocationCounter() const;

private:
	std::string _name;
	uint16_t _locctr;
};

