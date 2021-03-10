/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>
#include "parser.h"

using namespace std;
///////////////////////////////////////////////////////////////////////////////////////////
int *mem = new int[1000];					 				 // dynamic array of 1000 elements to hold data
symbol_table *table = new symbol_table[1000]; 				// create a new symbol table of size 1000
int next_available;
int numOfProc;												//counter for the number of procedures in a program
struct stack *stack = new struct stack();					//stack to store inputs
struct stack *s2 = new struct stack();						//since inputs will be reverse need another stack to put them in order
struct linkedList *list = new struct linkedList();			//create a list to hold the statments
struct proc_table *procTable = new struct proc_table[1000]; //create a procedure table

///////////////////////////////////////////////////////////////////////////////////////////

//########################################################################################################################
//this function wil take in a string which is the token t.lexme and will look to see if it already exists in the symbol table
//and return its location in the array

int location_in_s_table(Token t, symbol_table *table)
{
	int count = 0;
	while (count < 1000)
	{
		if (table[count].symbol.lexeme == t.lexeme)
		{
			return count;
		}
		count++;
	}
	return -1; //if not found in the symbol table we return a -1
}
//########################################################################################################################
//looks for a procedure in a table and returns the location to it
int location_in_p_table(Token t, proc_table *table)
{
	for (int i = 0; i < 1000; i++)
	{
		if (t.lexeme == table[i].procName)
			return i;
	}
	return -1; //return -1 if not found in the procedure table
}
//########################################################################################################################
/*the function checks to see if the symbol is in the table if not it allocated it  */
void allocate(Token t, symbol_table *table, int &next_avail, int *mem)
{
	if (location_in_s_table(t, table) != -1) //if we dont get a -1 from the symbol table then we need to add the symbol to both tables
		return;
	else
	{ //if we haven't seen the symbol before
		if (t.token_type == NUM)
		{
			table[next_avail].constant = true; //if the token is a constant make not of it
			mem[next_avail] = stoi(t.lexeme);  //store the quantity in the memory table because its a constant
		}
		else
		{
			table[next_avail].constant = false; //make note that this is not a constant
		}
		table[next_avail].symbol = t;			 //store the symbol at the next available location in memory
		table[next_avail].location = next_avail; //store the location of the symbol for the mem table
		next_avail++;
	}
}
//########################################################################################################################
//this function will execute the program from a given linked list
void execute_program(linkedList *list)
{
	stmt_node *pc = list->start; //get the start for the linked list
	while (pc != nullptr)
	{
		if (pc->statement_type == OUTPUT) //if the statment type is an output statement
			cout << mem[pc->LHS] << " ";
		else if (pc->statement_type == INPUT)
		{
			mem[pc->op1] = stoi(s2->pop().lexeme); //the input
												   //pop the input from the stack and store it at the memory location requested
		}
		else if (pc->statement_type == ASSIGN)
		{ //if  we get an assin statment find out which one it could be
			switch (pc->operator_type)
			{
			case PLUS:
				mem[pc->LHS] = mem[pc->op1] + mem[pc->op2];
				break;
			case MINUS:
				mem[pc->LHS] = mem[pc->op1] - mem[pc->op2];
				break;
			case MULT:
				mem[pc->LHS] = mem[pc->op1] * mem[pc->op2];
				break;
			case DIV:
				mem[pc->LHS] = mem[pc->op1] / mem[pc->op2];
				break;
			case NOOP:
				mem[pc->LHS] = mem[pc->op1];
				break; // if we are just assigning with 1 operand
			default:
				break;
			}
		}
		else if (pc->statement_type == INVOKE)
		{
			execute_program(pc->code); //run the code in the procedure
		}
		else if (pc->statement_type == DO)
		{
			int copy = mem[pc->op1]; //store the value of op1 in a copy variable this is the number of times we will iterate

			while (copy > 0)
			{
				execute_program(procTable[pc->op2].code); //execute the program
				copy--;
			}
		}
		pc = pc->next; //moce down the list
	}
}

