/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include<set>
#include <unordered_set>
#include "defn.h"

using namespace std;



vector<Rules> rules;//rules as tokens
vector<Token> Non_term;
vector<Token> terminal;
vector<Symbol> Symbols;
LexicalAnalyzer lexer;


bool isRHSGenerating(vector<Symbol> RHS , bool * array);//check to see if a vector LHS is generating
void parse_rules();//parses a set of rules
void printRules();//prints out the rule data structure
void printSymbols();//print out symbol table
bool isInTable(vector<Token> , Token);//checks if a token is already in the table
void task1();//prints out the non-terminals followed by terminals
void createSymbolTable();//creates a symbol table
void assign_indexes_to_rules();//takes our Rules and assigns all of them the index they need
int getIndex(Symbol s);//gets the index of a symbol from a given symbol table
bool* generating_symbols();//goes through and finds all of the generating symbols
void add_epsilon_to_rules(); //this function goes through and adds an epsilon symbol to any rule that has a size of 0 which is rules that would have epsilon
void useless_symbols();//this function will generate the useless symbols
bool* reachable_symbols();//this function goes through all of the rules and finds the symbols that are reachable

int main(int argc, char* argv[])
{
	int task;
	

	if (argc < 2)
	{
		std::cout << "Error: missing argument\n";
		return 1;
	}

	

	task = atoi(argv[1]);

	parse_rules();//parse the rules into the right structure
	createSymbolTable();//create my symbol table

	switch (task) {
	//print out a list of non-terminals and terminals
	case 1:
		task1();
		break;

	case 2: {
		useless_symbols();
		printRules();
	}
		break;

	case 3: {
		//FirstSets
		FirstSets first(Symbols, rules);
		first.generateFirstSet();
		first.printFirstSets();
	}
		break;

	case 4: {
		//followSets
		FirstSets follow(Symbols, rules);
		follow.generateFollowSet();
		follow.printFollow();
	}
		break;

	case 5: {
		FirstSets task5(Symbols, rules);
		task5.generateFollowSet();
		if (task5.task5() == true) {
			cout << "YES";
		}
		else
			cout << "NO";
	}
		break;

	default:
		std::cout << "Error: unrecognized task number " << task << "\n";
		break;
	}
	return 0;
}


bool isRHSGenerating(vector<Symbol> RHS , bool* array){
	int x = 0;
	for (int i = 0; i < RHS.size(); i++) {
		if (array[RHS[i].index] == true ) {
			x++;
			}
	}
	if (x == RHS.size()) //all of the symbols are generating?
		return true;
	else
		return false;
}

//parses a grammar into a set of Rules struct
void parse_rules(){
	int count = 0;
	Token t;
	t = lexer.GetToken();//start symbol
	while (t.token_type != DOUBLEHASH) {
		Rules temp;//create a temp object
		Symbol s; //create a symbol object
		if (count == 0) {//will mark the starting sumbol
			s.StartingSymb = true;
			count++;
		}
		s.token = t;//assign the token to the symbol
		temp.LHS = s ;//assign the left symbol 
		t = lexer.GetToken();
		while (t.token_type != HASH) {
			if (t.token_type != ARROW) {
				s.token = t;
				temp.RHS.push_back(s);//push the token onto the vector
			}
			t = lexer.GetToken();
		}
		rules.push_back(temp);//push the rule onto the vector
		t = lexer.GetToken();//get the next token whether it's a LHS or a Double Hash
	}
	
}

//utility functio to print out the rules
void printRules() {
	for (Rules r : rules) {
		cout << r.LHS.token.lexeme  << " -> " ;
		for (Symbol t : r.RHS) {
			cout << t.token.lexeme << " " ;
		}
		cout << "\n";
	}
}

/*creates a symbol table from our given grammar adds epsilon to our rules  , and also assigns an index to each of our symbols*/
void  createSymbolTable() {
	unordered_set<string> table;
	for (Rules r : rules) {
		table.insert(r.LHS.token.lexeme);//insert the lexeme into the string so we won't enter duplicates into the table
	}
	for (Rules rul : rules) {
		if (isInTable(Non_term,rul.LHS.token) == false)//if the value is not already in the table
			Non_term.push_back(rul.LHS.token);//push the token into the non terminals vector
		for (Symbol s : rul.RHS) {//look at the right hand side
			if (table.count(s.token.lexeme) > 0) {//if the count is greater than 1 then the token is a non-terminal
				if (isInTable(Non_term, s.token) == false)
					Non_term.push_back(s.token);//push the non-terminal onto the vector because we know its a non-terminal and its not already in there
			}
			//if the count is not greater than 0 then the token is a terminal
			else {
				if (isInTable(terminal, s.token) == false) {
					s.token.terminal = true;//mark the symbol  as a terminal
					terminal.push_back(s.token);//push the token onto the terminal vector since its the next and isn't already there

				}
			}
		}
	}
	Token epsilon; epsilon.lexeme = "#"; epsilon.token_type = EPSILON; epsilon.terminal = true;
	Symbol temp; temp.token = epsilon; temp.index = 0; Symbols.push_back(temp);//make a epsilon and place it on the table
	Token eof; eof.lexeme = "EOF"; eof.token_type = END_OF_FILE; eof.terminal = false;//make a EOF and place it on the table
	temp.token = eof; temp.index = 1; Symbols.push_back(temp);
	int i = 2;
	for (Token t : Non_term) {
		temp.token = t;
		temp.index = i;
		i++;
		Symbols.push_back(temp);
	}
	for (Token t : terminal) {
		temp.token = t;
		temp.index = i;
		i++;
		Symbols.push_back(temp);
	}
	assign_indexes_to_rules();//mark the symbols of the grammar with the indexes they are located at
	add_epsilon_to_rules();//add any epsilon into the rules where needed
}

