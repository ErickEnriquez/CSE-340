#pragma once
#include "lexer.h"
#include <set>
#include <algorithm>
#include <vector>

using namespace std;

struct Symbol {
	Token token;
	int index;
	bool generating = false;
	bool StartingSymb = false;
};

struct Rules {//each rule will be stored as a LHS token and a RHS vector of tokens 
	Symbol LHS;
	std::vector<Symbol> RHS;
};

/*this is where we will be generating our first sets*/
struct FirstSets {
	
	std::string nameOfSet;//name of the first set
	std::vector<int> members;//vector that will hold all of the members
	bool terminal = false;
	bool change = false;
	int index;
	std::vector<FirstSets> list;//to be used by our follow
	std::vector<FirstSets> follow;//to be used by our first sets
	std::vector<Symbol> symbolTable;
	std::vector<Rules> grammar;

	FirstSets() {//default constructor

	}
	FirstSets(std::vector<Symbol> table , std::vector<Rules> y ) {
		FirstSets temp;
		this->symbolTable = table;
		this->grammar = y;
	
		//RULE 1 && RULE 2/////////////////////
		for (Symbol s : symbolTable) {
			if (s.token.terminal == true) {
				temp.nameOfSet = s.token.lexeme;
				temp.members.push_back(s.index);
				temp.terminal = true;
				temp.index = s.index;
				list.push_back(temp);
				temp.members.clear();
			}
			else {
				temp.nameOfSet = s.token.lexeme;
				temp.index = s.index;
				temp.terminal = false;
				list.push_back(temp);
			}
		}
		/////////////////////////////
		//INITIALIZING OUR FOLLOW SET

		for (Symbol s : symbolTable) {
			if (s.token.terminal == true) {
				temp.nameOfSet = s.token.lexeme;
				temp.members.push_back(s.index);
				temp.terminal = true;
				temp.index = s.index;
				follow.push_back(temp);
				temp.members.clear();
			}
			else {
				temp.nameOfSet = s.token.lexeme;
				temp.index = s.index;
				temp.terminal = false;
				follow.push_back(temp);
			}
		}




		///////////////////////////////
	}

	/*utility function to print out first sets*/
	void printFirstSets() {
		for (FirstSets first : list) {
			if (first.terminal == false && first.nameOfSet != "EOF") {
				std::cout << "FIRST(" << first.nameOfSet << ") = { ";
				int x = first.members.size();
				for (int i : first.members) {
					vector<FirstSets>::iterator it = list.begin();
					advance(it, i);
					x--;
					if (x > 0) {
						cout << it->nameOfSet << ", ";
					}
					else
					cout << it->nameOfSet << " ";
				}
			std::cout << "}\n";
			}
		}
	}

	/*this function will return the union of 2 vectors  - epsilon*/
	vector<int> setUnion(vector< int> a, vector<int> b) {
		
		std::unordered_set<int> set;
		if(containsEpsilon(a) == false )
			set.insert(0);//insert epsilon first so it will not be added into our set union
		vector<int> newset;
		for (int i : a) {
			if (set.count(i) == 0) {//if the count is 0 then we havent seen the number yet
				set.insert(i);//inset the number into the set
				newset.push_back(i);
			}
		}
		for (int i : b) {
			if (set.count(i) == 0) {//if the count is 0 then we havent seen the number yet
				set.insert(i);//inset the number into the set
				newset.push_back(i);
			}
		}
		return newset;
	}

	/*this function removes epsilon from our vector until we need it , to be used with our set union operator*/
	bool has_epsilon(std::vector<FirstSets>::iterator location) {
		for (int i : location->members) {
			if (i == 0)
				return true;;
		}
		return false;
	}

	//generate first sets
	vector<FirstSets> generateFirstSet() {
		
		do {
			change = false;
			vector<Rules>::iterator rule = grammar.begin();//make a rule iterator for the rhs
			int count = numberOfElementsInFirstSets(list.begin());//get the number of elements in first sets
			while (rule != grammar.end()) {
				rule3(rule);
				rule++;
			}
			int newCount = numberOfElementsInFirstSets(list.begin());//get the number of elements in first sets after a pass
			if (newCount != count)//if theses numbers arent equal then there were things added to the first sets
				change = true;
		} while (change == true);

		/*final step after the generation is to sort them into the order they appear*/
		vector<FirstSets>::iterator it = list.begin();
		while (it != list.end()) {
			std::sort(it->members.begin(), it->members.end());
			it++;
		}
		return list;
	}

