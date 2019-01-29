/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"

class Parser {
private:
	LexicalAnalyzer lexer;

	void syntax_error();
	Token expect(TokenType expected_type);
	Token peek();

public:
	//to do//////////////////////////////////
	
	
	
	
	
	void parse_procedure_body();
	void parse_statement_list();
	void parse_statement();
	void parse_input_statement();
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

	//////////////////////////////////////////////
	/*DEF DONE*/




	//////////////////////////////////////////////
};

#endif

