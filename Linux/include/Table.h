#pragma once
#include <memory>
#include <string>
#include <vector>
#include <algorithm>

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

	iterator begin() { return &_table[0]; }
	const_iterator begin() const { return &_table[0]; }
	iterator end() { return iterator((char*)&_table[size() - 1] + sizeof(_table[size() - 1])); }
	const_iterator end() const { return const_iterator((char*)&_table[size() - 1] + sizeof(_table[size() - 1])); }

	std::size_t size() const { return _table.size(); }

	friend std::ostream &operator<<(std::ostream & os, const Table<T>& table)
	{
		return os<<table.to_string();
	}

	std::string to_string() const
	{
		std::string ret = "";
		for (auto &entry : _table)
			ret+= (*entry).to_string() + "\n";

		return ret;
	}

	//static Table<T> table;

private:
	std::vector<std::shared_ptr<T>> _table;
};

