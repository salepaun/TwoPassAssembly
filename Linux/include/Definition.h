#pragma once
#include <unordered_map>

class Definition
{
public:
	static bool contains(std::string def);

	static uint8_t getSize(std::string def);
private:
	static std::unordered_map<std::string, uint8_t> _map;
};

