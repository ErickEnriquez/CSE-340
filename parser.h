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
	void parse_input();
	bool parse_program();
	bool parse_proc_decl_section();
	bool parse_proc_decl();
	bool parse_procedure_name();
	void parse_procedure_body();
	void parse_statement_list();
	void parse_statement();
	bool parse_input_statement();
	bool parse_ouput_statement();
	bool parse_procedure_invocation();
	bool parse_do_statement();
	bool parse_assign_statement();
	bool parse_expr();
	void parse_operator();
	bool parse_primary();
	void parse_main();
	void parse_inputs();

	/////////////////////////////////////////////
	/*done?*/
	




	//////////////////////////////////////////////
	/*DEF DONE*/




	//////////////////////////////////////////////
};

#endif

