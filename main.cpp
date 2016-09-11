#include "Interpreter.h"
#include "Parse.h"

int main ()
{
	// char s[] = "test*.blip";

	// for (int i = 9; i <= 9; i++) {
	// 	s[4] = i + '0';

	// 	set_input(s);

		parse_tree tree;
		symbol_table table;

		parse (&tree);
		run (tree.begin(), table, table);
	// }

	return 0;
}