struct stmt_node// node for every statment
{
    int statement_type;
    int LHS;
    int operator_type;
    int op1;
    int op2; 
    stmt_node* next;
};

struct linkedList{//start of the statements
    stmt_node* start;
};