	/*this function gets the location of a LHS */
	int getLocation(vector<Symbol> vec , int i) {
		vector<Symbol>::iterator it = vec.begin();
		while (it != vec.end()) {
			if (it->index == i)
				return i;
			it++;
		}
		return -1;
	}

	void rule3(vector<Rules>::iterator ruleIterator) {
		vector<Symbol>::iterator rhsIterator = ruleIterator->RHS.begin();//make an iterator to the first rule
		do {
			int x = rhsIterator->index;//get the index for the LHS symbol in the symbol table
			vector<FirstSets>::iterator firstSetIterator = list.begin();//get an iterator to the FirstSet vector
			advance(firstSetIterator, x);//advance the firstSetIterator to be the location of our symbol on the LHS
			if (firstSetIterator->members.empty() == true)//if the symbol is empty
				return;
			else {
				int w = ruleIterator->LHS.index;//get the index for our Symbol on the RHS of the Rule
				vector<FirstSets>::iterator LHSFirst = list.begin();//make an iterator for our LHS in the first Set
				advance(LHSFirst, w);

					LHSFirst->members = setUnion(LHSFirst->members, firstSetIterator->members);//do a set union
			
				return Rule4(rhsIterator,ruleIterator,firstSetIterator,LHSFirst);
			}

		} while (rhsIterator != ruleIterator->RHS.end());
	}
	/*checks if the rule generate epsilon and if so adds the first of the collowing */
	void Rule4(vector<Symbol>::iterator RHSIterator, vector<Rules>::iterator RulesIterator, vector<FirstSets>::iterator terminal, vector<FirstSets>::iterator non_terminal)
	{ 
		RHSIterator++;
		if(RHSIterator != RulesIterator->RHS.end() ) {
			RHSIterator--;//reset rhs
			while (RHSIterator != RulesIterator->RHS.end()) {
				if (has_epsilon(terminal) == true) {
					RHSIterator++;//move this to the next symbol of the rule
					if (RHSIterator == RulesIterator->RHS.end())
						break;
					int i = RHSIterator->index;
					terminal = list.begin();
					advance(terminal, i);
					non_terminal->members = setUnion(non_terminal->members, terminal->members);

				}
				else
				RHSIterator++;
			}
		}
		rule5(RulesIterator, non_terminal, RulesIterator->RHS.begin());//here we will check if all of the RHS generates epsilon
	}

	/*this function will add epsilon if the entire rhs of a rule generates epsilon*/
	void rule5(vector<Rules>::iterator ruleIterator, vector<FirstSets>::iterator non_terminal, vector<Symbol>::iterator rhsIter) {
		if (isEntireRHSGenerateEpsilon(rhsIter, ruleIterator) == true && containsEpsilon(non_terminal) == false)
			non_terminal->members.push_back(0);
	}

	int numberOfElementsInFirstSets(vector<FirstSets>::iterator firstSets) {
		int count = 0; 
		while (firstSets != list.end()) {
			for (int i : firstSets->members) {
				count++;
			}
			firstSets++;
		}
		return count;
	}

	/*checks if a first of a rhs would generate epsilon*/
	bool containsEpsilon(vector<FirstSets>::iterator rhs) {
		for (int i : rhs->members) {
			if (i == 0)
				return true;
		}
		return false;
	}

	bool containsEpsilon(vector<Symbol>::iterator rhs) {
		int x =  rhs->index;
		vector<FirstSets>::iterator f = list.begin();
		advance(f, x);
		return containsEpsilon(f);
	}

	bool containsEpsilon(vector<int> a) {
		for (int i : a) {
			if (i == 0)
				return true;
		}
		return false;
	}
	/*this fcuntion should go through an entire rhs and check if all of the symbols generate epsilon if they do then we can add epsilon to lhs*/
	bool isEntireRHSGenerateEpsilon(vector<Symbol>::iterator rhs , vector<Rules>::iterator rule)
	{
		while (rhs != rule->RHS.end()) {
			if (containsEpsilon(rhs) == false)
				return false;
			rhs++;
		}
		return true;
	}

	//prints out the follow sets
	void printFollow() {
		for (FirstSets f : follow) {
			if (f.terminal == false && f.nameOfSet != "EOF" ) {
				cout << "FOLLOW(" << f.nameOfSet << ") = { ";
				int x = f.members.size() ;
				for (int i : f.members) {
					vector<FirstSets>::iterator t = follow.begin();
					advance(t, i);
					x--;
					if (x > 0) {
						if (t->nameOfSet == "EOF") {
							cout << "$, ";
						}
						else
							cout << t->nameOfSet << ", ";
					}
					else
					{
						if (t->nameOfSet == "EOF")
							cout << "$ ";
						else
							cout << t->nameOfSet << " ";
					}
				}
				cout << "}\n";
			}
		}
	}

