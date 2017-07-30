#pragma once
#include <string>
#include <unordered_map>

class Mnemonic
{
public:
	Mnemonic(std::string& name, uint8_t code, uint8_t modes);

public: // setters

	bool isValid(uint8_t mode) const;

public: // getters

	uint8_t getCode() const;
	std::string getName() const;
	uint8_t getModes() const;

public: // static

	static uint8_t getInstructionSize(uint8_t mode);

private: // private member variables

	std::string _name;
	uint8_t _code;
	uint8_t _modes;

private: // static variables

	static std::unordered_map<uint8_t, uint8_t> _map;
};

