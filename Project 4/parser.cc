#include "parser.h"
#include "compiler.h"
#include <iostream>

LexicalAnalyzer lexer;

using namespace std;

//use to peek at the next token without consuming it 
TokenType Parser::peek(){
    Token t = lexer.GetToken();
    return lexer.UngetToken(t);
}

//used to expect the next token if it is not of the right type then it will give a syntax error else it will return the token
Token Parser::expect(TokenType type){
    Token t = lexer.GetToken();
    if(t.token_type != type)
        syntax_error();
    else
        return t;
}

void Parser::syntax_error(){
    cout<<"SYNTAX ERROR";
    exit(1);
}

 struct StatementNode* parse_generate_intermediate_representation(){
    Parser par;
    return par.parse_program();
}


StatementNode* Parser::parse_program(){
    list = NULL;
    mem = NULL;
    if(peek() != ID){
        syntax_error();
    }
    else{
        parse_var_section();
        list = parse_body();
    }
    return list;
}

//rule is var_section -> id_list SEMICOLON
void Parser::parse_var_section(){
    if(peek() != ID)
        syntax_error();
    else{
        parse_id_list();
        Token t = expect(SEMICOLON);
    }
}
//rule is id_list -> ID COMMA id_list  | ID
void Parser::parse_id_list(){
    Token t = expect(ID);
    insert_into_memory(t);//inser the Token as a variable in memory
    if(peek() == COMMA){
        t = expect(COMMA);
        parse_id_list();//recursively call parse_id_list
    }
    else if(peek() == SEMICOLON)//follow of id_list
        return;//return from recursion
    else
        syntax_error();
}

//rule is body -> LBRACE stmt_list RBRACE
StatementNode* Parser::parse_body(){
    Token t = expect(LBRACE);
    StatementNode* temp =  parse_stmt_list();
    t = expect(RBRACE);
    return temp;
}

StatementNode* Parser::parse_stmt_list(){

    StatementNode* st;
    StatementNode* stmtlist;
    st = parse_stmt();
    if(peek() == ID || peek() == PRINT || peek() == WHILE || peek() == IF || peek() == SWITCH || peek() == FOR){
    stmtlist = parse_stmt_list();
    //append stl to st;
    st = append_statement(stmtlist , st);
    return st;
    }   
    else{
    return st;
    }
}
StatementNode* Parser::parse_stmt(){
    StatementNode* temp;
     if(peek() == ID)
     temp =  parse_assign_stmt();
    else if(peek() == PRINT)
     temp =  parse_print_stmt();
    else if (peek() == WHILE)
     temp =  parse_while_stmt();
    else if(peek() == IF)
     temp = parse_if_stmt();
    else if(peek() == SWITCH){
     temp =   parse_switch_stmt();
     return temp;//might be wrong need to check here
    }
    else if(peek() == FOR){
      temp = parse_for_stmt();
      temp->next->next = temp->next->if_stmt->false_branch;//assign the 2nd node in the link to point to the false branch
      return temp;
    }
    else
        syntax_error();
    if(temp->type == IF_STMT){
        temp->next = temp->if_stmt->false_branch;
        return temp;
    }
    temp->next = NULL;
    return temp;//return our statment node
}
StatementNode* Parser::parse_assign_stmt(){
    StatementNode* temp = new StatementNode();//allocate a new pionter
    temp->assign_stmt = new AssignmentStatement();//allocate the assign statment too
    temp->type = ASSIGN_STMT;//mark what type of statment we have
    Token t  = expect(ID);//consume ID
    temp->assign_stmt->left_hand_side = check_memory(t);//check memory and assign the location to the LHS
    t = expect(EQUAL);//Consume Equal
    //now here we need to do 2 look aheads to see if we will be doing expr or primary
    t = lexer.GetToken();//NEEDS TO BE ID , DO NOT CONSUME
    Token t2 = lexer.GetToken();//CAN BE 
    if(t.token_type == ID || t.token_type == NUM){
        if(t2.token_type == SEMICOLON){//parse_primary follow will be SEMICOLON
            lexer.UngetToken(t2);
            lexer.UngetToken(t);
            temp->assign_stmt->op = OPERATOR_NONE;
           temp->assign_stmt->operand1 = parse_primary();
           temp->assign_stmt->operand2 = NULL;//set the operand 2 to be null as a safety measure
        }
        else if(t2.token_type == PLUS || t2.token_type == MINUS || t2.token_type == MULT || t2.token_type == DIV  ){
            temp->assign_stmt->op = getOperator(t2);//get the operator
            lexer.UngetToken(t2);
            lexer.UngetToken(t);
           ValueNode** array =  parse_expr();//return the addresses of the operands with a double pointer
           temp->assign_stmt->operand1 = array[0];
           temp->assign_stmt->operand2 = array[1];
        }
        t = expect(SEMICOLON);
        return temp;
    }
    else
        syntax_error();

}


