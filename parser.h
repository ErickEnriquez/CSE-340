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
	bool parse_input(Token t);
	bool parse_program();
	bool parse_proc_decl_section();
	bool parse_proc_decl();
	bool parse_procedure_name();
	bool parse_prodecdure_body();
	bool parse_statement_list();
	bool parse_statement();
	bool parse_input_statement();
	bool parse_ouput_statement();
	bool parse_procedure_invocation();
	bool parse_do_statement();
	bool parse_assign_statement();
	bool parse_expr();
	bool parse_operator();
	bool parse_primary();
	bool parse_main();
	bool parse_inputs();

	/////////////////////////////////////////////
	/*done?*/




	//////////////////////////////////////////////
	/*DEF DONE*/




	//////////////////////////////////////////////
};

#endif

