#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"


/*
	fonction pour extraire l'expression rationelle contenue 
	dans le fichier fileName.
*/
char *getRegExpFromFile(char *fileName)
{
	FILE *f = fopen(fileName, "r");

	if(f == NULL){
		printf("Erreur d'ouverture du fichier\n");
		exit(1);
	}

	int n = 0;
	char c;
	char *regExp;

	do {
		c = fgetc(f);
		if (c != '\n' && c != EOF) {
			if (n == 0) regExp = (char *) malloc((n + 1) * sizeof(char));
			else regExp = (char *) realloc (regExp, (n + 1) * sizeof(char));
			regExp[n] = c;
		}
		n = n + 1;
	} while (c != '\n' && c != EOF);

	regExp[n] = '\0'; // Ajout du caractere de cloture de la chaine de caractere
	fclose(f);
	return regExp;
}

/* définie l'opération push pour une pile de char*/
struct stack *push_char(char item, struct stack *pile)
{
	if(pile->top >= pile->size-1) 
	{
		printf("\nStack Overflow.");
		return pile;
	}
	else
	{
		pile->top = pile->top+1;
		pile->content.array_char[pile->top] = item;
		return pile;
	}
}

/* définie l'opération push pour une pile de ast*/
struct stack *push_ast(ast *item, struct stack *pile)
{
	if(pile->top >= pile->size-1) 
	{
		printf("\nStack Overflow.");
		return pile;
	}
	else
	{
		pile->top = pile->top+1;
		pile->content.array_ast[pile->top] = item;
		return pile;
	}
}

/* définie l'opération pop pour une pile de char*/
char pop_char(struct stack *pile)
{
	char item ;

	if(pile->top < 0)
	{
		printf("stack under flow: invalid infix expression");
		getchar();
		/* underflow peut survenir pour des expressions invalides*/
		/* dans lequelles ( et ) ne correspondent pas */
		exit(1);
	}
	else
	{
		item = pile->content.array_char[pile->top];
		pile->top = pile->top-1;
		return(item);
	}
}

/* définie l'opération pop pour une pile de ast*/
ast *pop_ast(struct stack *pile)
{
	ast *item ;

	if(pile->top < 0)
	{
		printf("stack under flow: invalid infix expression");
		getchar();
		/* underflow peut survenir pour des expressions invalides*/
		/* dans lequelles ( et ) ne correspondent pas */
		exit(1);
	}
	else
	{
		item = pile->content.array_ast[pile->top];
		pile->top = pile->top-1;
		return(item);
	}
}

/* 
*fonction pour determiner si un symbol est un opérateur unaire ou non
* la focntion retourne 1 si le symbol est un oprérateur unaire et 0 sinon
*/
int isUnaryOperator(char symbol)
{
	if(symbol == '*') return 1;
	else return 0;
}

/* 
*fonction pour determiner si un symbol est un opérateur binaire ou non
* la focntion retourne 1 si le symbol est un oprérateur binaire et 0 sinon
*/
int isBinaryOperator(char symbol)
{
	if(symbol == '.' || symbol == '|') return 1;
	else return 0;
}

/* definie une fonction utilisée pour assigner une priorité aux opérateurs.
* ici "." représente la concaténation.
* Dans cette fonction on suppose qu'un entier plus grand signifie 
  une plus grande priorité.
*/
int precedence(char symbol)
{
	if(symbol == '*') return(3); /* répétition de kleene, plus forte priorité*/
	else if(symbol == '.') return(2);
	else if(symbol == '|') return(1); /* disjonction plus faible priorité */
	else return(0);
}

