#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fa.h"
#include "utils.h"
#include "ast.h"


/* 
	constructeur pour un etat d'un automate pour 
	simplifier la construction
*/
state *makeState(int label)
{
	state *s = (state *) malloc(sizeof(state));
	
	s->label = label;

	return s;
};

/* 
	constructeur pour un arret d'un automate pour 
	simplifier la construction
*/
edge *makeEdge(state *from, state *to, char label)
{
	edge *e = (edge *) malloc(sizeof(edge));
	
	e->from = *from;
    e->to = *to;
    e->label = label;

	return e;
};

/*
    fonction pour faire le merge de deslistes de state de deux automates
*/
state * mergeStates(nfa *nfa1, nfa *nfa2)
{
    state *newStates = malloc((nfa1->nStates+nfa2->nStates)*sizeof(state));
    int j = 0;
    for(int i=0; i<nfa1->nStates;i++)
    {
        newStates[j] = nfa1->states[i];
        j++;
    }
    for(int i=0; i<nfa2->nStates;i++)
    {
        newStates[j] = nfa2->states[i];
        j++;
    }

    return newStates;
}

/*
    fonction pour faire la concaténation(omettre état initial du 
    deuxième automate) de des listes de state de deux automates
*/
state * concatStates(nfa *nfa1, nfa *nfa2)
{
    state *newStates = malloc((nfa1->nStates+nfa2->nStates-1)*sizeof(state));
    int j = 0;
    for(int i=0; i<nfa1->nStates;i++)
    {
        newStates[j] = nfa1->states[i];
        j++;
    }
    for(int i=0; i<nfa2->nStates;i++)
    {
        if(nfa2->states[i].label != nfa2->initial_state.label)
        {
            newStates[j] = nfa2->states[i];
            j++;
        }
    }

    return newStates;
}

/*
    fonction pour remplacer un état dans un automate par un autre
*/
nfa *replaceState(nfa *nfa, state state1, state state2)
{
    int n_states = nfa->nStates;
    for(int i=0; i<n_states;i++)
    {
        if(nfa->states[i].label == state1.label) nfa->states[i] = state2;
    }
    
    if(nfa->initial_state.label == state1.label) nfa->initial_state = state2;
    if(nfa->final_state.label == state1.label) nfa->final_state = state2;
    
    int n_edges = nfa->nEdges;
    for(int i=0; i<n_edges;i++)
    {
        if(nfa->edges[i].from.label == state1.label) nfa->edges[i].from = state2;
        if(nfa->edges[i].to.label == state1.label) nfa->edges[i].to = state2;
    }

    return nfa;
}

/*
    fonction pour faire le merge des listes de edges de deux automates
*/
edge * mergeEdges(nfa *nfa1, nfa *nfa2)
{
    edge *newEdges = malloc((nfa1->nEdges+nfa2->nEdges)*sizeof(edge));
    int j = 0;
    for(int i=0; i<nfa1->nEdges;i++)
    {
        newEdges[j] = nfa1->edges[i];
        j++;
    }
    for(int i=0; i<nfa2->nEdges;i++)
    {
        newEdges[j] = nfa2->edges[i];
        j++;
    }

    return newEdges;
}

