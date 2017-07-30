#pragma once
#include <vector>
#include <regex>

class util {
	public:

	static std::vector<std::string> tokenizeString(const std::string& str, const std::string& delimiters);
	
	static std::hash<std::string> hash;

public: // regex
	static std::regex register_regex;
	static std::regex regindpom_regex;
	static std::regex regind_regex;
	static std::regex immed_regex;
	static std::regex memdir_regex;

};