//########################################################################################################################
void Parser::syntax_error()
{
	cout << "SYNTAX ERROR\n";
	exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
	Token t = lexer.GetToken();
	if (t.token_type != expected_type)
		syntax_error();
	return t;
}

// this function simply checks the next token without
// consuming the input
// Written by Mohsen Zohrevandi
Token Parser::peek()
{
	Token t = lexer.GetToken();
	lexer.UngetToken(t);
	return t;
}

// Parsing//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//########################################################################################################################
//parses the begining of the input
void Parser::parse_input()
{
	parse_program();
	parse_inputs();
	return;
}
//########################################################################################################################

void Parser::parse_program()
{
	Token t = lexer.GetToken();
	if (t.token_type == MAIN)
	{
		lexer.UngetToken(t);
		parse_main();
	}
	else if (t.token_type == PROC)
	{
		lexer.UngetToken(t);
		parse_proc_decl_section();
		parse_main();
	}
	else
	{
		syntax_error();
	}
	return;
}
//########################################################################################################################
void Parser::parse_proc_decl_section()
{
	Token t = lexer.GetToken(); // if main we are done with this section
	if (t.token_type == MAIN)
	{
		lexer.UngetToken(t);
		return;
	}
	else if (t.token_type == PROC)
	{
		lexer.UngetToken(t);
		parse_proc_decl();
		parse_proc_decl_section();
	}
	else
		syntax_error();
}
//########################################################################################################################
void Parser::parse_proc_decl()
{
	Token t1 = lexer.GetToken(); //PROC COMSUME TOKEN
	if (t1.token_type != PROC)
	{
		syntax_error();
	}
	procTable[numOfProc].procName = parse_procedure_name().lexeme; //store the name of the procedure
	procTable[numOfProc].code = new linkedList();				   //allocate the linked list on the proc table
	parse_procedure_body(procTable[numOfProc].code);			   //make a list of the code in the table
	Token t2 = lexer.GetToken();								   //ENDPROC CONSUME TOKEN
	numOfProc++;												   //increase the number of procedures in the table
}
//########################################################################################################################
//parses a procedure name and returns a token containing the procedure name
Token Parser::parse_procedure_name()
{								//parses a procedure name and returns a token containing the procedure name
	Token t = lexer.GetToken(); //ID OR NUM , CONSUME TOKEN
	if (t.token_type != ID && t.token_type != NUM)
		syntax_error();
	return t;
}
//########################################################################################################################
void Parser::parse_procedure_body(linkedList *stmtl)
{
	Token t = lexer.GetToken(); //ID || OUPUT || INPUT || DO
	if (t.token_type != OUTPUT && t.token_type != INPUT && t.token_type != DO && t.token_type != ID)
	{ //if the token given isn't any of the expected tokens throw a syntax error
		syntax_error();
	}
	lexer.UngetToken(t);
	parse_statement_list(stmtl);
	return;
}
//########################################################################################################################
//parses main in input
void Parser::parse_main()
{
	Token t = lexer.GetToken(); //get the token but if we get the terminal that is expected you consume it
	if (t.token_type == MAIN)
	{
		parse_procedure_body(list); //parse the list of commands
	}
	else
	{
		syntax_error();
	}
	return;
}
//########################################################################################################################
// this function takes in a linked list of statement and creates a "program " of linked procedures
linkedList *Parser::parse_statement_list(linkedList *stmntl)
{

	Token t = lexer.GetToken(); //if we get a number we have finished parsing the statements and we are in the inputs
	while (t.token_type != END_OF_FILE)
	{ //while we havent reached the end of the statment list
		if (t.token_type == ENDPROC)
		{ //end of procedure
			lexer.UngetToken(t);
			return stmntl; //return the list
		}
		else if (t.token_type != NUM)
		{
			lexer.UngetToken(t);
			stmt_node *node = parse_statement();
			stmntl->add_statement(node);
		}
		else if (t.token_type == NUM)
		{
			lexer.UngetToken(t);
			return stmntl; //return the linked list
		}
		t = lexer.GetToken();
	}
}
//########################################################################################################################
struct stmt_node *Parser::parse_statement()
{
	Token t1 = lexer.GetToken(); // INPUT || OUTPUT || ID || DO
	Token t2 = lexer.GetToken(); // EQUAL || SEMICOLON
	stmt_node *node = nullptr;
	if (t1.token_type == INPUT)
	{
		lexer.UngetToken(t2);
		lexer.UngetToken(t1);
		node = parse_input_statement();
	}
	else if (t1.token_type == OUTPUT)
	{
		lexer.UngetToken(t2);
		lexer.UngetToken(t1);
		node = parse_ouput_statement();
	}
	else if (t1.token_type == DO)
	{
		lexer.UngetToken(t2);
		lexer.UngetToken(t1);
		node = parse_do_statement();
	}
	else if (t1.token_type == ID && t2.token_type == EQUAL)
	{
		lexer.UngetToken(t2);
		lexer.UngetToken(t1);
		node = parse_assign_statement();
	}
	else if (t1.token_type == ID && t2.token_type == SEMICOLON)
	{
		lexer.UngetToken(t2);
		lexer.UngetToken(t1);
		node = parse_procedure_invocation();
	}
	else
	{
		syntax_error();
	}
	return node;
}
//########################################################################################################################
//this function takes in retuns a stmt_node* to be added into the linked list later
stmt_node *Parser::parse_input_statement()
{
	Token t1 = lexer.GetToken(); //INPUT CONSUME
	Token t2 = lexer.GetToken(); //ID	CONSUME
	stmt_node *st = nullptr;
	allocate(t2, table, next_available, mem); //allocate the ID into the symbol table
	if (t1.token_type == INPUT && t2.token_type == ID)
	{
		st = new stmt_node(); //create a new node
		st->statement_type = INPUT;
		st->next = nullptr;
		st->LHS = location_in_s_table(t2, table); //store the location of the left hand side
		st->op1 = location_in_s_table(t2, table); //store the location of  op1
		st->operator_type = NOOP;
	}
	else
	{
		syntax_error();
	}
	parse_expr(st);				 //
	Token t3 = lexer.GetToken(); //SEMICOLON CONSUME
	if (t3.token_type != SEMICOLON)
	{
		syntax_error();
	}
	return st;
}
//########################################################################################################################
stmt_node *Parser::parse_ouput_statement()
{
	Token t1 = lexer.GetToken(); //OUTPUT CONSUME
	Token t2 = lexer.GetToken(); //ID	CONSUME
	if (t1.token_type != OUTPUT || t2.token_type != ID)
	{
		syntax_error();
	}
	allocate(t2, table, next_available, mem); //allocate the variable into the symbol table
	stmt_node *st = new stmt_node();		  //create a new statement node
	st->statement_type = OUTPUT;			  //mkae not of the type of statment we are making
	int x = location_in_s_table(t2, table);	  //make the LHS the location of the ID in the symbol table
	st->LHS = x;
	st->op1 = x;
	st->operator_type = NOOP; //the operator type for this statment is a no-op
	st->next = nullptr;
	parse_expr(st);
	Token t3 = lexer.GetToken(); //SEMICOLON	CONSUME
	if (t3.token_type != SEMICOLON)
	{
		syntax_error();
	}
	return st;
}
//########################################################################################################################
stmt_node *Parser::parse_do_statement()
{
	Token t1 = lexer.GetToken();			  //DO	CONSUME
	Token t2 = lexer.GetToken();			  //ID	CONSUME
	allocate(t2, table, next_available, mem); //allocate the token into the symbol table
	if (t1.token_type != DO || t2.token_type != ID)
	{
		syntax_error();
	}
	stmt_node *st = new stmt_node(); //create a new node
	st->statement_type = DO;
	st->next = nullptr;
	st->op1 = location_in_s_table(t2, table);	 //store the location of how much we will be iterating by
	st->op2 = parse_procedure_invocation()->op1; //store the location of the procedure in memory of op2
	return st;
}
//########################################################################################################################
stmt_node *Parser::parse_procedure_invocation()
{
	Token t1 = lexer.GetToken();			  //ID	CONSUME
	Token t2 = lexer.GetToken();			  //SEMICOLON	CONSUME
	allocate(t2, table, next_available, mem); //allocate id to symbol table
	stmt_node *st = new stmt_node();		  //allocate a new statement node
	if (t1.token_type != ID || t2.token_type != SEMICOLON)
	{
		syntax_error();
	}
	st->statement_type = INVOKE;
	st->code = procTable[location_in_p_table(t1, procTable)].code; //link the statment code with the linked list of the procedure
	st->op1 = location_in_p_table(t1, procTable);				   //store the location of the procedure in the proc_table
	return st;
}
//########################################################################################################################
stmt_node *Parser::parse_assign_statement()
{
	Token t1 = lexer.GetToken();			  //ID	CONSUME
	Token t2 = lexer.GetToken();			  //EQUAL	CONSUME
	allocate(t1, table, next_available, mem); //allocate the symbol to the table
	stmt_node *st = new stmt_node();
	st->statement_type = ASSIGN;
	st->LHS = location_in_s_table(t1, table); //get the address of the left hand side of the assign

	if (t1.token_type != ID || t2.token_type != EQUAL)
	{
		syntax_error();
	}

	parse_expr(st);
	Token t3 = lexer.GetToken(); //SEMICOLON	CONSUME
	if (t3.token_type != SEMICOLON)
	{
		syntax_error();
	}
	return st;
}
//########################################################################################################################
void Parser::parse_operator(stmt_node *s)
{
	Token t = lexer.GetToken(); // ID || MINUS || DIV || MULT	CONSUME!!!!

	if (t.token_type != PLUS && t.token_type != MINUS && t.token_type != DIV && t.token_type != MULT)
		syntax_error();
	s->operator_type = t.token_type; //get the type of operator we will be using  + || - || * || /
	return;
}