	int numberOfElemInFOLLOW(vector<FirstSets>::iterator firstSets) {
		int count = 0;
		while (firstSets != follow.end()) {
			for (int i : firstSets->members) {
				count++;
			}
			firstSets++;
		}
		return count;
	}

	void generateFollowSet() {
		generateFirstSet();//generate our first set
		//RULE 1
		///////////////////////////////////////////////////////////
		vector<Rules>::iterator r = grammar.begin();
		vector<FirstSets>::iterator temp = follow.begin();
		advance(temp, r->LHS.index);
		temp->members.push_back(1);
		///////////////////////////////////////////////////////////
		bool change = false;
		do {
			int count = numberOfElemInFOLLOW(follow.begin());//get the number before we do a pass
			change = false;
			///////////////////////////////////////////////////////////////////
			while (r != grammar.end()) {
				cout << "new round\n-------------------------------------\n";
				printFollow();
				followRule2(r);
				cout << "\n-after rule 2------------------------------------------\n";
				printFollow();
				followRule5(r);
				cout << "\nafter rule 5----------------------------------------------\n";
				printFollow();
				r++;
			}
			/////////////////////////////////////////////////////////////////////
			int newCount = numberOfElemInFOLLOW(follow.begin());
			if (count != newCount) {
				change = true;
				r = grammar.begin();//reset r for another pass of the rules
			}
		} while (change == true);
		auto it = follow.begin();
		while (it != follow.end()) {
			std::sort(it->members.begin(), it->members.end());
			it++;
		}
	}

	/*this rule added the first of A to follow of B of a */
	void followRule2(vector<Rules>::iterator RuleIterator) {
		auto Right = RuleIterator->RHS.rbegin();//iterator to the rightmost symbol of the grammar
		vector<FirstSets>::iterator A = follow.begin();//our 
		advance(A, RuleIterator->LHS.index);//move A to the correct index in out first set
		for (Right; Right != RuleIterator->RHS.rend(); Right++) {//loop through the Rule RHS from right to left
			auto B = follow.begin();
			auto first = list.begin();
			advance(B, Right->index);
			advance(first, Right->index);
			if (B->terminal == false)// if B is not a terminal
			{
				B->members = setUnion(B->members, A->members);//add follow of A to Follow of B
			} 
			if (B->terminal == true)
				return;
			if (containsEpsilon(first) == false)//if B generates Epsilon we are done else we 
				return;
		}

	}



	void followRule5(vector<Rules>::iterator RulesIterator) {
		auto left = RulesIterator->RHS.rbegin();
		int count = 0;
		while (left != RulesIterator->RHS.rend()) {
			if (count == 0 && left !=RulesIterator->RHS.rend()) {
				left++;
				count++;
			}
			if (left == RulesIterator->RHS.rend()) //if the rhs only has 1 term return
				return;
			auto right = left;//the right pointer
			right--;
			auto A = follow.begin();
			auto B = list.begin();
			advance(A, left->index);
			advance(B, right->index);
			if (left->token.terminal == false) {//if the right symbol is a non_terminal
				bool done = false;
				while (right != RulesIterator->RHS.rend() && done == false) {
					A->members = setUnion(A->members, B->members);
					if (containsEpsilon(B) == false)
						done = true;
					if (right == RulesIterator->RHS.rend())
						break;
					right++;
				}
			}
			left++;
		}
		
		
	}



	/*task 5*/
	bool task5() {
		generateFollowSet();
		auto iterator = list.begin();
		auto followIterator = follow.begin();
		vector<int> intersection;//will be usd to hold the intersection
		//here we will be checking the if a Symbol generates epsilon then its first set intersected with its follow set should be the empty set
		while (iterator != list.end()) {

			if (iterator->containsEpsilon(iterator) == true && iterator->nameOfSet != "#") {//if our symbol generates epsilon
				set<int> a(iterator->members.begin(), iterator->members.end());
				set<int> b(followIterator->members.begin(), followIterator->members.end());
				set_intersection(a.begin(), a.end(), b.begin(), b.end(), intersection.begin());
				if (intersection.empty() == false) {
					return false;
				}
				intersection.clear();
				followIterator++;
				iterator++;
			}
			else {
				iterator++;
				followIterator++;
			}
		}
		return true;

	}

};