#include <cstdio>
#include <cstdint>
#include "Interpreter.h"
#include "String.h"
#include "Parse.h"

// Parsing funtion prototypes
parse_tree_node * create_tree_node ();
parse_tree_node * create_tree_node_string (String);
parse_tree_node * create_tree_node_num (int32_t);

// Evaluation function prototypes
return_value eval_void (parse_tree_node *, symbol_table&, symbol_table&);
int32_t eval_op (parse_tree_node *, symbol_table&, symbol_table&);
template <typename T = int32_t> const T& eval_value (parse_tree_node *);
template <typename T = int32_t> T * eval_var (parse_tree_node *, symbol_table&, symbol_table&);

// Let's begin with the functions
return_value run (parse_tree_node * node, symbol_table& local, symbol_table& global)
{
	return_value result;

	while (node->type != FIN) {
		result = eval_void(node, local, global);
		
		if (!result.is_void)
			return result;

		node = node->jump_to;
	}

	// result.is_void must be true if the function has come this far
	return result;
}

// Constructs parse tree nodes and pushes them into the parse tree
void parse (parse_tree * out)
{
	while (out->last()->type != FIN) {
		parse_tree_node * new_node = create_tree_node();
		out->push_back (new_node);
	}
}

// Creates a tree node of type MAGIC_STR
parse_tree_node * create_tree_node_string (String s)
{
	parse_tree_node * new_node = new parse_tree_node;
	new_node->type = MAGIC_STR;
	new_node->value = new String (s);
	new_node->value_type = STRING_VAL;

	return new_node;
}

// Creates a tree node of type MAGIC_NUM
parse_tree_node * create_tree_node_num (int32_t n)
{
	parse_tree_node * new_node = new parse_tree_node;
	new_node->type = MAGIC_NUM;
	new_node->value = new int32_t (n);
	new_node->value_type = NUM_VAL;

	return new_node;
}

// Creates a tree node of any type other than MAGIC_STR
parse_tree_node * create_tree_node ()
{
	read_next_token ();

	parse_tree_node * new_node = new parse_tree_node;

	if (next_token_type == END) {
		new_node->type = FIN;
	}

	else if (next_token_type == NAME) {
		String name_type {next_token()};

		new_node->type = STATEMENT;
		new_node->value = new String (name_type);
		new_node->value_type = STRING_VAL;

		if (name_type == "text") {
			read_next_token();
			new_node->args.push_back(create_tree_node_string(next_token()));
		}

		else if (name_type == "output" || name_type == "return") {
			new_node->args.push_back(create_tree_node());
		}

		else if (name_type == "var" || name_type == "set") {
			read_next_token();
			new_node->args.push_back(create_tree_node_string(next_token()));
			new_node->args.push_back(create_tree_node());
		}

		else if (name_type == "if") {
			parse_tree_node * condition = create_tree_node();

			while (1) {
				new_node->args.push_back(condition);

				parse_tree block;
				parse(&block);
				new_node->args.push_back(block.begin());

				if (*(String*)(block.last()->value) == "fi") {
					block.strip();
					break;
				}

				// We're leaving open the possibility here for "elif" type statements
				if (*(String*)(block.last()->value) == "else")
					condition = create_tree_node_num (1);
				else
					condition = create_tree_node();

				block.strip();
			}
		}

		else if (name_type == "do") {
			parse_tree_node * condition = create_tree_node();
			new_node->args.push_back(condition);

			parse_tree block;
			parse(&block);

			new_node->args.push_back(block.begin());
			block.strip();
		}

		else if (name_type == "defun") {
			read_next_token();
			parse_tree_node * fun_name = create_tree_node_string(next_token());
			new_node->args.push_back(fun_name);

			read_next_token(); // Skip "params" text
			read_next_token();
			while (String{next_token()} != "smarap") {
				new_node->args.push_back(create_tree_node_string(next_token()));
				read_next_token();
			}

			parse_tree block;
			parse(&block);

			new_node->args.push_back(block.begin());
			block.strip();
		}

		else if (name_type == "call") {
			read_next_token();
			parse_tree_node * fun_name = create_tree_node_string(next_token());
			new_node->args.push_back(fun_name);

			read_next_token(); // Skip "args" text

			while (String{peek_next_token()} != "sgra")
				new_node->args.push_back(create_tree_node());

			read_next_token(); // Skip "sgra" text
		}

		else if (name_type == "fi" || name_type == "else" || name_type == "od" || name_type == "nufed")
			new_node->type = FIN;

		else // A variable is being referenced.
			new_node->type = VARIABLE;
	}

	else if (next_token_type == NUMBER) {
		delete new_node;
		new_node = create_tree_node_num (token_number_value);
	}

	else if (next_token_type == SYMBOL) {
		String sym {next_token()};

		new_node->type = OPERATOR;
		new_node->value = new String (sym);
		new_node->value_type = STRING_VAL;

		// If we have reached a comment, just ignore it and move on
		if (sym.size() >= 2 && sym.c_str()[0] == '/' && sym.c_str()[1] == '/') {
			skip_line();
			delete new_node;
			return create_tree_node();
		}

		new_node->args.push_back(create_tree_node());

		if (sym != "!" && sym != "~")
			new_node->args.push_back(create_tree_node());
	}

	return new_node;
}