//this function is used in the create symbol table to assing all of the symbols in a grammar to an index in a  symbol table
void assign_indexes_to_rules(){
	for(int i  = 0 ; i < rules.size() ; i++){
		rules[i].LHS.index = getIndex(rules[i].LHS);
		for (int j = 0; j < rules[i].RHS.size(); j++) {
			rules[i].RHS[j].index = getIndex(rules[i].RHS[j]);
		}
	}
}


/*this function checks if a symbol has already been added into a vector*/
bool isInTable(vector<Token> vec , Token Key) {
	for (Token t : vec) {
		if (t.lexeme == Key.lexeme) {
			return true;
		}
	}
	return false;
}

//this function accomlpishes what is required for task 1 all it does it ouput the non-terminals followed by terminals in order which they appear
void task1() {
	for (Token t : Non_term) {
		cout << t.lexeme << " ";
	}
	for (Token t : terminal) {
		cout << t.lexeme << " ";
	}
}

/*this checks a grammar and marks the rules that are generating */
bool* generating_symbols() {
	bool* array = new bool[Symbols.size()];//set up the array that will tell us if the rules are generating
	for (int i = 0; i < Symbols.size(); i++) {
		if (Symbols[i].token.terminal == true)
			array[i] = true;
		else
			array[i] = false;
	}
	bool change = true;
	while (change != false) {
		change = false;
		for (int i = 0; i < rules.size(); i++) {
			if (isRHSGenerating(rules[i].RHS , array) == true && array[rules[i].LHS.index] == false) {//if the rule is not already generating and its rhs is already generating
				array[rules[i].LHS.index] = true;
				change = true;
			}
		}
	}
	return array;//return the array which contains all of the symbols that are generating
}

/*this function goes and adds an epsilon symbol into our rules */
void add_epsilon_to_rules(){
	Token temp;
	temp.lexeme = "#"; temp.terminal = true; temp.token_type = EPSILON; 
	Symbol s;
	s.token = temp;
	s.generating = true;
	s.index = 0;
	for (int i = 0; i < rules.size(); i++) {
		if (rules[i].RHS.empty() == true)//if the RHS IS empty
			rules[i].RHS.push_back(s);//push the createad symbol onto the vector
	}
}
/*this funtion calculates the useless symbols for a given grammer which is needed for task 2*/
void useless_symbols(){
	bool deletion_occurred = false;
	do{
		deletion_occurred = false;//loops through and checks if a deletion has occured
		bool* generating = generating_symbols();//get the generating symbols
		bool* reachable = reachable_symbols();//get the reachable symbols
		bool change = true;
		while (change != false) {
			change = false;
			for (int i = 0; i < rules.size(); i++) {
				if (generating[rules[i].LHS.index] == false || reachable[rules[i].LHS.index] == false) {//if the LHS is non-generating or non-reachable remove the symbol
					rules.erase(rules.begin() + i);//remove the entire rule
					change = true;
					deletion_occurred = true;
				}
				else {
					for (int j = 0; j < rules[i].RHS.size(); j++) {
						if (generating[rules[i].RHS[j].index] == false || reachable[rules[i].LHS.index] == false) {//if any of the synbols on the RHS are useless remove the entire rule
							change = true;
							rules.erase(rules.begin() + i);
							deletion_occurred = true;
						}
					}
				}
			}
		}
	} while (deletion_occurred == true);
}

/*this function goes through and returns an array of the reachable symbols*/
bool * reachable_symbols(){
	bool * reachable = new bool[Symbols.size()];//make a array 
	for (int i = 0; i < Symbols.size(); i++) {
		reachable[i] = false;//mark all of the symbols as non-reachable
	}
	auto r = rules.begin();//make an iterator to the first rule in the grammar
	int x = r->LHS.index;//get the index of the starting symbol 
	reachable[x] = true;//mark the starting symbol as reachable
	bool change = true;
	while (change != false) {
		change = false;
		for (int i = 0; i < rules.size(); i++) {
			if (reachable[rules[i].LHS.index] == true) {
				for (int j = 0; j < rules[i].RHS.size(); j++) {//go through and mark all of the symbols as reachable
					if (reachable[rules[i].RHS[j].index] == false) {
						change = true;
						reachable[rules[i].RHS[j].index] = true;//mark the symbol as reachable
					}
				}
			}
		}
	}
	return reachable;
}

//utility fruntion that prints out the symbols of a grammar
void printSymbols() {
	for (Symbol s : Symbols) {
		cout << s.token.lexeme << " " << s.index << endl;
	}
}


/*gets the index of a symbol from the symbol table*/
int getIndex(Symbol s) {
	for (Symbol sym : Symbols) {
		if (sym.token.lexeme == s.token.lexeme)
			return sym.index;
	}
	return -1;
}