ValueNode** Parser::parse_expr(){
    ValueNode** array = new ValueNode*[2];
    array[0] = parse_primary();
    parse_op();
    array[1] = parse_primary();
    
    return array ;
}

ValueNode* Parser::parse_primary(){
    Token t;
    if(peek() == ID){
         t = expect(ID);
        return check_memory(t);
    }
    else if(peek() == NUM){
         t = expect(NUM);
         insert_into_memory(t);//insert into memory
         ValueNode* temp = check_memory(t);//retrieve the location
         temp->value = stoi(t.lexeme);//this should be a constant so we can just convert the string to its integer value
         return temp;
    }
    else
        syntax_error(); 
}

void Parser::parse_op(){
    Token t = lexer.GetToken();
    if(t.token_type != PLUS && t.token_type != MINUS && t.token_type != MULT && t.token_type != DIV )
        syntax_error();
}

StatementNode* Parser::parse_print_stmt(){
    StatementNode* temp = new StatementNode();
    temp->type = PRINT_STMT;
    temp->print_stmt = new PrintStatement();
    Token t = expect(PRINT);
    t = expect(ID);
    temp->print_stmt->id  = check_memory(t);
    t = expect(SEMICOLON);
    return temp;
}

StatementNode* Parser::parse_while_stmt(){
    Token t  = expect(WHILE);
   StatementNode* temp = new StatementNode();//allocate a new statement pointer
   temp->type = IF_STMT;//make this a new 
   temp->if_stmt = new IfStatement();//allocate the pointer
   struct condition c = parse_condition();
    temp->if_stmt->condition_op = c.relop;
    temp->if_stmt->condition_operand1 = c.operand1;
    temp->if_stmt->condition_operand2 = c.operand2;
    temp->if_stmt->true_branch = parse_body();
    StatementNode* goto_node = new StatementNode();
    goto_node->type = GOTO_STMT;
    goto_node->goto_stmt = new GotoStatement();//make a new goto node
    goto_node->goto_stmt->target = temp;//set the goto to go back to the if_statement again
    append_statement(goto_node,temp->if_stmt->true_branch);//append
    StatementNode* no_op_node = new StatementNode();//create a new statement node
    no_op_node->type = NOOP_STMT;
    temp->if_stmt->false_branch = no_op_node;
    return temp;
    
}

StatementNode* Parser::parse_if_stmt(){
    StatementNode* temp = new StatementNode();//allocate the statment node
    temp->if_stmt = new IfStatement();//allocate the if_statment
    temp->type = IF_STMT;
    Token t = expect(IF);
   struct condition cond = parse_condition();
   temp->if_stmt->condition_op = cond.relop;
   temp->if_stmt->condition_operand1 = cond.operand1;
   temp->if_stmt->condition_operand2 = cond.operand2;
   temp->if_stmt->true_branch = parse_body();//set the true branch to the body
   StatementNode* no_op_node = new StatementNode();
   no_op_node->type = NOOP_STMT;
   append_statement(no_op_node,temp->if_stmt->true_branch);//append 
   temp->if_stmt->false_branch = no_op_node;//make no_op point to the false branch
   return temp;
}

condition Parser::parse_condition(){
    struct condition temp;//going to hold our data
    temp.operand1 = parse_primary();
    temp.relop = parse_relop();
    temp.operand2 = parse_primary();
    return temp;    
}