// Evaluates instructions that have no stored return value
return_value eval_void (parse_tree_node * node, symbol_table& table, symbol_table& global_table)
{
	return_value result = {true};

	switch (node->type) {
	case STATEMENT:
	{
		String stmnt_type {eval_value<String> (node)};
		if (stmnt_type == "text") {
			printf ("%s", eval_value<String> (node->args[0]).c_str());
		}

		else if (stmnt_type == "output") {
			printf ("%d", eval_op (node->args[0], table, global_table));
		}

		else if (stmnt_type == "var") {
			String var_name = eval_value<String> (node->args[0]);
			int32_t var_value = eval_op (node->args[1], table, global_table);

			if (table.get(var_name)) {
				printf("variable %s incorrectly re-initialized\n", var_name.c_str());
				*table.get(var_name) = var_value;
				break;
			}

			table.insert(var_name, var_value);
		}

		else if (stmnt_type == "set") {
			String var_name = eval_value<String> (node->args[0]);
			int32_t var_value = eval_op (node->args[1], table, global_table);

			int32_t * local = table.get(var_name);
			int32_t * global = global_table.get(var_name);

			if (!local && !global) {
				printf("variable %s not declared\n", var_name.c_str());
				table.insert(var_name, var_value);
			}
			else if (local)
				*local = var_value;
			else
				*global = var_value;
		}

		else if (stmnt_type == "if") {
			for (int i = 0; i < node->args.size(); i += 2) {
				if (eval_op(node->args[i], table, global_table)) {
					result = run (node->args[i+1], table, global_table);

					break;
				}
			}
		}

		else if (stmnt_type == "do")
			while (eval_op(node->args[0], table, global_table) && result.is_void)
				result = run (node->args[1], table, global_table);

		// Functions are stored in the symbol table, just like any other variable
		else if (stmnt_type == "defun") {
			String fun_name = eval_value<String> (node->args[0]);
			function new_fun;

			int i = 1;
			for (; i < node->args.size() - 1; i++)
				new_fun.args.push_back(eval_value<String>(node->args[i]));

			new_fun.block = node->args[i];

			table.insert(fun_name, new_fun, FUN_VAL);
		}

		else if (stmnt_type == "return") {
			result.is_void = false;
			result.value = eval_op(node->args[0], table, global_table);
		}

		// Should never be reached
		else
			printf("Error: unknown statement in eval_void()\n");

		break;
	}

	case NOP: case FIN: case MAGIC_NUM: case MAGIC_STR: case OPERATOR:
		break;

	default:
		printf ("Error: unknown parse_tree_node_type in eval_void()\n");
		break;
	}

	return result;
}

// Returns the "value" parameter of a node
template <typename T>
const T& eval_value (parse_tree_node * node)
{
	return * ((T *) (node->value));
}

// Returns the value of a variable from the symbol table
template <typename T>
T * eval_var (parse_tree_node * node, symbol_table& local_table, symbol_table& global_table)
{
	T * local = local_table.get<T>(eval_value<String> (node));

	if (local)
		return local;

	return global_table.get<T>(eval_value<String> (node));
}

// Returns the value of a calculation resulting from the use of an operator
int32_t eval_op (parse_tree_node * node, symbol_table& table, symbol_table& global_table)
{
	switch (node->type) {
	case MAGIC_NUM:
		return eval_value (node);

	case VARIABLE:
		return * eval_var (node, table, global_table);

	case STATEMENT:
	{
		// A function has been called
		function fun {*eval_var<function>(node->args[0], table, global_table)};
		symbol_table local_table;

		for (int i = 0; i < fun.args.size(); i++)
			local_table.insert(fun.args[i], eval_op(node->args[i+1], table, global_table));

		return_value result = run (fun.block, local_table, global_table);

		if (result.is_void)
			result.value = 0;

		return result.value;
	}

	case OPERATOR:
	{
		String optype {eval_value<String> (node)};
		if (optype == "+")
			return eval_op (node->args[0], table, global_table) + eval_op (node->args[1], table, global_table);
		if (optype == "-")
			return eval_op (node->args[0], table, global_table) - eval_op (node->args[1], table, global_table);
		if (optype == "*")
			return eval_op (node->args[0], table, global_table) * eval_op (node->args[1], table, global_table);
		if (optype == "/")
			return eval_op (node->args[0], table, global_table) / eval_op (node->args[1], table, global_table);
		if (optype == "%")
			return eval_op (node->args[0], table, global_table) % eval_op (node->args[1], table, global_table);
		if (optype == "&&") {
			if (eval_op (node->args[0], table, global_table) && eval_op (node->args[1], table, global_table))
				return 1;
			return 0;
		}
		if (optype == "||") {
			if (eval_op (node->args[0], table, global_table) || eval_op (node->args[1], table, global_table))
				return 1;
			return 0;
		}
		if (optype == "<") {
			if (eval_op (node->args[0], table, global_table) < eval_op (node->args[1], table, global_table))
				return 1;
			return 0;
		}
		if (optype == ">") {
			if (eval_op (node->args[0], table, global_table) > eval_op (node->args[1], table, global_table))
				return 1;
			return 0;
		}
		if (optype == "==") {
			if (eval_op (node->args[0], table, global_table) == eval_op (node->args[1], table, global_table))
				return 1;
			return 0;
		}
		if (optype == "!=") {
			if (eval_op (node->args[0], table, global_table) != eval_op (node->args[1], table, global_table))
				return 1;
			return 0;
		}
		if (optype == "<=") {
			if (eval_op (node->args[0], table, global_table) <= eval_op (node->args[1], table, global_table))
				return 1;
			return 0;
		}
		if (optype == ">=") {
			if (eval_op (node->args[0], table, global_table) >= eval_op (node->args[1], table, global_table))
				return 1;
			return 0;
		}
		if (optype == "!") {
			if (eval_op (node->args[0], table, global_table))
				return 0;
			return 1;
		}
		if (optype == "~")
			return -eval_op (node->args[0], table, global_table);

		// Should never be reached
		printf("Error: unknown operator in eval_op(): %s\n", optype.c_str());
		return 0;
	}

	default:
		printf("Error: unknown type in eval_op(): %d\n", node->type);
	}

	return 0;
}
