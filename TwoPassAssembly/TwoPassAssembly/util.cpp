#include <vector>
#include "util.h"

std::vector<std::string> util::tokenizeString(const std::string& str, const std::string& delimiters)
{
	std::vector<std::string> tokens;
	if (str.empty())return tokens;
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos)
	{  // Found a token, add it to the vector.
		std::string sub = str.substr(lastPos, pos - lastPos);
		if (sub.find(";") != std::string::npos) return tokens;
		if (sub.find("[") != std::string::npos)
		{
			auto first = str.find_first_of("[");
			auto last = str.find_last_of("]");
			tokens.push_back(str.substr(first, last - first + 1));
			return tokens;
		}
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
	return tokens;
}

std::regex util::register_regex ("^R([0-9]||1[0-5])$");
std::regex util::regindpom_regex("^\\[R([0-9]||1[0-5]) (\\+||\\-) [a-zA-z0-9]+\\]$");
std::regex util::regind_regex	("^\\[R([0-9]||1[0-5])\\]$");
std::regex util::immed_regex	("#([0-9A-F]+||[a-zA-z][a-zA-Z0-9]*)");
std::regex util::memdir_regex	("^[a-zA-Z][a-zA-Z0-9]+$");