%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cassandra_grammar.h"
#include<iostream>
  using namespace std;
	
extern int yylex();
void yyerror(const char *s);
int nrdigit(int a, int base);

 Cassandra_return *toret=(Cassandra_return *) malloc(sizeof(Cassandra_return));
 //Cassandra_return *ret=(Cassandra_return *) malloc(sizeof(Cassandra_return));
float *theMatrix = NULL;
int theCursor = -1;

%}

%union {
	char *	t_pchar;
	int *	t_pint;	
	float *	t_pfloat;
	int	t_int;
	float	t_float;
}

%token DISCOUNT VALUES REWARD COST STATES ACTIONS OBSERVATIONS START
%token T O R IDENTITY UNIFORM ASTERISK
%token <t_int>	INTEGER
%token <t_pchar> STRING
%token <t_float> FLOAT

%type <t_int> sign
%type <t_float>	prob number
%type <t_pint>	state action obs
/* %type <t_pfloat> matrix_1xs recur_1xs matrix_sxs recur_sxs matrix_1xo recur_1xo matrix_sxo recur_sxo */

%start pomdp_file

%%

pomdp_file:	preamble param_list ;
preamble:	/* empty */ {
                        toret->nrstates	= 0;
			toret->nract	= 0;
			toret->nrobs	= 0;
                        toret->Start    = NULL;
			toret->Tr 	= NULL;
			toret->Ob	= NULL;
			toret->Rw	= NULL;
                  
		}
		| preamble paramtype ;
paramtype:	discount_param
		| value_param
		| state_param
		| act_param
		| obs_param
                | start_param
		;

discount_param:	DISCOUNT ':' number	{toret->discount=$3;/* set the discount factor */ }
                ;



/*start_param:    START ':' prob {toret->START=$3;
		}
		;*/
start_param:    START ':'  {
			if (toret->Start == NULL) {
                            int i;
				
				for (i=0; i<toret->nrstates; i++) { 
					toret->Start = (float *) malloc(toret->nrstates * sizeof(float ));
				}
			} 
		} start_val
		;
start_val:     matrix_sxs		{
			
			memcpy(toret->Start, theMatrix , toret->nrstates * sizeof(float *));
			free(theMatrix); theMatrix=NULL; theCursor=-1;
		} ;

value_param:	VALUES ':' value_val ;
value_val:	REWARD{ toret->values=0;/* set wether values are rewards or costs */ }
                |COST{ toret->values=1;/* set wether values are rewards or costs */ }
		;

state_param:	STATES ':' state_val ;
state_val:	INTEGER			{
			/* initialize n states with default mnemonics */
			toret->nrstates = $1;
			toret->arr_states = (char **) malloc($1 * sizeof(char *));
			int i;
			for (i=0; i<$1; i++) { 
				toret->arr_states[i] = (char *) malloc((nrdigit($1,16)+2) * sizeof(char));
				sprintf(toret->arr_states[i], "s%X", i);
			}
		}
		| state_list
		;
state_list:	state_list STRING	{
			/* add new symbol with given mnemonic */
			toret->nrstates++;
			char **tmpptr = (char **) malloc(toret->nrstates * sizeof(char *));
			memcpy(tmpptr, toret->arr_states, (toret->nrstates-1)*sizeof(char *));
			free(toret->arr_states);
			toret->arr_states = tmpptr;
			toret->arr_states[toret->nrstates-1] = (char *) malloc((strlen($2)+1)*sizeof(char));
			strcpy(toret->arr_states[toret->nrstates-1], $2);
			free($2);
		}
		| STRING		{
			/* add new symbol with given mnemonic */
			toret->nrstates++;
			char **tmpptr = (char **) malloc(toret->nrstates * sizeof(char *));
			memcpy(tmpptr, toret->arr_states, (toret->nrstates-1)*sizeof(char *));
			free(toret->arr_states);
			toret->arr_states = tmpptr;
			toret->arr_states[toret->nrstates-1] = (char *) malloc((strlen($1)+1)*sizeof(char));
			strcpy(toret->arr_states[toret->nrstates-1], $1);
			free($1);
		}
		;