/* Cette fonction convertie une expression de la forme 
   infix (a|b) à la forme postfix (ab|).
 * la forme postfix est facilement interprétable par l'ordinateur
 * on represente la concatenation par un ".".
 * on a donc 3 opérateurs: (*, ., |) 
*/
char *infixToPostfix(char *infix_exp)
{ 
	int i, j;
	char item, x;
	char *postfix_exp = (char *) malloc(strlen(infix_exp) * sizeof(char));

	struct stack *pile = (struct stack *) malloc(sizeof(struct stack));
	pile->top = -1;
	pile->size = strlen(infix_exp);
	pile->type = stack_of_char;
	char array[strlen(infix_exp)];
	pile->content.array_char = array; /* initialiser la pile */

	pile = push_char('(', pile); /* push '(' sur la pile */
	strcat(infix_exp, ")"); /* ajouter ')' à l'expression sous la forme infix */

	i = j = 0;
	item = infix_exp[i]; /* initialiser avant la boucle*/

	while(item != '\0')
	{
		if(item == '(') pile = push_char(item, pile);
		else if(isUnaryOperator(item) == 1 || isBinaryOperator(item) == 1) /* si le symbol courant est un operateur alors */
		{
			/* extraire de la pile tous les opérateur à plus forte priorité 
			   et les ajouter dans l'expresion de sortie (postfix) */
			x = pop_char(pile);
			while((isUnaryOperator(item) == 1 || isBinaryOperator(item) == 1) && precedence(x) >= precedence(item))
			{
				postfix_exp[j] = x;
				j++;
				x = pop_char(pile);
			}
			pile = push_char(x, pile); /* remettre le dernier élement extrait à 
									tort de la pile dans la boucle while */

			pile = push_char(item, pile); /* mettre l'oprerateur lut dans la pile */
		}
		else if(item == ')') /* si le symbol courant est ')' alors */
		{
			x = pop_char(pile);          		/* pop et contnuer */
			while(x != '(')                 /* jusquà renconter '(' */
			{
				postfix_exp[j] = x;
				j++;
				x = pop_char(pile);
			}
		}
		else /* c'est un opérande */
		{
			postfix_exp[j] = item;  /* ajouter l'opérande à l'expréssion postfix */
			j++;
		}

		/* lecture du symbol suivant en décidant s'il y a concatenation ou non*/
		if(infix_exp[i+1] == '|' || infix_exp[i+1] == '*' 
		|| infix_exp[i+1] == ')' || infix_exp[i+1] == '\0' 
		|| item == '|' || item == '(' || item == '.')
		{
			i++;
			item = infix_exp[i]; // on lit le caractère suivant
		}
		else item = '.'; // on suppose que le catactère suivant c'est la concatenation
		
	} /* la boucle while se termine ici */

	if(pile->top > 0)
	{
		printf("\nInvalid infix Expression.\n");
		getchar();
		free(pile);
		exit(1);
	}

	postfix_exp[j] = '\0'; /* ajout du caractere de cloture de la chaine de caractere */
	free(pile);
	return postfix_exp;
}

char *getOpName(char op)
{
	if(op == '.') return "concaténation";
	else if(op == '*') return "répétition de Kleene";
	else if(op == '|') return "disjonction";
	else return "";
}

void print_ast_node(ast *t, FILE *stream)
{
    switch ( t->type )
    {
        case symbol: 
            fprintf(stream, "\tr%d[label=<r%d<BR />%c>];\n", 
			t->unique_key, t->unique_key, t->symbol_or_operator);
            break;
        case unary_operation:
            fprintf(stream, "\tr%d[label=<r%d<BR />%c<BR /><FONT POINT-SIZE=\"10\">%s</FONT>>];\n", 
			t->unique_key, t->unique_key, t->symbol_or_operator, getOpName(t->symbol_or_operator));
			
			fprintf(stream, "\tr%d -- r%d;\n", t->unique_key, 
			t->operands.unaryOperation.operand->unique_key);

			print_ast_node(t->operands.unaryOperation.operand, stream);
            
            break;
        case  binary_operation:
			fprintf(stream, "\tr%d[label=<r%d<BR />%c<BR /><FONT POINT-SIZE=\"10\">%s</FONT>>];\n", 
			t->unique_key, t->unique_key, t->symbol_or_operator, getOpName(t->symbol_or_operator));
			
			fprintf(stream, "\tr%d -- r%d;\n", t->unique_key, 
			t->operands.binaryOperation.leftOperand->unique_key);

			fprintf(stream, "\tr%d -- r%d;\n", t->unique_key, 
			t->operands.binaryOperation.rightOperand->unique_key);

			print_ast_node(t->operands.binaryOperation.leftOperand, stream);
			print_ast_node(t->operands.binaryOperation.rightOperand, stream);
    }
}

void make_ast_graph(ast *tree)
{
	system("mkdir -p ../graphs"); // créer le dossier graphs s'il n'existe pas
	FILE *ast_dot = fopen("../graphs/ast.dot", "w");
	if(ast_dot == NULL){
		printf("Erreur d'ouverture du fichier\n");
		exit(1);
	}
	else
	{
		fprintf(ast_dot, "graph AST {\n");
		fprintf(ast_dot, "\tnode [fontname=\"Arial\"];\n");

		print_ast_node(tree, ast_dot);

		fprintf(ast_dot, "}\n");
		fclose(ast_dot);
		system("dot -Tpng ../graphs/ast.dot -o ../graphs/ast.png"); // générer l'image de l'arbre
	}
}

