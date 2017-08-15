#pragma once
#include <queue>
#include <stack>
#include <set>
#include <list>
#include "SymbolTable.h"


enum class math_type { number, variable, operation, left_bracket, right_bracket };

class math_expression
{
public:


public: // utill

	static std::pair<std::string, math_type> getNext(std::string& expression);
	static bool shouldPop(std::string op);
	static bool isLeftBracketAtTop();
	static void pushFromStack();
	static uint32_t calculatePostfix(SYMTAB &symtab);
public:
	static std::list<std::pair<std::string,math_type>> _queue;
	static std::stack<std::pair<std::string, math_type>> _operation_stack;

	static std::string _operations;
	static std::vector<std::set<std::string>> _op_priorities;
};

