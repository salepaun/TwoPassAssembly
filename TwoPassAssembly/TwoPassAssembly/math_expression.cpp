#include "math_expression.h"
#include "Symbol.h"
#include <ctype.h>

std::string math_expression::_operations = "+-*";
std::vector<std::set<std::string>> math_expression::_op_priorities{ {"("},{"+","-"}, {"*","/"} };
std::list<std::pair<std::string, math_type>> math_expression::_queue;
std::stack<std::pair<std::string, math_type>> math_expression::_operation_stack;


std::pair<std::string, math_type> math_expression::getNext(std::string& expression)
{
	auto first_char = expression[0];
	auto result = std::string();
	auto i = 0;
	if (isdigit(first_char))
	{
		while (isdigit(expression[i]))
			result.push_back(expression[i++]);
		expression.erase(0, i);
		return std::make_pair(result, math_type::number);
	}

	if (math_expression::_operations.find(first_char) != std::string::npos)
	{
		expression.erase(0, 1);
		auto ret = std::string(1,first_char);
		return std::make_pair(ret, math_type::operation);
	}

	if (isalpha(first_char))
	{
		while (isalpha(expression[i]) || isdigit(expression[i]))
			result.push_back(expression[i++]);
		expression.erase(0, i);
		return std::make_pair(result, math_type::variable);
	}

	expression.erase(0, 1);
	math_type type;
	if (first_char == '(')
		type = math_type::left_bracket;
	else type = math_type::right_bracket;
	
	auto ret = std::string(1, first_char);
	return std::make_pair(ret, type);
}

bool math_expression::shouldPop(std::string op)
{
	if (_operation_stack.empty()) return false;
	auto top = _operation_stack.top().first;
	auto top_priority = 0;
	for (auto i = 0; i < _op_priorities.size(); ++i)
	{
		if (_op_priorities[i].find(top) != _op_priorities[i].end())
			top_priority = i;
	}

	auto op_priority = 0;
	for (auto i = 0; i < _op_priorities.size(); ++i)
	{
		if (_op_priorities[i].find(op) != _op_priorities[i].end())
			op_priority = i;
	}

	return op_priority <= top_priority;

}

bool math_expression::isLeftBracketAtTop()
{
	auto top = math_expression::_operation_stack.top();
	return top.first == "(";
}

void math_expression::pushFromStack()
{
	auto pair = math_expression::_operation_stack.top();
	math_expression::_operation_stack.pop();
	math_expression::_queue.push_back(pair);
}

uint32_t math_expression::calculatePostfix(SYMTAB &symtab)
{
	auto stack = std::stack<uint32_t>();

	while(!math_expression::_queue.empty())
	{
		auto elem = _queue.front();
		_queue.pop_front();
		auto value = 0;
		switch (elem.second)
		{
		case math_type::number:
			value = strtoul(elem.first.c_str(), nullptr, 10);
			stack.push(value);
			break;
		case math_type::variable:
			value = symtab.find(elem.first)->getOffset();
			stack.push(value);
			break;
		case math_type::operation:
			auto val2 = stack.top();
			stack.pop();
			auto val1 = stack.top();
			stack.pop();
			if (elem.first == "+")
				stack.push(val1 + val2);
			else
			if (elem.first == "-")
				stack.push(val1 - val2);
			else
			if (elem.first == "*")
				stack.push(val1 * val2);
			else
			if (elem.first == "/")
				stack.push(val1 / val2);
			break;
		}
	}
	return stack.top();
}