act_param:	ACTIONS ':' act_val ;
act_val:	INTEGER			{
			/* initialize n actions with default mnemonics */
			toret->nract = $1;
			toret->arr_act = (char **) malloc($1 * sizeof(char *));
			int i;
			for (i=0; i<$1; i++) {
				toret->arr_act[i] = (char *) malloc((nrdigit($1,16)+2) * sizeof(char));
				sprintf(toret->arr_act[i], "a%X", i);
			}
		}
		| act_list
		;
act_list:	act_list STRING		{
			/* add new action with given mnemonic */
			toret->nract++;
			char **tmpptr = (char **) malloc(toret->nract * sizeof(char *));
			memcpy(tmpptr, toret->arr_act, (toret->nract-1)*sizeof(char *));
			free(toret->arr_act);
			toret->arr_act = tmpptr;
			toret->arr_act[toret->nract-1] = (char *) malloc((strlen($2)+1)*sizeof(char));
			strcpy(toret->arr_act[toret->nract-1], $2);
			free($2);
		}
		| STRING		{
			/* add new action with given mnemonic */
			toret->nract++;
			char **tmpptr = (char **) malloc(toret->nract * sizeof(char *));
			memcpy(tmpptr, toret->arr_act, (toret->nract-1)*sizeof(char *));
			free(toret->arr_act);
			toret->arr_act = tmpptr;
			toret->arr_act[toret->nract-1] = (char *) malloc((strlen($1)+1)*sizeof(char));
			strcpy(toret->arr_act[toret->nract-1], $1);
			free($1);
		}
		;

obs_param:	OBSERVATIONS ':' obs_val ;
obs_val:	INTEGER			{
			/* initialize n observations with default mnemonics */
			toret->nrobs = $1;
			toret->arr_obs = (char **) malloc($1 * sizeof(char *));
			int i;
			for (i=0; i<$1; i++) {   
				toret->arr_obs[i] = (char *) malloc((nrdigit($1,16)+2) * sizeof(char));
				sprintf(toret->arr_obs[i], "o%X", i);
			}
		}
		| obs_list
		;
obs_list:	obs_list STRING		{
			/* add new observation with given mnemonic */
			toret->nrobs++;
			char **tmpptr = (char **) malloc(toret->nrobs * sizeof(char *));
			memcpy(tmpptr, toret->arr_obs, (toret->nrobs-1)*sizeof(char *));
			free(toret->arr_obs);
			toret->arr_obs = tmpptr;
			toret->arr_obs[toret->nrobs-1] = (char *) malloc((strlen($2)+1)*sizeof(char));
			strcpy(toret->arr_obs[toret->nrobs-1], $2);
			free($2);
		}
		| STRING		{
			/* add new observation with given mnemonic */
			toret->nrobs++;
			char **tmpptr = (char **) malloc(toret->nrobs * sizeof(char *));
			memcpy(tmpptr, toret->arr_obs, (toret->nrobs-1)*sizeof(char *));
			free(toret->arr_obs);
			toret->arr_obs = tmpptr;
			toret->arr_obs[toret->nrobs-1] = (char *) malloc((strlen($1)+1)*sizeof(char));
			strcpy(toret->arr_obs[toret->nrobs-1], $1);
			free($1);
		}
		;


param_list:	param_list param_val | param_val ;

param_val:	transit_param
		


| observ_param
		| rewards_param
		;




transit_param:	T ':' /* mid-rule action */ {
			if (toret->Tr == NULL) {
				int i, j;
				toret->Tr = (float ***) malloc(toret->nract * sizeof(float **));
				for (i=0; i<toret->nract; i++) { 
					toret->Tr[i] = (float **) malloc(toret->nrstates * sizeof(float *));
					for (j=0; j<toret->nrstates; j++)
						toret->Tr[i][j] = (float *) malloc(toret->nrstates * sizeof(float));
				}
			} // Blergh.
		} transit_val
		;
