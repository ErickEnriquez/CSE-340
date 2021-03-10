#ifndef _defn_H
#define _defn_H


#include "lexer.h"
#include <vector>
#include <string>




struct Variables{
  std::string Name;//name of the variable
  std::string Type; //name of the variable
  bool refernced = false;//flags if the variable has been referenced 
  int line_declared;//line the variable was declared
  int line_referenced; // line the variable was referenced
};

struct decl_error{//this is where we will be storing our declaration error WE WILL ONLY GET AT MOST 1 DECLARATION ERROR PER TEST CASE
  std::string error_code;
  std::string symbol_name;
  bool found;//marks if this error exists
};

struct type_mismatch{//this is where we will be storing our WE WILL ONLY GET AT MOST 1 TYPE_MISMATCH ERROR PER TEST CASE
  int line_no;
  std::string constraint;
  bool found;//marks if this error exists
};

struct uninitialized{//this will be storing our uninitialized variables
  std::string symbol_name;
  int line_referenced;
   bool found;//marks if this error exists
};


struct Scope{
  std::vector<Variables> variables;
  Scope* parent;
};

class program{
    public:
    ////////////////////////////parser functions///////////////////////////
    void parse_program();
    void parse_scope();
    void parse_scope_list();
    void parse_var_decl();
    void parse_id_list();
    std::string parse_type_name();
    void parse_stmt_list();
    void parse_stmt();
    void parse_assign_stmt();
    void parse_while_stmt();
    int parse_expr();
    int parse_primary();
    void parse_condition();
    /////////////////////////////////////////////////////////////////////////
    void syntax_error();


    bool is_in_scope(std::string);//this function is going to check to make sure that a variable hasn't been declared in a scope
    void insert_into_scope(std::vector<Token>::iterator , std::string type);
    void print_errors(decl_error, type_mismatch,std::vector<uninitialized>);//utility function to print out the different error codes we have
    bool is_var_declared(Token t);//this function is going to go through and check if a particular variable_on a statement has been declared
    bool is_var_referenced(Token t);//checks if the variable has been referenced (Don't think i'll need but it's already been made)
    void mark_as_referenced(Token t);//goes through and marks the variable on a LHS of an assign statment as referenced

    std::string variables_been_referenced(Scope*); //this function is going to go in and right before we delete a scope we check to see if it has been referenced if so then we can delete just fine if not the we mark it 

    int check_ID_type(Scope*, Token);//this returns the type of variable used if you get and ID
    void check_LHS(Scope* , Token ,int );//this checks the LHS of a scope and will mark C1 and C2 Errors in here

    int parse_operator();//takes in an operator and returns it's value
    int type_check(int , int , int);//takes in an operator and 2 types and returns a type
    int type_check(int, int);//overloaded for unary operator

    int resolve_arithmetic(int, int , int);//resolves type of arithmetic operator
    int resolve_relational(int, int , int);//resolves type of relational expression
    int resolve_binary_bool(int, int , int);//resolves type of binary_boolean
    

    void print_no_error_vars();//this function is just going to go through and print the variables if no other variable has occured
    int get_line_referenced(Scope* , Token);//returns the line referenced of a variable
    int get_line_declared(Scope* , Token);//returns the line where the variable was declared

};

#endif