char* concat(char *s1, char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void make_nfa_graph(nfa *nfautomata, char *graphName)
{
	system("mkdir -p ../graphs"); // créer le dossier graphs s'il n'existe pas
	char *dirName = "../graphs/";
	char *ext1 = ".dot";
	char *dirFile = concat(dirName, graphName);
	char *outDotFileName = concat(dirFile, ext1);
	char *ext2 = ".png";
	char *outPngFileName = concat(dirFile, ext2);
	free(dirFile);

	FILE *nfa_dot = fopen(outDotFileName, "w");
	if(nfa_dot == NULL){
		printf("Erreur d'ouverture du fichier\n");
		exit(1);
	}
	else
	{
		fprintf(nfa_dot, "digraph %s {\n", graphName);
		fprintf(nfa_dot, "\tfontname=\"Helvetica,Arial,sans-serif\";\n");
		fprintf(nfa_dot, "\tnode [fontname=\"Helvetica,Arial,sans-serif\"];\n");
		fprintf(nfa_dot, "\tedge [fontname=\"Helvetica,Arial,sans-serif\"];\n");
		fprintf(nfa_dot, "\trankdir=LR;\n");
		fprintf(nfa_dot, "\tnode [shape = doublecircle]; q%d;\n", nfautomata->final_state.label);
		fprintf(nfa_dot, "\tnode [shape = point ]; qi\n");
		fprintf(nfa_dot, "\tnode [shape = circle];\n");
		fprintf(nfa_dot, "\tqi -> q%d;\n", nfautomata->initial_state.label);

		for(int i = 0; i<nfautomata->nEdges; i++)
		{
			edge edge = nfautomata->edges[i];
			if(edge.label == '\0')
			{
				fprintf(nfa_dot, "\tq%d -> q%d [ label = \"ε\" ];\n", 
				edge.from.label, edge.to.label);
			} 
			else
			{
				fprintf(nfa_dot, "\tq%d -> q%d [ label = \"%c\" ];\n", 
				edge.from.label, edge.to.label, edge.label);
			}
			
		}

		fprintf(nfa_dot, "}\n");
		fclose(nfa_dot);

		char *cmd1 = "dot -Tpng ";
		char *cmd2 = " -o ";
		char *cmd3 = concat(cmd1, outDotFileName);
		char *cmd4 = concat(cmd3, cmd2);
		char *cmd = concat(cmd4, outPngFileName);
		system(cmd); // générer l'image de l'arbre

		free(outDotFileName);
		free(outPngFileName);
		free(cmd3);
		free(cmd4);
		free(cmd);
	}
}

void make_dfa_graph(dfa *dfautomata, char *graphName)
{
	system("mkdir -p ../graphs"); // créer le dossier graphs s'il n'existe pas
	char *dirName = "../graphs/";
	char *ext1 = ".dot";
	char *dirFile = concat(dirName, graphName);
	char *outDotFileName = concat(dirFile, ext1);
	char *ext2 = ".png";
	char *outPngFileName = concat(dirFile, ext2);
	free(dirFile);

	FILE *dfa_dot = fopen(outDotFileName, "w");
	if(dfa_dot == NULL){
		printf("Erreur d'ouverture du fichier\n");
		exit(1);
	}
	else
	{
		fprintf(dfa_dot, "digraph %s {\n", graphName);
		fprintf(dfa_dot, "\tfontname=\"Helvetica,Arial,sans-serif\";\n");
		fprintf(dfa_dot, "\tnode [fontname=\"Helvetica,Arial,sans-serif\"];\n");
		fprintf(dfa_dot, "\tedge [fontname=\"Helvetica,Arial,sans-serif\"];\n");
		fprintf(dfa_dot, "\trankdir=LR;\n");
		fprintf(dfa_dot, "\tnode [shape = doublecircle]; ");
		
		for(int i = 0; i<dfautomata->nFinalStates; i++)
		fprintf(dfa_dot, "%d; ", dfautomata->final_states[i].label);
		fprintf(dfa_dot, "\n");
		
		fprintf(dfa_dot, "\tnode [shape = point ]; ");
		for(int i = 0; i<dfautomata->nInitStates; i++)
			fprintf(dfa_dot, "qi%d", i);
		fprintf(dfa_dot, "\n");

		fprintf(dfa_dot, "\tnode [shape = circle];\n");

		for(int i = 0; i<dfautomata->nInitStates; i++)
			fprintf(dfa_dot, "\tqi%d -> %d;\n", i, dfautomata->initial_states[i].label);
		
		for(int i = 0; i<dfautomata->nEdges; i++)
		{
			edge e = dfautomata->edges[i];
			fprintf(dfa_dot, "\t%d -> %d [ label = \"%c\" ];\n", 
			e.from.label, e.to.label, e.label);
		}

		fprintf(dfa_dot, "}\n");
		fclose(dfa_dot);

		char *cmd1 = "dot -Tpng ";
		char *cmd2 = " -o ";
		char *cmd3 = concat(cmd1, outDotFileName);
		char *cmd4 = concat(cmd3, cmd2);
		char *cmd = concat(cmd4, outPngFileName);
		system(cmd); // générer l'image de l'arbre

		free(outDotFileName);
		free(outPngFileName);
		free(cmd3);
		free(cmd4);
		free(cmd);

	}
}

unsigned concatenate(unsigned x, unsigned y) {
    unsigned pow = 10;
    while(y >= pow)
        pow *= 10;
    return x * pow + y;        
}