/*
    algorithme de McNaughton, Yamada et Thompson pour
    l'implémentation du NFA
*/
nfa *mnyt(ast *tree, int *n)
{
    nfa *nfa_out = (nfa *) malloc(sizeof(nfa));
    state *states;
    edge *edges;

    /*
        Si l’expression rationnelle est E = F|G
    */
    if(tree->symbol_or_operator == '|'){
        nfa *nfa1 = mnyt(tree->operands.binaryOperation.leftOperand, n);
        nfa *nfa2 = mnyt(tree->operands.binaryOperation.rightOperand, n);
        // two new states
        (*n)++;
        state *init_state = makeState(*n);
        (*n)++;
        state *final_state = makeState(*n);
        states = mergeStates(nfa1, nfa2);
        int n_states = nfa1->nStates + nfa2->nStates;
        states = (state *) realloc(states, (n_states + 2) * sizeof(state));
        states[n_states] = *init_state;
        states[n_states+1] = *final_state;

        // 4 new edges ('\0' est le mot vide: epsilon transition)
        edge *e1 = makeEdge(init_state, &(nfa1->initial_state), '\0');
        edge *e2 = makeEdge(init_state, &(nfa2->initial_state), '\0');
        edge *e3 = makeEdge(&(nfa1->final_state), final_state, '\0');
        edge *e4 = makeEdge(&(nfa2->final_state), final_state, '\0');
        edges = mergeEdges(nfa1, nfa2);
        int n_edges = nfa1->nEdges + nfa2->nEdges;
        edges = (edge *) realloc(edges, (n_edges + 4) * sizeof(edge));
        edges[n_edges] = *e1;
        edges[n_edges+1] = *e2;
        edges[n_edges+2] = *e3;
        edges[n_edges+3] = *e4;

        nfa_out->states = states;
        nfa_out->nStates = n_states + 2; 
        nfa_out->edges = edges;
        nfa_out->nEdges = n_edges + 4; 
        nfa_out->initial_state = *init_state;
        nfa_out->final_state = *final_state;

        free(nfa1);free(nfa2);
        return nfa_out;
    }

    /*
        Si l’expression rationnelle est E = FG
    */
    else if(tree->symbol_or_operator == '.'){
        nfa *nfa1 = mnyt(tree->operands.binaryOperation.leftOperand, n);
        nfa *nfa2 = mnyt(tree->operands.binaryOperation.rightOperand, n);

        state init_state = nfa1->initial_state;
        state final_state = nfa2->final_state;
        // remplacer état initial du deuxième automate par 
        // l'état final du premier
        nfa2 = replaceState(nfa2, nfa2->initial_state, nfa1->final_state);
        states = concatStates(nfa1, nfa2);
        edges = mergeEdges(nfa1, nfa2);
        
        nfa_out->states = states;
        nfa_out->nStates = nfa1->nStates + nfa2->nStates - 1;
        nfa_out->edges = edges;
        nfa_out->nEdges = nfa1->nEdges + nfa2->nEdges;
        nfa_out->initial_state = init_state;
        nfa_out->final_state = final_state;

        free(nfa1);free(nfa2);
        return nfa_out;
    }

    /*
        Si l’expression rationnelle est E = F*
    */
    else if(tree->symbol_or_operator == '*'){
        nfa *nfa1 = mnyt(tree->operands.unaryOperation.operand, n);
        (*n)++;
        state *init_state = makeState(*n);
        (*n)++;
        state *final_state = makeState(*n);

        int n_states = nfa1->nStates;
        states = (state *) malloc((n_states+2)*sizeof(state));
        for(int i = 0; i<n_states;i++) states[i] = nfa1->states[i];
        states[n_states] = *init_state;
        states[n_states+1] = *final_state;

        edge *e1 = makeEdge(init_state, &(nfa1->initial_state), '\0');
        edge *e2 = makeEdge(init_state, final_state, '\0');
        edge *e3 = makeEdge(&(nfa1->final_state), final_state, '\0');
        edge *e4 = makeEdge(&(nfa1->final_state), &(nfa1->initial_state), '\0');
        
        int n_edges = nfa1->nEdges;
        edges = (edge *) malloc((n_edges + 4)*sizeof(edge));
        for(int i = 0; i<n_edges;i++) edges[i] = nfa1->edges[i];
        edges[n_edges] = *e1;
        edges[n_edges+1] = *e2;
        edges[n_edges+2] = *e3;
        edges[n_edges+3] = *e4;

        nfa_out->states = states;
        nfa_out->nStates = n_states+2;
        nfa_out->edges = edges;
        nfa_out->nEdges = n_edges+4;
        nfa_out->initial_state = *init_state;
        nfa_out->final_state = *final_state;

        free(nfa1);
        return nfa_out;
    }
    
    /*
        si l’expression rationnelle correspond à un symbole de l’alphabet,
        construction du NFA à deux états et une transition
    */
    else 
    {
        (*n)++;
        state *init_state = makeState(*n);
        (*n)++;
        state *final_state = makeState(*n);
        states = (state *) malloc(2*sizeof(state));
        states[0] = *init_state;
        states[1] = *final_state;
        edge *e = makeEdge(init_state, final_state, tree->symbol_or_operator);
        edges = (edge *) malloc(1*sizeof(edge));
        edges[0] = *e;

        nfa_out->states = states;
        nfa_out->nStates = 2;
        nfa_out->edges = edges;
        nfa_out->nEdges = 1;
        nfa_out->initial_state = *init_state;
        nfa_out->final_state = *final_state;

        return nfa_out;
    }
}