transit_val:	action ':' state ':' state prob	{
			/* record the prob of getting from state1 to state2 through action */
			int i = 0, j, k;
			while ($1[i]!=-1) { // need to loop in case of *'s
				j = 0;
				while ($3[j]!=-1) {
					k = 0;
					while ($5[k]!=-1) toret->Tr [$1[i]] [$3[j]] [$5[k++]] = $6;
					j++;
				}
				i++;
			}
			free($1); free($3); free($5);			
		}
		| action ':' state matrix_1xs	{
			/* record the prob of getting anywhere from state through action */
			int i = 0, j;
			while ($1[i]!=-1) { // need to loop in case of *'s
				j = 0;
				while ($3[j]!=-1) memcpy(toret->Tr [$1[i]] [$3[j++]] , theMatrix , toret->nrstates * sizeof(float *));
				i++;
			}
			free(theMatrix); theMatrix=NULL; theCursor=-1;
			free($1); free($3);
		}
		| action matrix_sxs		{
			/* record the prob of getting anywhere from anywhere through action */
			int i = 0, j;
			while ($1[i]!=-1) { // need to loop in case of *'s
				for (j=0; j<toret->nrstates; j++)
					memcpy(toret->Tr[$1[i]][j], theMatrix + j * toret->nrstates, toret->nrstates * sizeof(float *));
				i++;
			}
			free(theMatrix); theMatrix=NULL; theCursor=-1;
			free($1);
		}
		;

observ_param:	O ':' /* mid-rule action */ {
			if (toret->Ob == NULL) {
				int i, j;
				toret->Ob = (float ***) malloc(toret->nract * sizeof(float **));
				for (i=0; i<toret->nract; i++) {
					toret->Ob[i] = (float **) malloc(toret->nrstates * sizeof(float *));
					for (j=0; j<toret->nrstates; j++)
						toret->Ob[i][j] = (float *) malloc(toret->nrobs * sizeof(float));
				}
			}
		} observ_val ;
observ_val:	action ':' state ':' obs prob	{
			/* record the prob of getting 'obs' if 'action' is done in 'state' */
			int i = 0, j, k;
			while ($1[i]!=-1) { // need to loop in case of *'s
				j = 0;
				while ($3[j]!=-1) {
					k = 0;
					while ($5[k]!=-1) toret->Ob [$1[i]] [$3[j]] [$5[k++]] = $6;
					j++;
				}
				i++;
			}
			free($1); free($3); free($5);
		}
		| action ':' state matrix_1xo	{
			/* record the prob of getting any observation when doing 'action' in 'state' */
			int i = 0, j;
			while ($1[i]!=-1) { // need to loop in case of *'s
				j = 0;
				while ($3[j]!=-1) memcpy(toret->Ob [$1[i]] [$3[j++]] , theMatrix, toret->nrobs * sizeof(float *));
				i++;
			}
			free(theMatrix); theMatrix=NULL; theCursor=-1;
			free($1); free($3);
		}
		| action matrix_sxo		{
			/* record the prob of getting any observation from any state when doing 'action' */
			int i = 0, j;
			while ($1[i]!=-1) { // need to loop in case of *'s
				for (j=0; j<toret->nrstates; j++)
					memcpy(toret->Ob[$1[i]][j], theMatrix + j * toret->nrobs, toret->nrobs * sizeof(float *));
				i++;
			}
			free(theMatrix); theMatrix=NULL; theCursor=-1;
			free($1);
		}
		;

rewards_param:	R ':' rewards_val ; /* rewards are not currently needed by the PSRs and are thus NOT IMPLEMENTED! */
rewards_val:	action ':' state ':' state ':' obs number	{ free($1); free($3); free($5); free($7); }
		| action ':' state ':' state nr_matrix		{ free($1); free($3); free($5); }
		| action ':' state nr_matrix			{ free($1); free($3); }
		;

/* STATE - ACTION - OBSERVATION PRIMITIVES */
state:		INTEGER	{ // return an int array containing [state, -1]
			$$ = (int *) malloc(2 * sizeof(int)); $$[0] = $1; $$[1] = -1;
		}
		| STRING { // return an int array containing [state, -1]
			int i, found=0;
			$$ = (int *) malloc(2 * sizeof(int));
			for (i=0; i < toret->nrstates; i++) // first find the number of the given state
				if (!strcmp($1, toret->arr_states[i])) {
					$$[0]=i; found=1;
				}
			if (!found) $$[0]=-1;
			$$[1]=-1;
			free($1);
		}
		| ASTERISK { // return an int array containing all states, followed by -1
			int i;
			$$= (int *) malloc(sizeof(int) * (toret->nrstates+1));
			for (i=0; i < toret->nrstates; i++) $$[i] = i;
			$$[toret->nrstates] = -1;
		}
		;
