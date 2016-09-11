#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include "String.h"
#include "Vector.h"

enum val_type_t
{
	STRING_VAL,
	NUM_VAL,
	FUN_VAL // That's fun as in "function"
};

// Forward declaration is necessary here, as this .h file is also included in ParseTree.h
struct parse_tree_node;

struct function {
	Vector<String> args;
	parse_tree_node * block;

	function () : args {}
	{
		block = nullptr;
	}

	function (const function &that) : args(that.args)
	{
		block = that.block;
	}

	~function ()
	{
	}
};

#endif