statesList *e_cloture_single_state(state s, nfa *nfautomata)
{
    statesList *e_cloture = (statesList *) malloc(sizeof(statesList));
    e_cloture->states = (state *) malloc(sizeof(state));
    e_cloture->states[0] = s;
    e_cloture->nStates = 1;

    // initialiser le nombre détats de l'e_cloture à 1
    int n = 1;
    state q;
    int i = 0;
    // pour tous les états q dans l'e_cloture
    while(i < n)
    {
        q = e_cloture->states[i];
        // chercher toutes les transitions ayant q comme etat de départ et epsilon
        // comme label
        for(int j = 0; j<nfautomata->nEdges; j++)
        {
            edge e = nfautomata->edges[j];
            if(e.from.label == q.label && e.label == '\0')
            {
                // quand c'est le cas, ajouter l'état d'arrivée dans e_cloture et 
                // incrémenter le nombre d'etats de e_cloture
                n++;
                e_cloture->states = (state *) realloc(e_cloture->states, n*sizeof(state));
                e_cloture->states[n-1] = e.to;
                e_cloture->nStates = n;
            }
        }

        i++;
    }

    return e_cloture;
}

statesList *e_cloture_multiple_states(statesList *listStates, nfa *nfautomata)
{
    statesList *e_cloture_all = (statesList *) malloc(sizeof(statesList));
    e_cloture_all->nStates = 0;
    int flag_first_state = 1; int n; int j = 0; int k;

    state q;
    statesList *e_cloture;

    for(int i = 0; i<listStates->nStates; i++)
    {
        q = listStates->states[i];
        e_cloture = e_cloture_single_state(q, nfautomata);
        
        if(flag_first_state == 1)
        {
            n = e_cloture->nStates;
            e_cloture_all->states = (state *) malloc(n*sizeof(state));
            e_cloture_all->nStates = n;
            flag_first_state = 0;
        }
        else
        {
            n = e_cloture_all->nStates + e_cloture->nStates;
            e_cloture_all->states = (state *) realloc(e_cloture_all->states, n*sizeof(state));
            e_cloture_all->nStates = n;
        }
        
        k = 0;
        while(k < e_cloture->nStates){
            e_cloture_all->states[j] = e_cloture->states[k];
            k++; 
            j++;
        }

        free(e_cloture);
    }

    return e_cloture_all;
}

statesList *transition(statesList *E, char a, nfa *nfautomata)
{
    statesList *t = (statesList *) malloc(sizeof(statesList));
    t->states = (state *) malloc(sizeof(state));
    t->nStates = 0;
    int n = 1;

    int i = 0;int flag = 1;
    state q; edge e;

    while(i < E->nStates)
    {
        q = E->states[i];
        for(int j = 0; j<nfautomata->nEdges; j++)
        {
            e = nfautomata->edges[j];
            if(e.from.label == q.label && e.label == a)
            {
                if(flag == 1)
                {
                    t->states[n-1] = e.to;
                    t->nStates = n;
                    flag = 0;
                }
                else
                {
                    n++;
                    t->states = (state *) realloc(t->states, n*sizeof(state));
                    t->states[n-1] = e.to;
                    t->nStates = n;
                }
            }
        }
        i = i+1;
    }

    return t;
}

int state_in(state q, statesList *list)
{
    for(int i=0; i<list->nStates; i++) if(q.label == list->states[i].label) return 1;
    return 0;
}

int equal(statesList *list1, statesList *list2)
{
    for(int i=0; i<list1->nStates; i++) if(state_in(list1->states[i], list2) == 0) return 0;
    for(int i=0; i<list2->nStates; i++) if(state_in(list2->states[i], list1) == 0) return 0;
    return 1;
}

int statesListInDetats (statesList *list, Detats *detats)
{
    if(list->nStates <= 0) return -1;
    for(int i=0; i<detats->nLists; i++) if(equal(list, &(detats->lists[i])) == 1) return i+1;
    return -1;
}

char *getAlphabet(nfa *nfautomata)
{
    char *alphabet = (char *) malloc(1);
    alphabet[0] = '\0';
    int n = 1;
    
    for(int i=0; i<nfautomata->nEdges; i++)
    {
        edge e = nfautomata->edges[i];
        if(e.label != '\0' && strchr(alphabet, e.label) == NULL)
        {
            n++;
            alphabet = (char *) realloc(alphabet, n);
            alphabet[n-2] = e.label;
        }
    }

    alphabet[n-1] = '\0';
    return alphabet;
}