action:		INTEGER	{ // return an int array containing [action, -1]
			$$ = (int *) malloc(2 * sizeof(int)); $$[0] = $1; $$[1] = -1;
		}
		| STRING { // return an int array containing [action, -1]
			int i, found=0;
			$$ = (int *) malloc(2 * sizeof(int));
			for (i=0; i < toret->nract; i++) // first find the number of the given action
				if (!strcmp($1, toret->arr_act[i])) {
					$$[0]=i; found=1;
				}
			if (!found) $$[0]=-1;
			$$[1]=-1;
			free($1);
		}
		| ASTERISK { // return an int array containing all actions, followed by -1
			int i;
			$$ = (int *) malloc(sizeof(int) * (toret->nract+1));
			for (i=0; i < toret->nract; i++) $$[i] = i;
			$$[toret->nract] = -1;
		}
		;
obs:		INTEGER	{ // return an int array containing [observation, -1]
			$$ = (int *) malloc(2 * sizeof(int)); $$[0] = $1; $$[1] = -1;
		}
		| STRING { // return an int array containing [observation, -1]
			int i, found=0;
			$$ = (int *) malloc(2 * sizeof(int));
			for (i=0; i < toret->nrobs; i++) // first find the number of the given observation
				if (!strcmp($1, toret->arr_obs[i])) {
					$$[0]=i; found=1;
				}
			if (!found) $$[0]=-1;
			$$[1]=-1;
			free($1);
		}
		| ASTERISK { // return an int array containing all observations, followed by -1
			int i;
			$$ = (int *) malloc(sizeof(int) * (toret->nrobs+1));
			for (i=0; i < toret->nrobs; i++) $$[i] = i;
			$$[toret->nrobs] = -1;
		}
		;

/* MATRICES */
matrix_1xs:	UNIFORM	{
			float uniformVal = 1.0 / toret->nrstates;
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * sizeof(float));
			for (theCursor=0; theCursor<toret->nrstates; theCursor++) theMatrix[theCursor] = uniformVal;
		}
		| recur_1xs ;
recur_1xs:	recur_1xs prob {
			if (theMatrix==NULL) printf("Critical problem: theMatrix ain't there!\n");
			else theMatrix[theCursor++] = $2;
		}
		| prob { // the end of the recursion, ie the beginning of the matrix
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * sizeof(float));
			theCursor = 0;
			theMatrix[theCursor++] = $1;
		}
		;

matrix_sxs:	UNIFORM {
			float uniformVal = 1.0 / toret->nrstates;
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * toret->nrstates * sizeof(float));
			for (theCursor=0; theCursor<toret->nrstates*toret->nrstates; theCursor++) theMatrix[theCursor] = uniformVal;
		}
		| IDENTITY {
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * toret->nrstates * sizeof(float));
			for (theCursor=0; theCursor<toret->nrstates*toret->nrstates; theCursor++) theMatrix[theCursor] = 0.0;
			for (theCursor=0; theCursor<toret->nrstates; theCursor++) theMatrix[theCursor*toret->nrstates+theCursor] = 1.0;
			theCursor = toret->nrstates * toret->nrstates;
		}
		| recur_sxs ;
recur_sxs:	recur_sxs prob {
			if (theMatrix==NULL) printf("Critical problem: theMatrix ain't there!\n");
			else theMatrix[theCursor++] = $2;
		}
		| prob { // the end of the recursion, ie the beginning of the matrix
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * toret->nrstates * sizeof(float));
			theCursor = 0;
			theMatrix[theCursor++] = $1;
		}
		;
		
matrix_1xo:	UNIFORM	{
			float uniformVal = 1.0 / toret->nrobs;
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrobs * sizeof(float));
			for (theCursor=0; theCursor<toret->nrobs; theCursor++) theMatrix[theCursor] = uniformVal;
		}
		| recur_1xo ;
