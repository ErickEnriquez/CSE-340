#include "defn.h"
#include "lexer.h"
#include <iostream>
#include <string>

using namespace std;

LexicalAnalyzer lexer;
Scope* current_scope = new Scope();//head of our scopes
vector<Token> names;//global variable which is used to hold names for var_declaration
decl_error declaration_error;//will hold a declaration error if needed
type_mismatch Type_mismatch;//will hold a type mismatch error if needed
vector<uninitialized> uninit; //will hold our variables that have been uninitialized
int line_number;//holds the line number of a assignment or a condition , just in case we need it to check
vector<Variables> final_contents;//will ouput the final contents of a program if everything no_errors occur


void program::parse_program(){
    Token t = lexer.GetToken();
    if(t.token_type == LBRACE){
    lexer.UngetToken(t);
    current_scope->parent = nullptr;
    current_scope = nullptr;
    declaration_error.found = false;//make sure we intitialize this 
    Type_mismatch.found = false;
    parse_scope();
    t =lexer.GetToken();
    if(t.token_type != END_OF_FILE)
        syntax_error();
    }
    else
        syntax_error();
    
}

void program::parse_scope(){
    Token t = lexer.GetToken();//should be a LBRACE and we consume here
    if(t.token_type != LBRACE)
        syntax_error();
    t= lexer.GetToken();
    lexer.UngetToken(t);
    if(t.token_type == RBRACE){//here we check and make sure that we are not immedietly accepting a right brace following a left brace as is is not part of the language
        syntax_error();
    }
    Scope * new_scope = new Scope();//create a new scope
    new_scope->parent = current_scope;//temp's parent be the scope 
    current_scope  = new_scope;
    parse_scope_list();
     t = lexer.GetToken();
    if(t.token_type != RBRACE)
        syntax_error();
    Scope* temp = current_scope;//after we finish with a scope we can remove it 
    current_scope = current_scope->parent;
    if(variables_been_referenced(temp) != "nullptr"){
        declaration_error.symbol_name = variables_been_referenced(temp);
        declaration_error.error_code = "1.3";
        declaration_error.found = true;
    }
    delete temp;
}
//

void program::parse_scope_list(){
    Token t = lexer.GetToken();
    if(t.token_type == RBRACE || t.token_type == END_OF_FILE){
        lexer.UngetToken(t);
        return;
    }
    else if(t.token_type == LBRACE){
        lexer.UngetToken(t);
        parse_scope();
        parse_scope_list();//parse_scope_list recursively
    }
    else if(t.token_type == WHILE){
        lexer.UngetToken(t);
        parse_stmt();
        parse_scope_list();
    }
    else if(t.token_type == ID){
        Token t2  = lexer.GetToken();
        if(t2.token_type == EQUAL){
            lexer.UngetToken(t2);
            lexer.UngetToken(t);
            parse_stmt();
            parse_scope_list();
        }
        else if(t2.token_type == COMMA || t2.token_type == COLON){
            lexer.UngetToken(t2);
            lexer.UngetToken(t);
            parse_var_decl();
            parse_scope_list();
        }
        else
            syntax_error();
    }
    else
        syntax_error();
}

//parse var decl should have the rule var_decl -> id_list COLON type_name SEMICOLON
void program::parse_var_decl(){
    Token t = lexer.GetToken();//needs to be an ID else syntax error
    if(t.token_type != ID)
        syntax_error();
    else{
        lexer.UngetToken(t);//return the token
        parse_id_list();
        t = lexer.GetToken();//needs to be COLON and we consume here
        if(t.token_type != COLON)
            syntax_error();
         string type_name =  parse_type_name();
        t = lexer.GetToken();//get a token here, make sure it's SEMICOLON AND CONSUME HERE
        if(t.token_type != SEMICOLON)
            syntax_error();
        auto iterator = names.begin();//make an iterator to the begining of the names vector
        while(iterator != names.end()){//check the variables
            if(is_in_scope(iterator->lexeme) == true){//this is a 1.1 Error, variable declared more than once
                declaration_error.error_code = "1.1";//mark the declaration error as 1.1
                declaration_error.symbol_name = iterator->lexeme;//store the name of the offending variable
                declaration_error.found  = true;//mark the error as found
            }
            else
                insert_into_scope(iterator, type_name);//insert the variable into the symbol table
            iterator++; 

        }
        names.erase(names.begin(),names.end());//empty the vector for future use
    }
}

