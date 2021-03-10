#include "lexer.h"
#include "compiler.h"
#include <vector>

class Parser{
    public:
    TokenType peek();//can be used to peek
    Token expect(TokenType);//use this function to make the parser cleaner
    void syntax_error();//should not use this but might be useful in debugging
    ///////////////PARSE FUNCTIONS/////////////////////////
    ///////////////////////////////////////////////////////
    StatementNode* parse_program();
    void parse_var_section();
    void parse_id_list();
    StatementNode* parse_body();
    StatementNode* parse_stmt_list();
    StatementNode* parse_stmt();
    StatementNode* parse_assign_stmt();
    ValueNode** parse_expr();//return an array 
    ValueNode* parse_primary();
    void parse_op();
    StatementNode* parse_print_stmt();
    StatementNode* parse_while_stmt();
    StatementNode* parse_if_stmt();
    struct condition parse_condition();
    ConditionalOperatorType parse_relop();
    StatementNode* parse_switch_stmt();//////////////////////need return statement
    StatementNode* parse_for_stmt();
    StatementNode* parse_case_list(Token t , StatementNode* );////////////////////////need return statement
    StatementNode* parse_case(Token t ,StatementNode*);/////////////////////////////need return statement
    StatementNode* parse_default_case(Token t);////////////////////need return statement
    //////////////////////////////////////////////////////

   
    struct memory* mem;//head of our linked list od value nodes
    ValueNode* check_memory(Token);//goes through and looks through memory and retrieves pointer to location of our "memory"
    void insert_into_memory(Token);//adds something into memory table

    StatementNode* check_true_branch(StatementNode*,StatementNode* );//us this to mark our true's goto to the right place
    StatementNode* list;//this is the list of the statments that will be generated when we parse. this is what is returned by the parse Statement list
    StatementNode* append_statement(StatementNode* , StatementNode*);//appends a new statment to a statment list
    StatementNode* link_goto (StatementNode* , StatementNode*); // this is what we will use to link all of the goto_statements to the no_op at the end
    ArithmeticOperatorType getOperator(Token );//returns the arithmetic operator
    ConditionalOperatorType getRelop(Token);//returns the relational operator
};//


struct memory{
ValueNode* data;
memory* next;
};

struct condition{
    ValueNode* operand1;
    ConditionalOperatorType relop;
    ValueNode* operand2;
};
