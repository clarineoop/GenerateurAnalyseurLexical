#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ast.h"
#include "utils.h"

/* 
	constructeur pour un noeud de type symbol pour 
	simplifier la construction de l'arbre de syntaxe abstraite
*/
ast *makeSymbol(char c, int key)
{
	ast *t = (ast *) malloc(sizeof(ast));
	
	t->type = symbol;
	t->unique_key = key;
	t->symbol_or_operator = c;

	return t;
};


/* 
	constructeur pour un noeud de type unary_operation pour 
	simplifier la construction de l'arbre de syntaxe abstraite
*/
ast *makeUnaryOperation(char op, ast *operand, int key)
{
	ast *t = (ast *) malloc(sizeof(ast));
	
	t->type = unary_operation;
	t->unique_key = key;
	t->symbol_or_operator = op;
	t->operands.unaryOperation.operand = operand;

	return t;
};

/* 
	constructeur pour un noeud de type binary_operation pour 
	simplifier la construction de l'arbre de syntaxe abstraite
*/
ast *makeBinaryOperation(char op, ast *leftOperand, ast *rightOperand, int key)
{	
	ast *t = (ast *) malloc(sizeof(ast));
	
	t->type = binary_operation;
	t->unique_key = key;
	t->symbol_or_operator = op;
	t->operands.binaryOperation.leftOperand = leftOperand;
	t->operands.binaryOperation.rightOperand = rightOperand;

	return t;

};

/*
	fonction pour générer l'ast à partir de l'expression rationelle 
	sous forme postfix
*/
ast *buildAstFromPostfixExp(char *ePostfix)
{
	struct stack *pile = (struct stack *) malloc(sizeof(struct stack));
	pile->top = -1;
	pile->size = strlen(ePostfix);
	pile->type = stack_of_ast;
	ast *array[strlen(ePostfix)];
	pile->content.array_ast = array; // initialiser la pile

	int i = 0, n = 1;
	char item = ePostfix[i]; // initialiser avant la boucle

	while(item != '\0')
	{
		if (isUnaryOperator(item) == 1)
		{
            ast *operand = pop_ast(pile);
			ast *node = makeUnaryOperation(item, operand, n);
			n++;
            pile = push_ast(node, pile);
        }
		else if (isBinaryOperator(item) == 1)
		{
            ast *rightOperand = pop_ast(pile);
			ast *leftOperand = pop_ast(pile);
			ast *node = makeBinaryOperation(item, leftOperand, rightOperand, n);
			n++;
            pile = push_ast(node, pile);
        }
        else {
            ast *node = makeSymbol(item, n);
			n++;
            pile = push_ast(node, pile);
        }

		i++;
		item = ePostfix[i];
	}

	ast *t = pop_ast(pile);
	return t;
}