%{
#include "cassandra_grammar.h"
#include "cassandra_grammar.tab.h"
#include <string.h>

int debug = 1;

%}


DIGIT	[0-9]
STRING	[_\-a-z][_\-a-z0-9]*

%%

[ \t\n]+
^#.*	/* eat up lines starting with # */

"*"			{ if (debug) printf("ASTERISK\n"); return ASTERISK; }
T			{ if (debug) printf("\nT"); return T; }
O			{ if (debug) printf("\nO"); return O; }
R			{ if (debug) printf("\nR"); return R; }
IDENTITY		{ if (debug) printf("\nIDENTITY"); return IDENTITY; }
UNIFORM			{ if (debug) printf("\nUNIFORM"); return UNIFORM; }
DISCOUNT		{ if (debug) printf("\ndiscount"); return DISCOUNT; }
VALUES			{ if (debug) printf("\nvalues"); return VALUES; }
REWARD			{ if (debug) printf("reward\n"); return REWARD; }
COST			{ if (debug) printf("cost"); return COST; }
STATES			{ if (debug) printf("\nstates"); return STATES; }
ACTIONS			{ if (debug) printf("\nactions"); return ACTIONS; }





OBSERVATIONS		{ if (debug) printf("\nobservations"); return OBSERVATIONS; }
START			{ if (debug) printf("\nstart"); return START; }
{DIGIT}+"."{DIGIT}+	{ sscanf(yytext, "%f", &yylval.t_float); if (debug) printf("FLOAT: %f\n", yylval.t_float);return FLOAT; }
{DIGIT}*		{ sscanf(yytext, "%d", &yylval.t_int); if (debug) printf("INTEGER: %d\n", yylval.t_int); return INTEGER; }
{STRING}		{ yylval.t_pchar = strdup(yytext); if (debug) printf("STRING: %s\n", yylval.t_pchar); return STRING; }

.			{ if (debug) printf("'%c'\n",yytext[0]); return yytext[0]; }

%%

int yywrap() { return 1; }
