// Programme C qui implémente l’analyseur lexical de
// l'expréssion rationelle e = (a|(bc))*(ba(ca|ba))
// à partir du DFA correspondant

#include <stdio.h>
#include <string.h>

// Etat courant du DFA
int dfa = 1;

// Fonction qui implémente l'état 0 du DFA
void state0(char c)
{
	if (c == 'a') dfa = 0;
	else if (c == 'b') dfa = 0;
	else if (c == 'c') dfa = 0;
	else dfa = -1;
}

// Fonction qui implémente l'état 1 du DFA
void state1(char c)
{
	if (c == 'a') dfa = 1;
	else if (c == 'b') dfa = 2;
	else if (c == 'c') dfa = 0;
	else dfa = -1;
}

// Fonction qui implémente l'état 2 du DFA
void state2(char c)
{
	if (c == 'a') dfa = 3;
	else if (c == 'b') dfa = 0;
	else if (c == 'c') dfa = 1;
	else dfa = -1;
}

// Fonction qui implémente l'état 3 du DFA
void state3(char c)
{
	if (c == 'a') dfa = 0;
	else if (c == 'b') dfa = 4;
	else if (c == 'c') dfa = 5;
	else dfa = -1;
}

// Fonction qui implémente l'état 4 du DFA
void state4(char c)
{
	if (c == 'a') dfa = 6;
	else if (c == 'b') dfa = 0;
	else if (c == 'c') dfa = 0;
	else dfa = -1;
}

// Fonction qui implémente l'état 5 du DFA
void state5(char c)
{
	if (c == 'a') dfa = 7;
	else if (c == 'b') dfa = 0;
	else if (c == 'c') dfa = 0;
	else dfa = -1;
}

// Fonction qui implémente l'état 6 du DFA
void state6(char c)
{
	if (c == 'a') dfa = 0;
	else if (c == 'b') dfa = 0;
	else if (c == 'c') dfa = 0;
	else dfa = -1;
}

// Fonction qui implémente l'état 7 du DFA
void state7(char c)
{
	if (c == 'a') dfa = 0;
	else if (c == 'b') dfa = 0;
	else if (c == 'c') dfa = 0;
	else dfa = -1;
}

// Fonction qui vérifie si une chaine est accptée
// par le DFA ou non
int isAccepted(char str[])
{
	int i, len = strlen(str);
	char c;

	for (i = 0; i < len; i++) {
		c = str[i];
		if (dfa == 0) state0(c);
		else if (dfa == 1) state1(c);
		else if (dfa == 2) state2(c);
		else if (dfa == 3) state3(c);
		else if (dfa == 4) state4(c);
		else if (dfa == 5) state5(c);
		else if (dfa == 6) state6(c);
		else if (dfa == 7) state7(c);
		else return 0;
	}

	if ( dfa == 6 || dfa == 7 ) return 1;
	else return 0;
}

// driver code
int main(int argc, char *argv[])
{
	if (argc != 2) 
	{
		printf("Usage: al <mot_à_analyser>\n");
		return -1;
	}
	char *str = argv[1];
	if (isAccepted(str)) printf("%s appartient au langage rationnel défini par e = (a|(bc))*(ba(ca|ba)).\n", str);
	else printf("%s n'appartient pas au langage rationnel défini par e = (a|(bc))*(ba(ca|ba)).\n", str);
	return 0;
}