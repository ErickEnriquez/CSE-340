/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"
#include "defn.h"



class Parser {
private:
	LexicalAnalyzer lexer;

	void syntax_error();
	Token expect(TokenType expected_type);
	Token peek();

public:

	struct linked_list* head;


	/////////////////////////////////////////////
	/*done?*/
	void parse_input();
	void parse_program();
	void parse_proc_decl_section();
	void parse_proc_decl();
	void parse_procedure_name();
	void parse_procedure_body();
	void parse_statement_list();
	struct stmt_node* parse_statement();
	struct stmt_node* parse_input_statement();
	struct stmt_node* parse_ouput_statement();
	void parse_procedure_invocation();
	void parse_do_statement();
	stmt_node* parse_assign_statement();
	void parse_expr(stmt_node*s);
	void parse_operator(stmt_node*s);
	Token parse_primary();
	void parse_main();
	void parse_inputs();


	//////////////////////////////////////////////
	/*DEF DONE*/




	//////////////////////////////////////////////
};

#endif

