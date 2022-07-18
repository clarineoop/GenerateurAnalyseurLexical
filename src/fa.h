#ifndef FA_H
#define FA_H
#include "ast.h"

typedef struct state
{
    int label;
} state;

typedef struct edge
{
    state from;
    state to;
    char label;
} edge;

typedef struct nfa
{
    state *states;
    int nStates;
    edge *edges;
    int nEdges;
    state initial_state;
    state final_state;
} nfa;

typedef struct dfa
{
    state *states;
    int nStates;
    edge *edges;
    int nEdges;
    state *initial_states;
    int nInitStates;
    state *final_states;
    int nFinalStates;
    char *alphabet;
} dfa;

typedef struct statesList
{
    state *states;
    int nStates;
} statesList;

typedef struct Detats
{
    statesList *lists;
    int nLists;
} Detats;

state *makeState(int label);
edge *makeEdge(state *from, state *to, char label);
state * mergeStates(nfa *nfa1, nfa *nfa2);
state * concatStates(nfa *nfa1, nfa *nfa2);
nfa *replaceState(nfa *nfa, state state1, state state2);
edge * mergeEdges(nfa *nfa1, nfa *nfa2);
nfa *mnyt(ast *tree, int *n);
statesList *e_cloture_single_state(state s, nfa *nfautomata);
statesList *e_cloture_multiple_states(statesList *listStates, nfa *nfautomata);
statesList *transition(statesList *E, char a, nfa *nfautomata);
int state_in(state q, statesList *list);
int equal(statesList *list1, statesList *list2);
int statesListInDetats (statesList *list, Detats *detats);
char *getAlphabet(nfa *nfautomata);
dfa *nfa2dfa(nfa *nfautomata);
statesList *get_non_accepting_states(dfa *dfa_in);
dfa *complete_dfa(dfa *dfa_in);
dfa *minimize_dfa(dfa *dfa_in);
void generate_al(dfa *dfa_min, char *e);

#endif