//rule is id_list -> ID  | id_list -> ID COMMA id_list 
void program::parse_id_list(){
    Token t1 =lexer.GetToken();//NEEDS TO BE ID CONSUME
    Token t2 = lexer.GetToken();// Can be a COLON or a COMMA
    if(t1.token_type == ID){
        names.push_back(t1);
        if(t2.token_type == COLON){//Follow 
            lexer.UngetToken(t2);
            return ;
        }
        else if(t2.token_type == COMMA){
            parse_id_list();
        }
        else
            syntax_error();
    }
    else
        syntax_error();
    

}
//rule is type_name -> REAL  | INT | BOOLEAN | STRING
string program::parse_type_name(){
    Token t = lexer.GetToken();//needs to be REAL INT BOOLEAN or STRING
    if(t.token_type != REAL && t.token_type != INT && t.token_type != BOOLEAN && t.token_type != STRING)
        syntax_error();
    else{
        return t.lexeme;//return the type of the variable
    }
}

// rule is stmt_list -> stmt | stmt stmt_list
void program::parse_stmt_list(){
    Token t = lexer.GetToken();
    if(t.token_type == RBRACE){//return to while statement 
        lexer.UngetToken(t);
        return;
    }
    else if(t.token_type == ID || t.token_type == WHILE){
        lexer.UngetToken(t);//unget the token
        parse_stmt();//parse the statement
        parse_stmt_list();//now parse the statement list return if you get a RBRACE 
    }
    else
        syntax_error();
}

//rule is stmt -> while_stmt | assign_stmnt
void program::parse_stmt(){
    Token t = lexer.GetToken();//needs to be a WHILE OR AN ID do not consume
    if(t.token_type == WHILE){
        lexer.UngetToken(t);
        parse_while_stmt();
    }
    else if(t.token_type == ID){
        lexer.UngetToken(t);
        parse_assign_stmt();
    }
    else
        syntax_error();
}

//rule is assign_stmt -> ID EQUAL expr SEMICOLON
void program::parse_assign_stmt(){
    Token t1 = lexer.GetToken();//needs to be a ID consume
    Token t2 = lexer.GetToken();// needs to be an EQUAL consume
    line_number = t1.line_no;//store the line number in case it's not valid
    Variables temporary ; temporary.Name = t1.lexeme; temporary.line_referenced = line_number; temporary.line_declared = get_line_declared(current_scope,t1);
    final_contents.push_back(temporary);//push the temporary variabe onto the vector
    if(t1.token_type != ID || t2.token_type != EQUAL)
        syntax_error();
    else{    
        //first check if a variable had been declared then we can mark the variable as referenced
    if(is_var_declared(t1) == true){
        mark_as_referenced(t1);//mark the variable as referenced
    }
    else {//the variable has not been declared so we should store the info in the uninitialized area
        mark_as_referenced(t1);
       /* uninitialized temp ;
        temp.found = true;
        temp.symbol_name = t1.lexeme;
        temp.line_referenced = t1.line_no;
        uninit.push_back(temp);*/
    }
   int type = parse_expr();
   check_LHS(current_scope , t1 , type );//check C1 And C2 Errors Here
    t1 = lexer.GetToken();//needs to be SEMICOLON , consume
    if(t1.token_type != SEMICOLON)
        syntax_error();
    }
}

//rule is while_stmt -> WHILE Condition LBRACE stmt_list RBRACE  | WHILE condition stmt
void program::parse_while_stmt(){
    Token t1 = lexer.GetToken();//needs to be WHILE  , Consume
    if(t1.token_type != WHILE)
        syntax_error();
    else{
        parse_condition();
        t1 = lexer.GetToken();
        if(t1.token_type == LBRACE){//if it is a LBRACE CONSUME
            parse_stmt_list();
            t1 = lexer.GetToken();//should be a RBRACE
            if(t1.token_type != RBRACE)
                syntax_error();
        }
       else if(t1.token_type == ID || t1.token_type  == WHILE){
           lexer.UngetToken(t1);
           parse_stmt();
       }
       else
        syntax_error();
    }
}

// returns a type of expression , is recursive
int program::parse_expr(){
    Token t = lexer.GetToken();
    if(t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV ||
    t.token_type == AND || t.token_type == OR || t.token_type == XOR || 
    t.token_type == GREATER || t.token_type == GTEQ || t.token_type == LESS || t.token_type == NOTEQUAL || t.token_type == LTEQ
    ){
        lexer.UngetToken(t);
        int op_code = parse_operator();
        int operand1 = parse_expr();
        int operand2 = parse_expr();
        return type_check(op_code,operand1 , operand2);
    }
    else if(t.token_type == NOT){
        lexer.UngetToken(t);
        int op_code = parse_operator();
        int operand = parse_expr();
        return type_check(op_code, operand);
    }
    
    else if(t.token_type == ID || t.token_type == NUM || t.token_type == REALNUM || t.token_type == STRING_CONSTANT || t.token_type == TRUE || t.token_type == FALSE){
        lexer.UngetToken(t);
        if(t.token_type == ID){
            mark_as_referenced(t);
            Variables x ; x.Name = t.lexeme; x.line_referenced = line_number ; x.line_declared = get_line_declared(current_scope,t);
            final_contents.push_back(x);
        }
        return parse_primary();
    }
    else
        syntax_error();
}

