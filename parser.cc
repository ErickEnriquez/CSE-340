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

// Parsing
bool Parser::parse_input(Token t){
	
	if (t.token_type == MAIN) {
		cout << "main\n";
		t = lexer.GetToken();
	}
	else {
		syntax_error();
	}
	return false;
}

bool Parser::parse_operator(){
	Token t = lexer.GetToken();
	if(t.token_type != PLUS || t.token_type != MINUS || t.token_type != DIV || t.token_type != MULT)
		syntax_error();
	return true;
}


int main()
{
	LexicalAnalyzer lexer;
	Token token;
	
	
}
