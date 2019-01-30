/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"


	struct stmt_node// node for every statment
{
    int statement_type;
    int LHS;
    int operator_type;
    int op1;
    int op2; 
    struct stmt_node* next;
};

struct linkedList{//start of the statements
    struct stmt_node* start;
};

class Parser {
private:
	LexicalAnalyzer lexer;

	void syntax_error();
	Token expect(TokenType expected_type);
	Token peek();

public:

	struct linked_list* head;

	//to do//////////////////////////////////
	
	struct stmt_node* parse_input_statement();
	void parse_ouput_statement();
	void parse_procedure_invocation();
	void parse_do_statement();
	void parse_assign_statement();
	void parse_expr();
	void parse_operator();
	void parse_primary();
	void parse_main();
	void parse_inputs();

	/////////////////////////////////////////////
	/*done?*/
	void parse_input();
	void parse_program();
	void parse_proc_decl_section();
	void parse_proc_decl();
	void parse_procedure_name();
	void parse_procedure_body();
	void parse_statement_list();
	void parse_statement();

	//////////////////////////////////////////////
	/*DEF DONE*/




	//////////////////////////////////////////////
};

#endif

