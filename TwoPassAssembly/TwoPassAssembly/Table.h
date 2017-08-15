#pragma once
#include <memory>
#include <string>
#include <vector>

template <class T>
class Table
{
public: // typedefs

	typedef std::shared_ptr<T>* iterator;
	typedef const std::shared_ptr<T>* const_iterator;

public: // basic operations

	void insert(std::shared_ptr<T> symbol)
	{
		_table.push_back(symbol);
	}

	std::shared_ptr<T> find(std::string name)
	{
		return *std::find_if(
			std::begin(_table),
			std::end(_table),
			[&](auto entry)
		{
			return name == entry->getName();
		}
		);
	}

	std::shared_ptr<T>& operator[] (uint32_t index) { return _table[index]; }


public: // foreach

	iterator begin(){ return std::begin(_table)._Ptr; }
	const_iterator begin() const { return std::begin(_table)._Ptr; }
	iterator end(){ return std::end(_table)._Ptr; }
	const_iterator end() const { return std::end(_table)._Ptr; }

	std::size_t size() const { return _table.size(); }

	friend std::ostream &operator<<(std::ostream & os, const Table<T>& table)
	{
		for (auto &entry : table)
			os << *entry;

		return os;
	}


private:
	std::vector<std::shared_ptr<T>> _table;
};