recur_1xo:	recur_1xo prob {
			if (theMatrix==NULL) printf("Critical problem: theMatrix ain't there!\n");
			else theMatrix[theCursor++] = $2;
		}
		| prob { // the end of the recursion, ie the beginning of the matrix
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrobs * sizeof(float));
			theCursor = 0;
			theMatrix[theCursor++] = $1;
		}
		;

matrix_sxo:	UNIFORM {
			float uniformVal = 1.0 / toret->nrobs;
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * toret->nrobs * sizeof(float));
			for (theCursor=0; theCursor<toret->nrstates * toret->nrobs; theCursor++) theMatrix[theCursor] = uniformVal;
		}
		| recur_sxo ;
recur_sxo:	recur_sxo prob {
			if (theMatrix==NULL) printf("Critical problem: theMatrix ain't there!\n");
			else theMatrix[theCursor++] = $2;
		}
		| prob { // the end of the recursion, ie the beginning of the matrix
			if (theMatrix!=NULL) {
				free(theMatrix);
				printf("Possible problem: new matrix started but previous one not freed.\n");
			}
			theMatrix = (float *) malloc(toret->nrstates * toret->nrobs * sizeof(float));
			theCursor = 0;
			theMatrix[theCursor++] = $1;
		}
		;


nr_matrix:	nr_matrix number | number ;

/* NUMBER PRIMITIVES */
prob:		FLOAT { $$ = (float) $1; } ;
number:		sign FLOAT { $$ = $1 * $2; } | sign INTEGER { $$ = (float) ($1 * $2); } ;
sign:		'+' { $$ = 1; } | '-' { $$ = -1; } | /* empty */ { $$ = 1; } ;


%%

Cassandra_return *parse_Cassandra(FILE *f) {
	// This is the function that should be called from the cassandra_env constructor to load the data from file
  
	extern FILE *yyin;
	yyin = f;
	if (yyparse()!=0) return NULL;
	else return toret;
}


void yyerror(const char *s) { /* Called by yyparse on error */
	  printf("%s\n", s);
}

int nrdigit(int a, int base) { // returns number of digits a (in base 10) takes in base 'base'
	int i = 0;
	while (a/base > 0) { i++; a = a / base; }
	return i+1;
}
		

/*int main(int argc,char *argv[])
{Cassandra_return *ret;
  FILE *f;
 
char name[]={"/cheese.95.POMDP"};
 char buffer[80];
 getcwd(buffer, 80);
 printf("The current directory is: %s\n", buffer);
  char *cf=strcat(buffer,name);
 cout<<cf<<endl;  //char *filename = 当前目录+文件名; 利用getcwd(char* buff)函数获取当前工作路径
 if((f=fopen(cf,"r"))==NULL)
 {
     printf("cannot open file\n");
 exit(0);
}
  else printf("success\n");
 
 
 ret=parse_Cassandra(f);

cout<<"DISCOUNT="<<ret->discount<<endl;//加上61行toret->discount=$3;可正确输出；否则输出不对

cout<<"STATES="<<ret->nrstates<<endl;
cout<<"ACTIONS="<<ret->nract<<endl;
cout<<"OBSERVATIONS="<<ret->nrobs<<endl;

cout<<"VALUES="<<ret->values<<endl;//不管数据中是REWARD还是COST，只要用ret->vREWARD,输出0，用ret->vCOST, 输出1，只是输出了结构体中的值，而用ret->valuetype,会一直输出0，所以要在88行设置值， 怎么赋值？ 
cout<<"START="<<ret->Start[1]<<endl;//这句写法？ 结果不对


cout<<"Ob[1][2][2]="<<ret->Ob[1][2][2]<<endl;
 cout<<"Tr[1][11][7]="<<ret->Tr[1][1][1]<<endl;


//cout<<"Ob[0][59][1]="<<ret->Ob[0][59][1]<<endl;

//cout<<"Rw[1][2][56][0]="<<ret->Rw[1][2][56][0]<<endl;
}
*/


		