/*
    Convertion du NFA en DFA
*/
dfa *nfa2dfa(nfa *nfautomata)
{
    // automate deterministe de sortie
    dfa *dfautomata = (dfa *) malloc(sizeof(dfa));
    // liste d'états de l'automate déterministe
    Detats *detats = (Detats *) malloc(sizeof(Detats));
    detats->lists = (statesList *) malloc(sizeof(statesList));
    detats->nLists = 1;
    int n_states = 1;
    int n_final_states = 0;
    int n_edges = 0;
    int flag_final_state = 1;
    int flag_edge = 1;
    // epsilon cloture de l'automate non déterministe 
    // (etat initial de l'automate déterministe)
    statesList *list1 = e_cloture_single_state(nfautomata->initial_state, nfautomata);
    detats->lists[0] = *list1;
    dfautomata->initial_states = (state *) malloc(sizeof(state));
    state *init_state_dfa = makeState(1);
    dfautomata->initial_states[0] = *init_state_dfa;
    dfautomata->nInitStates = 1;
    // ajout de l'etat initial dans la liste d'états de l'automate déterministe
    dfautomata->states = (state *) malloc(sizeof(state));
    dfautomata->states[0] = *init_state_dfa;
    dfautomata->nStates = n_states;
    // vérifier si l'état initial est un état final (acceptant)
    // c'est à dire s'il contient un état final de l'automate non déterministe
    if(state_in(nfautomata->final_state, list1) == 1)
    {
        n_final_states++;
        dfautomata->final_states = (state *) malloc(sizeof(state));
        dfautomata->final_states[n_final_states-1] = *init_state_dfa;
        dfautomata->nFinalStates = 1;
        flag_final_state = 0;
    }

    // calcul de l'alphabet de l'automate non déterministe
    // meme alphabet que celui du déterministe
    char *alphabet = getAlphabet(nfautomata);
    dfautomata->alphabet = alphabet;

    // Tantque (il y a un état non-marqué E dans Detats) faire
    int i = 0;
    statesList *tran;
    statesList *F;
    statesList *E;
    state *from_state;
    state *to_state;
    edge *e; state *s;

    while(i < n_states)
    {
        //printf("State: %d\n", i+1);
        
        //Pour (chaque symbole d’entrée a) faire
        for(int j = 0; alphabet[j]!='\0'; j++)
        {
            E = &(detats->lists[i]);

            char a = alphabet[j];
            
            //printf("Symbol: %c\n", a);

            //printf("E: ");for(int z = 0; z<E->nStates; z++) printf("q%d ", E->states[z].label);
            //printf("\n");
            
            tran = transition(E, a, nfautomata);
            
            //printf("Transition(E,%c): ", a);for(int z = 0; z<tran->nStates; z++) printf("q%d ", tran->states[z].label);
            //printf("\n");

            F = e_cloture_multiple_states(tran, nfautomata);
            
            //printf("F: ");for(int z = 0; z<F->nStates; z++) printf("q%d ", F->states[z].label);
            //printf("\n");
            if(F->nStates > 0)
            {
                //Si (F n’est pas dans Detats) alors
                if(statesListInDetats(F, detats) == -1)
                {
                    //printf("F n'est pas dans Detats\n");
                    // Ajouter F en tant qu’état non-marqué dans Detats
                    n_states++;
                    detats->lists = (statesList *) realloc(detats->lists, n_states*sizeof(statesList));
                    detats->nLists = n_states;
                    detats->lists[n_states-1] = *F;

                    // ajouter F comme nouvel état dans l'automate déterministe
                    dfautomata->states = (state *) realloc(dfautomata->states, n_states*sizeof(state));
                    s = makeState(statesListInDetats(F, detats));
                    dfautomata->states[n_states-1] = *s;
                    dfautomata->nStates = n_states;

                    // vérifier si F est un état final (acceptant)
                    if(state_in(nfautomata->final_state, F) == 1)
                    {
                        //printf("F est un etat acceptant\n");
                        n_final_states++;
                        if(flag_final_state == 1)
                        {
                            dfautomata->final_states = (state *) malloc(n_final_states*sizeof(state));
                            flag_final_state = 0;
                        }
                        else
                        {
                            dfautomata->final_states = (state *) realloc(dfautomata->final_states, n_final_states*sizeof(state));
                        }
                        dfautomata->final_states[n_final_states-1] = *s;
                        dfautomata->nFinalStates = n_final_states;
                    }
                }

                from_state = makeState(i+1);
                to_state = makeState(statesListInDetats(F, detats));
                e = makeEdge(from_state, to_state, a);

                //printf("Etat target: %d\n", statesListInDetats(F, detats));

                // ajouter la nouvelle transition dans l'automate déterministe
                n_edges++;
                if(flag_edge == 1)
                {
                    dfautomata->edges = (edge *) malloc(n_edges * sizeof(edge));
                    flag_edge = 0;
                }
                else dfautomata->edges = (edge *) realloc(dfautomata->edges, n_edges * sizeof(edge));
                dfautomata->edges[n_edges-1] = *e;
                dfautomata->nEdges = n_edges;
            }
        }

        i++; 
    }

    free(detats);free(F);free(from_state);free(to_state);
    free(e);free(s);
    free(tran);free(list1);free(init_state_dfa);

    return dfautomata;
}