//########################################################################################################################
void Parser::parse_expr(stmt_node *st)
{
	Token t = lexer.GetToken(); //If we get a semicolon then we are done parsing expression
	if (t.token_type == SEMICOLON)
	{
		lexer.UngetToken(t);
		return;
	}
	else
	{
		lexer.UngetToken(t);
		Token temp = parse_primary();
		st->op1 = location_in_s_table(temp, table); //store the value of the variable in op1

		t = lexer.GetToken(); //check if expression is done
		if (t.token_type == SEMICOLON)
		{ //if we get a semicolon the expression is done
			lexer.UngetToken(t);
			st->operator_type = NOOP;
			return;
		}
		lexer.UngetToken(t); //unget the token
		parse_operator(st);

		temp = parse_primary();
		st->op2 = location_in_s_table(temp, table); //store the address of op2
	}
}

//########################################################################################################################
Token Parser::parse_primary()
{
	Token t = lexer.GetToken(); //ID || NUM
	if (t.token_type == ID || t.token_type == NUM)
	{
		allocate(t, table, next_available, mem); //allocate the token onto the symbol table
	}
	else
	{
		syntax_error();
	}
	return t; //return the token
}
//########################################################################################################################
void Parser::parse_inputs()
{
	Token t = lexer.GetToken(); // NUM	CONSUME!!!!!!
	if (t.token_type == NUM)
	{
		//do stuff here
		stack->push(t); //store the token on the stack
		parse_inputs(); //recurse
	}
	else if (t.token_type == END_OF_FILE)
	{
		lexer.UngetToken(t);
		//program is done parsing input
	}
	else
	{
		syntax_error();
	}
	return;
}
//########################################################################################################################
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{

	LexicalAnalyzer lexer;
	Token token;
	Parser p;			   //make a parser object to parse the input
	list->start = nullptr; //initialize the start of the list to be a nullptr
	for (int i = 0; i < 1000; i++)
	{ //go through and make all of the quantities in the tables 0
		mem[i] = 0;
		table[i].location = 0;
		table[i].constant = false;
	}
	p.parse_input();
	while (stack->isEmpty() == false)
	{
		s2->push(stack->pop());
	}

	execute_program(list); //execute the program

	return 0;
}
