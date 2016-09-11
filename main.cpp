#include "Interpreter.h"
#include "Parse.h"

int main ()
{
	parse_tree tree;
	symbol_table table;

	parse (&tree);
	run (tree.begin(), table, table);

	return 0;
}