#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "ast.h"
#include "fa.h"


int main(int argc, char *argv[]){

	char *fileName = argv[1];
	//char *fileName = "../regExp.txt";
	char *e = getRegExpFromFile(fileName);
	char *exp = getRegExpFromFile(fileName);
	printf("Expression a analyser en infix: %s \n", e);
	char *ePostfix = infixToPostfix(e);
	printf("Expression en postfix (le \".\" représente la concaténation): %s \n", ePostfix);
	free(e);

	// AST
	printf("\n**********\n");
	printf("Construction de l'arbre de syntaxe abstraite...\n");
	ast *t = buildAstFromPostfixExp(ePostfix);
	free(ePostfix);
	make_ast_graph(t);
	printf("Arbre généré avec success (../graphs/ast.png).\n");
	
	// NFA
	printf("\n**********\n");
	printf("Construction de l'automate fini non déterministe...\n");
	int n = 0;
	nfa *nfautomata = mnyt(t, &n);
	make_nfa_graph(nfautomata, "nfa");
	printf("Automate généré avec success (../graphs/nfa.png).\n");
	free(t);

	// DFA
	printf("\n**********\n");
	printf("Construction de l'automate fini déterministe...\n");
	dfa *dfautomata = nfa2dfa(nfautomata);
	make_dfa_graph(dfautomata, "dfa");
	printf("Automate généré avec success (../graphs/dfa.png).\n");
	dfautomata = complete_dfa(dfautomata);
	printf("Construction de l'automate fini déterministe complet...\n");
	make_dfa_graph(dfautomata, "dfa_complet");
	printf("Automate généré avec success (../graphs/dfa_complet.png).\n");
	free(nfautomata);

	// Minimized DFA
	printf("\n**********\n");
	printf("Minimisation de l'automate fini déterministe...\n");
	dfa *dfa_min = minimize_dfa(dfautomata);
	make_dfa_graph(dfa_min, "dfa_min");
	printf("Automate minimal généré avec success (../graphs/dfa_min.png).\n");
	free(dfautomata);

	// Génération de l'analyseur lexical
	printf("\n**********\n");
	printf("Génération de l'analyseur lexical...\n");
	generate_al(dfa_min, exp);
	printf("Analyseur lexical généré avec success (./al.c).\n");
	printf("Utilisation après compilation avec gcc: ./al <mot_à_analyser>\n");
	free(dfa_min);
	free(exp);

	return 0;
}