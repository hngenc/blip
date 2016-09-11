#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ParseTree.h"
#include "SymbolTable.h"

struct return_value
{
	bool is_void;
	int32_t value;
};

void parse (parse_tree * out);
return_value run (parse_tree_node *, symbol_table&, symbol_table&);

#endif