int isFinalState(state s, dfa *dfa_in)
{
    for(int i=0; i<dfa_in->nFinalStates; i++) if(s.label == dfa_in->final_states[i].label) return 1;
    return 0;
}

/*
    restitue les états non acceptants d'un dfa
*/
statesList *get_non_accepting_states(dfa *dfa_in)
{
    statesList *list = malloc(sizeof(statesList));
    int n = 0;int flag_first_state = 1;
    
    for(int i = 0; i < dfa_in->nStates; i++)
    {
        state s = dfa_in->states[i];
        if(isFinalState(s, dfa_in) == 0)
        {
            if(flag_first_state == 1){
                n++;
                list->states = malloc(n*sizeof(state));
                list->states[n-1] = s;
                list->nStates = n;
                flag_first_state = 0;
            }
            else
            {
                n++;
                list->states = realloc(list->states, n*sizeof(state));
                list->states[n-1] = s;
                list->nStates = n;
            }
        }
    }

    return list;
}

int compare_list_parts(Detats *pi1, Detats *pi2)
{
    for(int i=0; i<pi1->nLists; i++) if(statesListInDetats(&(pi1->lists[i]), pi2) == -1) return 0;
    for(int i=0; i<pi2->nLists; i++) if(statesListInDetats(&(pi2->lists[i]), pi1) == -1) return 0;
    return 1;
}

char *states_list_to_string(statesList *l)
{
    int n = 1;
    char *s = malloc(n*sizeof(char));
    s[n-1] = '{';

    for(int i=0; i<l->nStates; i++)
    {
        state q = l->states[i];
        n = n + 3;
        s = realloc(s, n*sizeof(char));
        s[n - 3] = ' ';
        s[n - 2] = q.label + '0';
        s[n - 1] = ' ';
    }
    n=n+2;
    s = realloc(s, n*sizeof(char));
    s[n - 2] = '}';
    s[n - 1] = '\0';
    
    return s;
}

char *pi_to_string(Detats *pi)
{
    int n = 1;
    char *s = malloc(n*sizeof(char));
    s[n - 1] = '[';

    for(int i=0; i<pi->nLists; i++)
    {
        statesList g = pi->lists[i];
        char *g_string = states_list_to_string(&g);

        int len = strlen(g_string);
        n = n + len;

        s = realloc(s, n*sizeof(char));
        for(int j = 0; g_string[j] != '\0'; j++)
        {
            s[n - len] = g_string[j];
            len--;
        }
        
        free(g_string);
    }
    n = n + 2;
    s = realloc(s, n*sizeof(char));
    s[n - 2] = ']';
    s[n - 1] = '\0';

    return s;
}

int get_partition(state q, Detats *pi)
{
    for(int i = 0; i < pi->nLists; i++)
    {
        statesList g = pi->lists[i];
        if(state_in(q, &g) == 1) return i+1;
    }

    return -1;
}

state *transition_func_dfa(state q, char a, dfa *dfa_in)
{
    for(int i = 0; i < dfa_in->nEdges; i++)
    {
        edge e = dfa_in->edges[i];
        if(e.from.label == q.label && e.label == a) return makeState(e.to.label);
    }

    state *s = makeState(-1);
    return s;
}

