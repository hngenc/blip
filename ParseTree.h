#ifndef PARSE_TREE_H
#define PARSE_TREE_H

#include "Vector.h"
#include "ValueTypes.h"

enum parse_tree_node_type
{
	MAGIC_NUM,
	MAGIC_STR,
	VARIABLE,
	OPERATOR,
	STATEMENT,
	FIN,
	NOP,
};

struct parse_tree_node
{
	parse_tree_node_type type;

	void * value;
	val_type_t value_type;

	Vector<parse_tree_node*> args;

	parse_tree_node * jump_to;

	parse_tree_node ();
	~parse_tree_node ();
};

struct parse_tree
{
private:
	parse_tree_node * head;
	parse_tree_node * end;

public:
	parse_tree ();
	~parse_tree ();

	void push_back (parse_tree_node *);
	void strip (); // Prevents the destructor from deleting nodes. Parse_tree is left in an unusable state.

	parse_tree_node * begin ();
	parse_tree_node * last ();
};

#endif
