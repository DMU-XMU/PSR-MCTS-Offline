#ifndef __SYMBOL_CPP_
#define __SYMBOL_CPP_

#include "symbol.h"

Symbol::Symbol(int i, symbol_type t) :
 id(i), type(t) { }

Symbol::Symbol(Symbol *s) :
  id(s->id), type(s->type) { }

Symbol::~Symbol() {}

bool operator==(Symbol &a, Symbol &b) {
  if ((a.id == b.id) && (a.type == b.type)) return true;
  else return false;
}
ostream& operator<<(ostream &o, Symbol &s) {
  return o  << "(i" << s.id << ":t" << s.type << ')';
}

// ***************
// *   symList   *
// ***************

symList *symList::append(symList *sl) {               
  if( (sl->s == NULL) && (sl->n == NULL) )           
    return seekEnd();
    //return this;
  if ((s==NULL) && (n==NULL)) 
  {
    s = sl->s;
    n = sl->n==NULL ? NULL : new symList(sl->n);
    delete sl;                                      
  }
  else if (n==NULL) n = sl;
  else return n->append(sl);
  return seekEnd();
}
symList *symList::append(Symbol *sym) { return append(new symList(sym)); }

symList *symList::seekEnd() { return n==NULL ? this : n->seekEnd(); }

int symList::size() { if (n==NULL) return 1; else return 1 + n->size(); }

int symList::beginsWith(symList *ls) {
  if ((ls==NULL) || ((ls->s==NULL)&&(ls->n==NULL))) 
    return 1;
  else if ((s->id == ls->s->id) && (s->type == ls->s->type))
    return ls->n==NULL?1:n->beginsWith(ls->n); // recur
  else return 0;
 
}

symList	*symList::difference(symList *ls) {
  if (ls==NULL || ls->s==NULL) return this;
  else if (s==NULL) return NULL;
  else if ((s->id == ls->s->id) && (s->type == ls->s->type)) return n==NULL?NULL:n->difference(ls->n); 
  else return NULL;
}

symList *symList::filter(symbol_type t) {
  //if(t>1)
  //t=0;
  if (s==NULL) return n==NULL ? NULL : n->filter(t);
  else if (s->type==t) return n==NULL ? new symList(s) : new symList(s, n->filter(t));
  else return n==NULL ? NULL : n->filter(t);
}
bool  symList::endWith(symList *ls)                  
{
  int size_th   = this->size();                       
  int size_ls = ls->size();                            
  if(size_ls > size_th)
    return false;
  symList* sl = this;
  for(int i=0; i<(size_th-size_ls); i++)
    sl = sl->n;
  if(*sl == *ls)                                      
    return true;
  else
    return false;
}

symList::symList(Symbol *sym, symList *next) : s(sym), n(next) { }
symList::symList(Symbol *sym) : s(sym), n(NULL) { }
symList::symList(symList *cp) : s(cp->s) { n = cp->n==NULL ? NULL : new symList(cp->n);  }
symList::symList() : s(NULL), n(NULL) { }
symList::~symList() { if (n!=NULL) { delete n; n=NULL; } }

bool operator==(symList &a, symList &b) {
  bool areeq;
  if (a.s == NULL) {
    if (b.s == NULL) areeq = true;
    else areeq = false;
  }
  else {
    if (b.s == NULL) areeq = false;
    else areeq = (*(a.s) == *(b.s));
  }
  if (areeq) {
    if (a.n == NULL) {
      if (b.n == NULL) return true;
      else return false;
    }
    else {
      if (b.n == NULL) return false;
      else return *(a.n) == *(b.n);
    }
  }
  else return false;
}
ostream& operator<<(ostream &o, symList &l) {        
//	return o << (l.s!=NULL?*(l.s):'\0') << " - " << (l.n!=NULL?*(l.n):'\0');
  return l.n!=NULL?	(l.s!=NULL? o << *(l.s) : o) << " - " << *(l.n)
  :	(l.s!=NULL? o << *(l.s) : o);
}

// **********************
// *  symbolComparator  *
// **********************
// REMEMBER: Equality can be determined by the expression (!(a<b) && !(b<a)).

bool symbolComparator::operator() (symList *s1, symList *s2) {
  if (s2==NULL) return false;
  else if (s1==NULL) return true; // s2 is not empty
  else if (s2->s == NULL) return false;
  else if (s1->s == NULL) return true; // s2->s is not empty
  else if (s1->size()!=s2->size()) return s1->size()<s2->size();
  else if (s1->s->id != s2->s->id) return s1->s->id < s2->s->id;
  else return this->operator()(s1->n, s2->n);
}

symbolComparator::symbolComparator() {}
symbolComparator::~symbolComparator() {}

#endif
