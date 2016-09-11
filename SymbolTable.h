#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <cstdint>
#include "ValueTypes.h"
#include "String.h"

struct symbol_table
{
private:
	struct node
	{
		String name;
		void * value;
		val_type_t value_type;

		node * parent;
		node * left;
		node * right;

		node () : node ("", 0, NUM_VAL) {}

		template<typename T>
		node (const String& _name, T _value, val_type_t _value_type)
		{
			name = _name;
			value = new T (_value);
			value_type = _value_type;
			left = right = nullptr;
		}

		~node ()
		{
			if (value) {
				if (value_type == NUM_VAL)
					delete (int32_t *) value;
				else // if value_type == FUN_VAL
					delete (function *) value;
				value = nullptr;
			}

			if (left)
				delete left;
			if (right)
				delete right;

			left = right = nullptr;
		}
	};

	node * head;

public:
	symbol_table ()
	{
		head = nullptr;
	}

	~symbol_table ()
	{
		if (head)
			delete head;
		head = nullptr;
	}

	template<typename T> void insert (const String& _name, const T& _value, val_type_t _value_type = NUM_VAL);
	template<typename T = int32_t> T * get (const String& _name);
};

template<typename T>
void symbol_table::insert (const String& _name, const T& _value, val_type_t _value_type)
{
	if (head == nullptr) {
		head = new node (_name, _value, _value_type);
		return;
	}

	node * parent;

	for (node * it = head; it != nullptr;) {
		parent = it;

		if (_name < it->name)
			it = it->left;
		else
			it = it->right;
	}

	if (_name < parent->name)
		parent->left = new node (_name, _value, _value_type);
	else
		parent->right = new node (_name, _value, _value_type);
}

template<typename T>
T * symbol_table::get (const String& _name)
{
	node * result = head;

	while (result) {
		if (_name == result->name)
			return (T *) result->value;

		if (_name < result->name)
			result = result->left;
		else
			result = result->right;
	}

	return nullptr;
}

#endif
