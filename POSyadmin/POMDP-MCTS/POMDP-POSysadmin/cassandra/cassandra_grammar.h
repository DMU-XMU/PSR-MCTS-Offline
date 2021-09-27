// The structure that is returned by the parser, containing all the data in the .POMDP file

#ifndef __CASSANDRA_GRAMMAR_H_
#define __CASSANDRA_GRAMMAR_H_

#include <stdio.h>

typedef struct {

int nrstates, nract, nrobs;
float discount;
enum valuetype {vREWARD,vCOST} ;
char ** arr_states; // arr_states[i] is a NULL-terminated string representing mnemonic of state i
char ** arr_act; // arr_act[i] is a NULL-terminated string representing mnemonic of action i
char ** arr_obs; // arr_obs[i] is a NULL-terminated string representing mnemonic of observation i
float *** Tr; // Tr[a][s1][s2] is the probability of getting from s1 to s2 under a
float *** Ob; // Ob[a][s][o] is the prob of observing o when performing a and ending up in s
int **** Rw; // Rw[a][s1][s2][o] is the reward when getting o as a went from s1 to s2
float *Start;
float values;
// couldn't use T/O/R because of Bison (don't shoot the programmer...)
} Cassandra_return;

Cassandra_return *parse_Cassandra(FILE *f);


#endif
