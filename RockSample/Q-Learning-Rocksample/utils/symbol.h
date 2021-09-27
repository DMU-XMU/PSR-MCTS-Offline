#ifndef __SYMBOL_H_
#define __SYMBOL_H_

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <ostream>


typedef enum { SYM_ACTION, SYM_OBSERVATION, SYM_ACT_WILDCARD, SYM_OBS_WILDCARD, SYM_CUSTOM, SYM_LANDMARK, SYM_NEWACTION, SYM_NEWOBSER} symbol_type;

using namespace std;

class Symbol {
  // represents an action or an observation
  // linked list capability removed to allow symbols to simply reside in the environment object
  //	and be pointed to from code, without cloning. Moved to class symList.
  // equality is assured by id and type alone, regardless of mnemonic

  public:
  int id;	// an ID that's unique among all actions or among all observations
  //char *mnem; // a human-readable representation/mnemonic
  symbol_type type;

  friend ostream& operator<<(ostream &o, Symbol &s);

  Symbol(int i, symbol_type t);
  Symbol(Symbol *s);		// for cloning
  ~Symbol();
};
bool operator==(Symbol &a, Symbol &b); //运算符重载

class symList {
  // a linked list of symbols
	
  public:
  Symbol *s;
  symList *n;

  symList	*append(Symbol *sym);
  symList	*append(symList *sl);
  symList	*seekEnd();
  int	size();
  int	beginsWith(symList *ls); // returns 1 iff 'this' begins with 'ls', or viceversa if 'ls' longer
  bool  endWith(symList *ls);
  symList	*difference(symList *ls); // if 'this' begins with 'ls', returns the next node after the end of 'ls'
  symList *filter(symbol_type t); // returns the sub-symlist containing only symbols of type 't'

  friend ostream& operator<<(ostream &o, symList &l);

  symList(Symbol *sym, symList *next);
  symList(Symbol *sym);
  symList(symList *cp);
  symList();
  ~symList();
};
bool operator==(symList &a, symList &b);

class symbolComparator {
  public:
  // binary functor used in STL sets of Symbol's
  bool operator() (symList *s1, symList *s2);

  symbolComparator();
  ~symbolComparator();
};

#endif