ConditionalOperatorType Parser::parse_relop(){
    Token t = lexer.GetToken();
    if(t.token_type != GREATER && t.token_type != LESS && t.token_type != NOTEQUAL)
        syntax_error();
    return getRelop(t);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StatementNode* Parser::parse_switch_stmt(){
    StatementNode* goto_node = new StatementNode();
    StatementNode* no_op_node = new StatementNode();

    no_op_node->type =NOOP_STMT;
    goto_node->goto_stmt = new GotoStatement();
    goto_node->type = GOTO_STMT;
    goto_node->goto_stmt->target = no_op_node;

    Token t = expect(SWITCH);
    t = expect(ID);
    Token ID = t;
    t = expect(LBRACE);
    StatementNode* temp = parse_case(ID,goto_node);//linked list of if_statements
    temp->next = no_op_node;
    t  = expect(RBRACE);
    return temp;

}

StatementNode* Parser::parse_for_stmt(){
    StatementNode* initialize_node = new StatementNode();
    initialize_node->type  = ASSIGN_STMT;
    initialize_node->assign_stmt = new AssignmentStatement();
    StatementNode* temp = new StatementNode();
    temp->type = IF_STMT;
    temp->if_stmt = new IfStatement();
    StatementNode* increment_node = new StatementNode();
    increment_node->type = ASSIGN_STMT;
    increment_node->assign_stmt = new AssignmentStatement();
        Token t = expect(FOR);
        t = expect(LPAREN);
       initialize_node = parse_assign_stmt();
        struct condition c =  parse_condition();
        temp->if_stmt->condition_op = c.relop;
        temp->if_stmt->condition_operand1 = c.operand1;
        temp->if_stmt->condition_operand2 = c.operand2;
        t = expect(SEMICOLON);
        increment_node = parse_assign_stmt();
        t = expect(RPAREN);
        temp->if_stmt->true_branch = parse_body();
        append_statement(increment_node,temp->if_stmt->true_branch);//append the increment to the end
        StatementNode* goto_node = new StatementNode();
        goto_node->goto_stmt = new GotoStatement();
        goto_node->goto_stmt->target = temp;
        goto_node->type = GOTO_STMT;
        append_statement(goto_node,temp->if_stmt->true_branch);
        StatementNode* no_op_node = new StatementNode();
        no_op_node->type = NOOP_STMT;
        temp->if_stmt->false_branch = no_op_node;
    initialize_node->next = temp;//link the initialization node with temp
    return initialize_node;
}



StatementNode* Parser::parse_case(Token tok ,StatementNode* list){
    StatementNode* temp = new StatementNode();
    temp->type = IF_STMT;
    temp->if_stmt = new IfStatement();
    temp->if_stmt->condition_op = CONDITION_NOTEQUAL;
    temp->if_stmt->condition_operand1 = check_memory(tok);//get the original token
    Token t = lexer.GetToken();//CASE OR DEFAULT
    switch (t.token_type)
    {
    case CASE:
        t = lexer.GetToken();//get the number
        insert_into_memory(t);
        temp->if_stmt->condition_operand2 = check_memory(t);
        break;
    case DEFAULT:
        temp->if_stmt->condition_operand2 = check_memory(tok);
        
    }
    t = expect(COLON);
    temp->if_stmt->false_branch = parse_body();//has to be with false branch as we do not have an equal
    append_statement(list,temp->if_stmt->false_branch);//append if stmt to the end of our node's false branch 
    if(peek() == DEFAULT || peek() == CASE){
        temp->if_stmt->true_branch = parse_case(tok,list);
    }
    else{
        temp->if_stmt->true_branch = list;
    }
    return temp;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ArithmeticOperatorType Parser::getOperator(Token t){
    switch(t.token_type){
        case PLUS: return OPERATOR_PLUS ; break;
        case MINUS: return OPERATOR_MINUS ; break;
        case MULT: return OPERATOR_MULT; break;
        case DIV: return OPERATOR_DIV; break;
    }//
}

ConditionalOperatorType Parser::getRelop(Token t){
    switch(t.token_type){
        case GREATER: return CONDITION_GREATER;break;
        case LESS: return CONDITION_LESS;break;
        case NOTEQUAL: return CONDITION_NOTEQUAL; break;
    }
}


StatementNode* Parser::append_statement(StatementNode* new_statement , StatementNode* stlist){
    StatementNode* temp = stlist;//get a temp to head of the list
    if(temp == NULL){
        temp  = new_statement;
        return stlist;
    }
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = new_statement;
    return stlist;

}

void Parser::insert_into_memory(Token t){
    if(check_memory(t) == NULL){
        ValueNode* new_node = new ValueNode();
        new_node->name = t.lexeme;
        new_node->value = 0;
        memory* temp = new memory();
        temp->data = new_node;
        temp->next = NULL;
        if(mem == NULL){
            mem =  temp;
            return;
        }
        temp->next = mem;
        mem = temp;
        return;
    }
}

ValueNode* Parser::check_memory(Token t){
    memory* head = mem;//point to head of data
    while(head != NULL){
        if(head->data->name == t.lexeme)
            return head->data;
        head = head->next;
    }
    return NULL;
}

//links the GOTO with the NO_OP at the end
StatementNode* Parser::link_goto(StatementNode* tail , StatementNode* stlist){
    if(stlist == NULL)//should never be the case but just being preemptive here
        return NULL;
    else{
        StatementNode* temp = stlist;
        while(temp != NULL){
            StatementNode* body = temp->if_stmt->false_branch;//start of our body 
            check_true_branch(temp->if_stmt->true_branch,tail);//link our true branch if needed
            while(body->next != NULL){
                body = body->next;
            }
            body->goto_stmt->target  = tail;//link the goto_node to the tail No_op
            temp= temp->if_stmt->true_branch->next;//has to be our true branch next because true branch originaly has a no_op
        }
        return stlist;
    }
}


StatementNode* Parser::check_true_branch(StatementNode* true_branch ,StatementNode* tail){
    if(true_branch->type == NOOP_STMT && true_branch != tail){
        true_branch = true_branch->next;//move this to the goto
        true_branch->goto_stmt->target = tail;
        return true_branch;
    }
}