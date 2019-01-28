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

//parses the begining of the input
void Parser::parse_input(){
	Token t = lexer.GetToken();
	if(t.token_type == MAIN){
		lexer.UngetToken(t);
		parse_main();
	}
	else if( t.token_type == PROC){
		lexer.UngetToken(t);
		parse_proc_decl_section();
	}
	else{
		syntax_error();
	}
	parse_inputs();
		cout<<"parse successful\n";
		return;
}

//parses main in input
void Parser::parse_main(){
	Token t = lexer.GetToken();//get the token but if we get the terminal that is expected you consume it
	if(t.token_type == MAIN){
		parse_procedure_body();
	}
	else{
		syntax_error();
	}
	return;
}

void Parser::parse_procedure_body(){
	parse_statement_list();
	return;
}
void Parser::parse_statement_list(){
	Token t = lexer.GetToken();//if we get a number we have finished parsing the statements and we are in the inputs
	if( t.token_type != NUM){//while there are more statements to parse
		parse_statement();
	}
	return;
}

void Parser::parse_statement(){
	Token t1 = lexer.GetToken();// INPUT || OUTPUT || ID || DO
	if(t1.token_type == INPUT){
		lexer.UngetToken(t1);
		parse_input_statement();
	}
	else if(t1.token_type ==OUTPUT){
		lexer.UngetToken(t1);
		parse_ouput_statement();
	}
	else if(t1.token_type == DO){
		lexer.UngetToken(t1);
		parse_do_statement();
	}
	else if(t1.token_type == ID){
		Token t2 = lexer.GetToken();// EQUAL || SEMICOLON
		if(t2.token_type == EQUAL){
			lexer.UngetToken(t2);
			lexer.UngetToken(t1);
			parse_assign_statement();
		}
		else if(t2.token_type == SEMICOLON){

		}
		else{
			syntax_error();
		}
	}
	else{
		syntax_error();
	}

	
}


void Parser::parse_operator(){
	Token t = lexer.GetToken();
	if(t.token_type != PLUS || t.token_type != MINUS || t.token_type != DIV || t.token_type != MULT)
		syntax_error();
	return ;
}

bool Parser::parse_procedure_name(){
	Token t = lexer.GetToken();
	if(t.token_type != ID || t.token_type != NUM)
		syntax_error();
	return true;
}
//
void Parser::parse_inputs(){
	Token t = lexer.GetToken();
	if(t.token_type == NUM){
		//do stuff here
		parse_inputs();
	}
	else if(t.token_type == END_OF_FILE){
		//program is done parsing input
	}
	else{
		syntax_error();
	}
	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
	LexicalAnalyzer lexer;
	Token token;
	
	
}
