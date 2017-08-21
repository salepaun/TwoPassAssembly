#include "Definition.h"

std::unordered_map<std::string, uint8_t> Definition::_map = { { "DD",4 },{ "DW",2 },{ "DB",1 } };

bool Definition::contains(std::string def)
{
	return _map.find(def) != std::end(_map);
}

uint8_t Definition::getSize(std::string def)
{
	return _map.find(def)->second;
}