//this funtion returns the 
int program::parse_operator(){
    Token t = lexer.GetToken();
    if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV  || // arithmetic
     t.token_type == AND || t.token_type == OR || t.token_type == XOR || // Binary boolean
     t.token_type == GREATER || t.token_type == GTEQ || t.token_type == LESS || t.token_type == NOTEQUAL || t.token_type == LTEQ || // relational
     t.token_type == NOT
     )
     {
         switch (t.token_type){
             case PLUS: return PLUS; break;
             case MINUS : return MINUS ; break;
             case MULT : return MULT ;break;
             case DIV : return DIV ; break;
             case AND : return AND ; break;
             case OR : return OR; break; 
             case XOR : return XOR ; break; 
             case GREATER :return GREATER ; break;
             case GTEQ : return GTEQ ;break; 
             case LESS : return LESS ; break;
             case NOTEQUAL : return NOTEQUAL ;break; 
             case LTEQ : return LTEQ ;break;
             case NOT : return NOT ;break;
         }
     }
     else{
         syntax_error();
     }
}

//rule is primary -> ID | NUM | REALNUM | STRING_CONSTANT | bool_constant
int program::parse_primary(){
    Token t = lexer.GetToken();
    if(t.token_type == ID || t.token_type == NUM || t.token_type == REALNUM || t.token_type == STRING_CONSTANT || t.token_type == TRUE || t.token_type == FALSE){
        switch(t.token_type){
            case TRUE: return BOOLEAN;
            case FALSE: return BOOLEAN;
            case NUM: return INT;
            case REALNUM : return REAL;
            case STRING_CONSTANT: return STRING;
            case ID : {
                if(is_var_declared(t) == false){
                    declaration_error.error_code = "1.2";
                    declaration_error.symbol_name = t.lexeme;
                    declaration_error.found = true;
                }
                return check_ID_type(current_scope , t);
            }
        }
        
    }
    else
        syntax_error();
        
    
}

void program::parse_condition(){
    Token t = lexer.GetToken();//should be a LPAREN and we consume
    if(t.token_type != LPAREN)
        syntax_error();
    line_number = t.line_no;//store the line number
    int operand = parse_expr();
    if(operand != BOOLEAN && Type_mismatch.found == false){//c7 violation
        Type_mismatch.constraint = "C7";
        Type_mismatch.found = true;
        Type_mismatch.line_no = line_number;//placeholder
    }
    t =lexer.GetToken();//should be a RPAREN and we consume
    if(t.token_type != RPAREN)
        syntax_error();
}

void program::syntax_error(){
    std::cout<<"Syntax Error";
    exit(1);
}

//this funtion is going to go through our scope and see if it is already included in there
bool program::is_in_scope(std::string variable_name){
    for(Variables v : current_scope->variables){
        if(v.Name == variable_name)
            return true;
    }
    return false;
}

//this function will just go through and insert out token into our current scope's symbol table
void program::insert_into_scope(std::vector<Token>::iterator t , string type){
    Variables temp ;//make a temporary variables variable
    temp.Name = t->lexeme;//store the name of the variable
    temp.Type = type;//store the type of variable it is
    temp.line_declared = t->line_no;// note where on what line it was declared
    temp.refernced = false;//mark the variable as not_referenced
    current_scope->variables.push_back(temp);//push the variable onto the vector
}

//function wil print out whatever various errors we get
void program::print_errors(decl_error d_error , type_mismatch t_error , std::vector<uninitialized> u_error){
    if(d_error.found == true){//if we found a declaration error
        cout << "ERROR CODE " << d_error.error_code << " " << d_error.symbol_name;
        exit(1);
    }
    else if(t_error.found == true){
        cout<< "TYPE MISMATCH " << t_error.line_no << " " << t_error.constraint;
        exit(1);
    }
    else if(u_error.empty() == false){//check if the u_error is empty and if it's not then print the uninitialized errors
        auto iterator =u_error.begin();
        while(iterator != u_error.end()){
            cout<<"UNINITIALIZED " << iterator->symbol_name << " " << iterator->line_referenced << endl;
            iterator++;
        }
        exit(1);
    }
    else  
        return;
}

