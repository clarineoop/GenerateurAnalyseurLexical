#ifndef AST_H
#define AST_H

/*
	définition de l'arbre de syntaxe abstraite en utilisant une struct
	ast: abstract syntaxic tree
*/
typedef struct ast_node ast;
struct ast_node 
{	
	enum {symbol, unary_operation, binary_operation} type;
	int unique_key;
	char symbol_or_operator;

	union 
	{
		struct {
	   		struct ast_node *operand;
	   	} unaryOperation;
        
		struct {
			struct ast_node *leftOperand;
	   		struct ast_node *rightOperand;
	   	} binaryOperation;

    } operands;

};

ast *makeSymbol(char c, int n);
ast *makeUnaryOperation(char op, ast *operand, int key);
ast *makeBinaryOperation(char op, ast *leftOperand, ast *rightOperand, int n);
ast *buildAstFromPostfixExp(char *ePostfix);

#endif