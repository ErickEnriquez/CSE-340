#pragma once
#include <string>
#include "lexer.h"

//a struct for symbol table which will hold the symbol as a string and the location in the mem array 
struct symbol_table {
	Token symbol;// the symbol 
	int location;//location of the symbol in the memory table
	bool constant;// may need this to tell if the symbol is supposed to represent a constant

};

struct stmt_node// node for every statment
{
	int statement_type;
	int LHS;
	int operator_type;
	int op1;
	int op2;
	struct stmt_node* next;
	struct stmt_node* code;// will be useful in procedure declarations
};

struct linkedList {//start of the statements
	struct stmt_node* start;
};
//nodes that will be used to store the inputs of the program
struct input_node {
	Token t;
	input_node* nextToken;
};
//hold the nnodes in a stack
struct stack {
	input_node* top;
	stack() {
		top = nullptr;
	}
	void push(Token t) {
		input_node* temp = new input_node();//create a new node
		temp->t = t;//store the token in the node
		temp->nextToken = top;//link temp with previous top
		top = temp;//make temp the new top
	}
	Token pop() {
		input_node* temp = top;// make a temporary variable to hold the old top
		top = top->nextToken;//make top the next 
		return temp->t;//return the token
	}
	bool isEmpty() {
		if (top == nullptr)
			return true;
		else
			return false;
	}
};