//checks to see if a variable in a  RHS of a statement has been declared or not
bool program::is_var_declared(Token t){
    Scope * temp = current_scope;//make a temp scope to check our scopes
    while(temp != nullptr){//while there are more scopes to check
    auto it = temp->variables.begin();
    while(it != temp->variables.end()){
        if(it->Name == t.lexeme)
            return true;
        it++;
        }
    temp = temp->parent;
    }
    return false;
}

//the method goes through and checks to see if a 
bool program::is_var_referenced(Token t){
    Scope* temp = current_scope;
    while(temp != nullptr){
        auto it = current_scope->variables.begin();
        while(it != current_scope->variables.end()){
            if(it->Name == t.lexeme)
                return true;
            it++;
        }
        temp = temp->parent;
    }
}

//checks our current scope and makes sure that all of the variables have been marked
string program::variables_been_referenced(Scope * s){
    for(Variables v : s->variables){
        if(v.refernced == false){
            return v.Name;
        }
    }
    return "nullptr";
}

//this function will go through and mark a token as referenced 
void program::mark_as_referenced(Token t){
    Scope* temp = current_scope;
    while(temp != nullptr){
        auto it = temp->variables.begin();
        while(it != temp->variables.end()){
            if(it->Name == t.lexeme){
                it->refernced = true;
                it->line_referenced = t.line_no;
                return;
            }
            it++;
            
        }
        temp = temp->parent;
    }
}

//checks the type of a variable
int program::check_ID_type(Scope* current , Token t){
    Scope* temp = current;
        while(temp != nullptr ){
       auto iterator = temp->variables.begin();//iterator to the begining of the variables
    while(iterator != temp->variables.end()){
        if(iterator->Name == t.lexeme){
            if(iterator->Type == "REAL")
                return REAL;
            else if(iterator->Type == "INT")
                return INT;
            else if(iterator->Type == "BOOLEAN")
                return BOOLEAN;
            else if(iterator->Type == "STRING")
                return STRING;
            else 
                return -1;//just in case 
        }
        iterator++;
    }
    temp = temp->parent;
        } 
}


//this is only to be used with the not command
int program::type_check(int op_code , int operand){
    if(operand == -1)
        return -1;
    
    else if(op_code == NOT && operand == BOOLEAN){
        return BOOLEAN;//this is a valid expression
    }
    else{
        Type_mismatch.found = true;
        Type_mismatch.constraint = "C8";
        //still need to write donw the line this occurs
        Type_mismatch.line_no = line_number;
        return -1;
    }
}

//resolves the type given 2 types and an operator
int program::type_check(int op_code , int type1 , int type2){
    if(op_code == PLUS || op_code == MINUS || op_code == MULT || op_code == DIV){
        return resolve_arithmetic(op_code , type1 , type2);
    }
    else if(op_code == AND || op_code == OR || op_code == XOR){
        return resolve_binary_bool(op_code , type1 , type2);
    }
    else if(op_code == GREATER || op_code == GTEQ || op_code == LESS || op_code == NOTEQUAL || op_code == LTEQ){
        return resolve_relational(op_code , type1, type2);
    }
    else
    {
        return -1;
    }
}

//this resolves arithmetic types
int program::resolve_arithmetic(int op_code , int type1 ,int type2){
    if(type1 == -1 || type2 == -1)//not valid
        return -1;
    else{
        if(op_code == PLUS){
            if(type1 == INT && type2 == INT)
                return INT;
            else if(type1 == INT && type2 == REAL)
                return REAL;
            else if(type1 == REAL && type2 == INT)
                return REAL;
            else if(type1 == REAL && type2 == REAL)
                return REAL;
            else
            {//c3 violation 
                Type_mismatch.found = true;
                Type_mismatch.constraint = "C3";
                Type_mismatch.line_no = line_number;//placeholder for now
                return -1;
            }
        }
        else if(op_code == MINUS){
             if(type1 == INT && type2 == INT)
                return INT;
            else if(type1 == INT && type2 == REAL)
                return REAL;
            else if(type1 == REAL && type2 == INT)
                return REAL;
            else if(type1 == REAL && type2 == REAL)
                return REAL;
            else
             {//c3 violation 
                Type_mismatch.found = true;
                Type_mismatch.constraint = "C3";
                Type_mismatch.line_no = line_number;//placeholder for now
                return -1;
            }           
        }
        else if(op_code == DIV){
            if(type1 == INT && type2 == INT)
                return REAL;
            else if(type1 == INT && type2 == REAL)
                return REAL;
            else if(type1 == REAL && type2 == INT)
                return REAL;
            else if(type1 == REAL && type2 == REAL)
                return REAL;
            else
             {//c3 violation 
                Type_mismatch.found = true;
                Type_mismatch.constraint = "C3";
                Type_mismatch.line_no = line_number;//placeholder for now
                return -1;
            }
        }
        else if(op_code  == MULT){
            if(type1 == INT && type2 == INT)
                return INT;
            else if(type1 == INT && type2 == REAL)
                return REAL;
            else if(type1 == REAL && type2 == INT)
                return REAL;
            else if(type1 == REAL && type2 == REAL)
                return REAL;
            else
             {//c3 violation 
                Type_mismatch.found = true;
                Type_mismatch.constraint = "C3";
                Type_mismatch.line_no = line_number;//placeholder for now
                return -1;
            }
        }
    }
}

