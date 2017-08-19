#pragma once
#include <string>
#include <unordered_map>

class Mnemonic
{
public:
	Mnemonic(std::string& name, uint8_t code, uint8_t modes, uint8_t type, uint8_t inst_type = 0);
	uint32_t constructInstruction(std::vector<std::string> inst) const;

public: // setters

	bool isValid(uint8_t mode) const;

public: // getters

	uint8_t getCode() const;
	std::string getName() const;
	uint8_t getModes() const;
	uint8_t getType() const;
	uint8_t getInstType() const;

public: // static

	static uint8_t getInstructionSize(uint8_t mode);
	static uint8_t getRegisterCode(std::string reg);
	static uint8_t getRegisterIndCode(std::string reg);


private: // private member variables

	std::string _name;
	uint8_t _code;
	uint8_t _modes;
	uint8_t _type;
	uint8_t _inst_type;

private: // static variables

	static std::unordered_map<uint8_t, uint8_t> _map;
};