/*
    Completer un automate fini déterministe
    en ajoutant un état poubelle si necessaire
*/
dfa *complete_dfa(dfa *dfa_in)
{
    state *poubelle = makeState(0);
    int flag = 1;

    for(int i = 0; i < dfa_in->nStates; i++)
    {
        state q = dfa_in->states[i];

        for(int j = 0; dfa_in->alphabet[j] != '\0'; j++)
        {
            char a = dfa_in->alphabet[j];

            if(transition_func_dfa(q, a, dfa_in)->label == -1)
            {
                if(flag == 1)
                {
                    // Ajouter l'etat poubelle dans l'automate
                    dfa_in->nStates = dfa_in->nStates + 1;
                    dfa_in->states = realloc(dfa_in->states, dfa_in->nStates*sizeof(state));
                    dfa_in->states[dfa_in->nStates - 1] = *poubelle;
                    flag = 0;
                }
                // ajouter une transition entre vers l'etat poubelle 
                edge *e = makeEdge(&q, poubelle, a);
                dfa_in->nEdges = dfa_in->nEdges + 1;
                dfa_in->edges = realloc(dfa_in->edges, dfa_in->nEdges*sizeof(edge));
                dfa_in->edges[dfa_in->nEdges - 1] = *e;
            }
        }
    }

    return dfa_in;
}

statesList *makePartition(int id, int *group_ids, int size)
{
    statesList *part = malloc(sizeof(statesList));
    int n = 0;
    part->nStates = n;
    
    for(int i = 0; i < size; i++)
    {
        if(group_ids[i] == id)
        {
            state *q = makeState(i);
            if(part->nStates == 0) 
            {
                n++;
                part->states =  malloc(n*sizeof(state));
                part->nStates = n;
                part->states[n-1] = *q;
            }
            else
            {
                n++;
                part->states =  realloc(part->states, n*sizeof(state));
                part->nStates = n;
                part->states[n-1] = *q;
            }
            free(q);
        }
    }
    
    return part;
}

