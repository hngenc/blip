#include "ParseTree.h"
#include "String.h"

// Node functions

parse_tree_node::parse_tree_node ()
{
	type = NOP;
	value = nullptr;
	jump_to = nullptr;
}

parse_tree_node::~parse_tree_node ()
{
	if (value) {
		if (value_type == STRING_VAL)
			delete (String *) value;
		else if (value_type == NUM_VAL)
			delete (int32_t *) value;
		else // if (value_type == NODE_VAL)
			delete (parse_tree_node *) value;
		value = nullptr;
	}

	for (int i = 0; i < args.size(); i++)
		if (args[i]) {
			delete args[i];
			args[i] = nullptr;
		}

	if (jump_to) {
		delete jump_to;
		jump_to = nullptr;
	}
}

// Tree functions

parse_tree::parse_tree ()
{
	head = new parse_tree_node;
	head->type = NOP;
	end = head;
}

parse_tree::~parse_tree ()
{
	if (head) {
		delete head;
		head = nullptr;
	}
}

void parse_tree::push_back (parse_tree_node * new_node)
{
	end->jump_to = new_node;
	end = new_node;
}

void parse_tree::strip ()
{
	head = end = nullptr;
}

parse_tree_node * parse_tree::begin ()
{
	return this->head;
}

parse_tree_node * parse_tree::last ()
{
	return this->end;
}