//needs more work
int program::resolve_relational(int op_code , int type1 , int type2){
    if(type1 == -1 || type2 == -1)
        return -1;
    else{

        if(type1 == INT || type1 == REAL || type2 == REAL || type2 == INT){//one of the operands has the correct type 
             if(type1 == INT && type2 == INT)
                return BOOLEAN;
            else if(type1 == INT && type2 == REAL)
                return BOOLEAN;
            else if(type1 == REAL && type2 == INT)
                return BOOLEAN;
            else if(type2 == REAL && type2 == REAL)
                return BOOLEAN; 
            else{//C6 Violation
                Type_mismatch.constraint = "C6";
                Type_mismatch.found = true;
                Type_mismatch.line_no = line_number;
            }
        }
        else  if (type1 != INT && type1 != REAL && type2 != INT && type2 != REAL){//if non_of them is INT OR REAL
            if(type1 == STRING && type2 == STRING)
                return BOOLEAN;
            else if(type1 == BOOLEAN && type2 == BOOLEAN)
                return BOOLEAN;
            else{//C5 violation
                Type_mismatch.constraint = "C5";
                Type_mismatch.found = true;
                Type_mismatch.line_no = line_number;//placeholder
                return -1;
            }
        }
    
    }
}

int program::resolve_binary_bool(int op_code , int type1 , int type2){
    if(type1 == -1 || type2 == -1)  //not valid expression
        return -1;
    else if(type1 == BOOLEAN && type2 == BOOLEAN)
        return BOOLEAN;
    else {//c4 violation
        Type_mismatch.found = true;
        Type_mismatch.constraint = "C4";
        Type_mismatch.line_no = line_number;//placeholder
    }
}


void program::check_LHS(Scope* current , Token t , int type){
    if(Type_mismatch.found == false){//only check the LHS IF THE LHS DIDN'T ALREADY CREATE AND ERROR
        int lhs = check_ID_type(current_scope,t);
        if(lhs == INT || lhs == STRING || lhs == BOOLEAN){
            if(lhs != type){//INT -> INT , STRING -> STRING  , BOOLEAN -> BOOLEAN
                //c1 violation
                Type_mismatch.found = true;
                Type_mismatch.constraint = "C1";
                Type_mismatch.line_no = line_number;//placeholder
                return;
            }
        }
        else if(lhs == REAL){
            if(lhs == REAL && type == REAL){
                return;
            }
            else if(lhs == REAL && type == INT)
                return;
            else {// REAL -> REAL | REAL -> INT
                //C2 Violation
                Type_mismatch.found = true;
                Type_mismatch.constraint = "C2";
                Type_mismatch.line_no = line_number;
            }
        }
    }
    else{
        return ;
    }

}

//this function is just going to go and print the variables at the end if there is no error
void program::print_no_error_vars(){
    auto iterator = final_contents.begin();
    while(iterator != final_contents.end()){
        cout << iterator->Name << " " << iterator->line_referenced << " " << iterator->line_declared<<"\n";
        iterator++;
    }
}

//this function returns the line that a variable was declared at
int program::get_line_declared(Scope* current , Token t){
    Scope* temp = current;
    while(temp != nullptr){
    auto iterator = temp->variables.begin();
    while(iterator != temp->variables.end()){
        if(iterator->Name == t.lexeme){
            return iterator->line_declared;
        }
        iterator++;
        }
        temp = temp->parent;
    }
}

int program::get_line_referenced(Scope* current , Token t){
    auto iterator = current->variables.begin();
    while(iterator != current->variables.end());
        if(iterator->Name == t.lexeme)
            return iterator->line_referenced;
        iterator++;
}





int main(){
    program p;
    p.parse_program();
    p.print_errors(declaration_error , Type_mismatch , uninit);//will go throught and print any of the errors we may need
    p.print_no_error_vars();
}