/*
    Minimisation d'un DFA
*/
dfa *minimize_dfa(dfa *dfa_in)
{
    // initialisation des partitions
    Detats *pi = malloc(sizeof(Detats));
    int n = 1;
    pi->lists = malloc(n*sizeof(statesList));
    pi->nLists = n;
    statesList *part1 = get_non_accepting_states(dfa_in);
    pi->lists[0] = *part1;
    
    for (int i = 0; i < dfa_in->nFinalStates; i++)
    {
        statesList *part = malloc(sizeof(statesList));
        part->nStates = 1;
        part->states =  malloc(sizeof(state));
        part->states[0] = dfa_in->final_states[i];

        n++;
        pi->lists = realloc(pi->lists, n*sizeof(statesList));
        pi->lists[n-1] = *part;
        pi->nLists = n;

        free(part);
    }

    char *alphabet = dfa_in->alphabet;

    // calcul des nouvelles partitions
     Detats *pi_new;
    // répéter jusqu'à ce que pi_new = pi
    while(1)
    {
        pi_new = malloc(sizeof(Detats));
        int n_parts_new = 0;
        pi_new->nLists = n_parts_new;

        // Pour (chaque groupe G de PI) faire
        for(int i = 0; i < pi->nLists; i++)
        {
            statesList g = pi->lists[i];

            // liste d'entiers représantant les id uniques de groupes
            int group_ids[dfa_in->nStates + 1];
            for(int j = 0; j < dfa_in->nStates + 1; j++) group_ids[j] = -1;
            
            // Pour chaqu'état q du groupe G
            for(int j = 0; j < g.nStates; j++)
            {
                state q = g.states[j];

                int id =  0;
                // Pour chaque symbol a de l'alphabet
                for(int k = 0; alphabet[k] != '\0'; k++)
                {
                    char a = alphabet[k];
                    
                    state *to = transition_func_dfa(q, a, dfa_in);
                    int part_num_in_pi = get_partition(*to, pi);
                    free(to);
                    id = concatenate(id, part_num_in_pi);
                }

                group_ids[q.label] = id;
            }

            int group_ids_check_dup[g.nStates];
            for(int i = 0; i < g.nStates; i++) group_ids_check_dup[i] = -1;
            int i_dup = 0;

            // creation des sous groupes
            for(int i = 0; i < dfa_in->nStates + 1; i++)
            {
                int id = group_ids[i];
                int duplicate = 0;
                for(int j = 0; j < g.nStates; j++) if(id == group_ids_check_dup[j]) duplicate = 1;
                if(duplicate == 0 && id != -1)
                {
                    statesList *part = makePartition(id, group_ids, dfa_in->nStates + 1);
                    n_parts_new++;
                    if(pi_new->nLists == 0) pi_new->lists = malloc(n_parts_new*sizeof(statesList));
                    else pi_new->lists = realloc(pi_new->lists, n_parts_new*sizeof(statesList));
                    pi_new->lists[n_parts_new - 1] = *part;
                    pi_new->nLists = n_parts_new;
                    free(part);
                    group_ids_check_dup[i_dup] = id;
                    i_dup++;
                }
            }
        }
        
        // break if pi_new == pi
        if (compare_list_parts(pi, pi_new) == 1){
            free(pi);
            pi = pi_new;
            break;
        }
        else
        {
            free(pi);
            pi = pi_new; 
        }
    }

    //char *pi_string  = pi_to_string(pi);
    //printf("PI FINAL: %s\n", pi_string);
    //free(pi_string);

    // automate deterministe minimal de sortie
    dfa *dfa_min = (dfa *) malloc(sizeof(dfa));
    dfa_min->nStates = pi->nLists;
    dfa_min->states = malloc(dfa_min->nStates * sizeof(state));
    dfa_min->nInitStates = 0;
    dfa_min->nFinalStates = 0;
    dfa_min->nEdges = 0;

    //printf("Nombre d'etats à creer: %d\n", dfa_min->nStates);

    // creation des états de l'automate déterministe minimal
    // pour chaque groupe G de PI
    for(int i = 0; i < pi->nLists; i++)
    {
        // créer un nouvel état
        statesList g = pi->lists[i];

        char *g_string  = states_list_to_string(&g);
        //printf("\nCréation de l'état pour le groupe G: %s\n", g_string);
        free(g_string);

        state *q = makeState(i);
        dfa_min->states[i] = *q;

        //printf("Etat créé: %d\n", dfa_min->states[i].label);

        // si G contient l'état initial alors il est l'état initial du dfa minimal
        int etat_init = 0;
        for(int j = 0; j < dfa_in->nInitStates; j++) if(state_in(dfa_in->initial_states[j], &g) == 1) etat_init = 1;
        if(etat_init == 1)
        {
            //printf("Il sera l'état initial\n");
            if(dfa_min->nInitStates == 0)
            {
                dfa_min->nInitStates = dfa_min->nInitStates + 1;
                dfa_min->initial_states = malloc(dfa_min->nInitStates * sizeof(state));
                dfa_min->initial_states[dfa_min->nInitStates - 1] = *q;
            }
            else
            {
                dfa_min->nInitStates = dfa_min->nInitStates + 1;
                dfa_min->initial_states = realloc(dfa_min->initial_states, dfa_min->nInitStates * sizeof(state));
                dfa_min->initial_states[dfa_min->nInitStates - 1] = *q;
            }
        }

        // si G contient un état final alors il est un état final du dfa minimal
        int etat_final = 0;
        for(int j = 0; j < dfa_in->nFinalStates; j++) if(state_in(dfa_in->final_states[j], &g) == 1) etat_final = 1;
        if(etat_final == 1)
        {
            //printf("Il sera l'état final\n");
            if(dfa_min->nFinalStates == 0)
            {
                dfa_min->nFinalStates = dfa_min->nFinalStates + 1;
                dfa_min->final_states = malloc(dfa_min->nFinalStates * sizeof(state));
                dfa_min->final_states[dfa_min->nFinalStates - 1] = *q;
            }
            else
            {
                dfa_min->nFinalStates = dfa_min->nFinalStates + 1;
                dfa_min->final_states = realloc(dfa_min->final_states, dfa_min->nFinalStates * sizeof(state));
                dfa_min->final_states[dfa_min->nFinalStates - 1] = *q;
            }
        }
        
        free(q);
    }

    //printf("\n\nCreation des transitions:\n");

    // création des transitions de l'automate déterministe minimal
    // pour chaque groupe G de PI
    for(int i = 0; i < pi->nLists; i++)
    {
        statesList g = pi->lists[i];

        //char *g_string  = states_list_to_string(&g);
        //printf("\nCréation des transitions pour le groupe G: %s\n", g_string);
        //free(g_string);

        // Pour chaque symbol a de l'alphabet
        for(int j = 0; alphabet[j] != '\0'; j++)
        {
            // créer une nouvelle transition
            char a = alphabet[j];

            //printf("symbol: %c\n", a);

            state from = dfa_min->states[i];

            state *to = transition_func_dfa(g.states[0], a, dfa_in);
            int part_num_in_pi = get_partition(*to, pi);
            free(to);

            state *dest = makeState(part_num_in_pi-1);
            edge *e = makeEdge(&from, dest, a);
            free(dest);

            if(dfa_min->nEdges == 0)
            {
                dfa_min->nEdges = dfa_min->nEdges + 1;
                dfa_min->edges = malloc(dfa_min->nEdges * sizeof(edge));
                dfa_min->edges[dfa_min->nEdges - 1] = *e;
            }
            else
            {
                dfa_min->nEdges = dfa_min->nEdges + 1;
                dfa_min->edges = realloc(dfa_min->edges, dfa_min->nEdges * sizeof(edge));
                dfa_min->edges[dfa_min->nEdges - 1] = *e;
            }

            //printf("Transition: %d -> %d\n", dfa_min->edges[dfa_min->nEdges - 1].from.label, dfa_min->edges[dfa_min->nEdges - 1].to.label);
            free(e);
        }
    }

    dfa_min->alphabet = alphabet;

    return dfa_min;
}

