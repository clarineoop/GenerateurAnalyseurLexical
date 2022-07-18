#ifndef UTILS_H
#define UTILS_H
#include "ast.h"
#include "fa.h"

struct stack
{
    int size;
    int top;
    enum {stack_of_char, stack_of_ast} type;
    union {
        char *array_char;
        ast **array_ast;
    } content;
    
};

char *getRegExpFromFile(char *fileName);
struct stack *push_char(char item, struct stack *pile);
struct stack *push_ast(ast *item, struct stack *pile);
char pop_char(struct stack *pile);
ast *pop_ast(struct stack *pile);
int isUnaryOperator(char symbol);
int isBinaryOperator(char symbol);
int precedence(char symbol);
char *infixToPostfix(char *infix_exp);
void print_ast_node(ast *t, FILE *stream);
void make_ast_graph(ast *tree);
char* concat(char *s1, char *s2);
void make_nfa_graph(nfa *nfautomata, char *graphName);
void make_dfa_graph(dfa *dfautomata, char *graphName);
unsigned concatenate(unsigned x, unsigned y);

#endif