void generate_al(dfa *dfa_min, char *e)
{
    FILE *al = fopen("al.c", "w");
	if(al == NULL){
		printf("Erreur d'ouverture du fichier\n");
		exit(1);
	}
	else
	{
        fprintf(al, "// Programme C qui implémente l’analyseur lexical de\n");
        fprintf(al, "// l'expréssion rationelle e = %s\n", e);
        fprintf(al, "// à partir du DFA correspondant\n\n");

        fprintf(al, "#include <stdio.h>\n#include <string.h>\n\n// Etat courant du DFA\n");
        fprintf(al, "int dfa = %d;\n\n", dfa_min->initial_states[0].label);

        // pour chaque état du dfa, créer une fonction
        for (int i = 0; i < dfa_min->nStates; i++)
        {
            state q = dfa_min->states[i];
            fprintf(al, "// Fonction qui implémente l'état %d du DFA\n", q.label);
            fprintf(al, "void state%d(char c)\n{\n", q.label);
        
            // pour chaque symbole de l'alphabet
            for (int j = 0; dfa_min->alphabet[j] != '\0'; j++)
            {
                char a = dfa_min->alphabet[j];
                state *to = transition_func_dfa(q, a, dfa_min);
                
                if(j == 0) fprintf(al, "\tif (c == '%c') dfa = %d;\n", a, to->label);
                else fprintf(al, "\telse if (c == '%c') dfa = %d;\n", a, to->label);
            }

            fprintf(al, "\telse dfa = -1;\n}\n\n");
        }

        fprintf(al, "// Fonction qui vérifie si une chaine est accptée\n");
        fprintf(al, "// par le DFA ou non\n");
        fprintf(al, "int isAccepted(char str[])\n{\n");
        fprintf(al, "\tint i, len = strlen(str);\n\tchar c;\n\n");

        fprintf(al, "\tfor (i = 0; i < len; i++) {\n\t\tc = str[i];\n");

        // pour chaque état du dfa
        for (int i = 0; i < dfa_min->nStates; i++)
        {
            state q = dfa_min->states[i];
            if(i == 0) fprintf(al, "\t\tif (dfa == %d) state%d(c);\n", q.label, q.label);
            else fprintf(al, "\t\telse if (dfa == %d) state%d(c);\n", q.label, q.label);
        }

        fprintf(al, "\t\telse return 0;\n\t}\n\n");

        fprintf(al, "\tif ( dfa == %d ", dfa_min->final_states[0].label);
        for (int i = 1; i < dfa_min->nFinalStates; i++)
        {
            fprintf(al, "|| dfa == %d ", dfa_min->final_states[i].label);
        }
        fprintf(al, ") return 1;\n\telse return 0;\n}\n\n");

        fprintf(al, "// driver code\nint main(int argc, char *argv[])\n{\n");
        fprintf(al, "\tif (argc != 2) \n\t{\n\t\tprintf(\"Usage: al <mot_à_analyser>\\n\");\n\t\treturn -1;\n\t}\n");
        fprintf(al, "\tchar *str = argv[1];\n");
        fprintf(al, "\tif (isAccepted(str)) ");
        fprintf(al, "printf(\"%%s appartient au langage rationnel défini par e = %s.\\n\", str);\n", e);
        fprintf(al, "\telse ");
        fprintf(al, "printf(\"%%s n'appartient pas au langage rationnel défini par e = %s.\\n\", str);\n", e);
        fprintf(al, "\treturn 0;\n}");

        fclose(al);
    }
}