%{
/**
 * lellexer.l file
 * To generate the lexical analyzer run: "flex ltllexer.l"
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "ltl_formula.h"
#include "ltlparser.h"

#include <stdio.h>

%}

%option warn nodefault
%option reentrant noyywrap never-interactive nounistd
%option bison-bridge

LPAREN		"("
RPAREN		")"

EQUIV		"<->"
IMPLIES		"->"
OR			\|\|?
AND			&&?
RELEASE		R|V
UNTIL		U
WEAK_UNTIL	W
FUTURE		F|\<>
GLOBALLY	G|\[\]
NEXT		X
WEAK_NEXT	N
NOT			!|~

TRUE		1|[tT]rue|TRUE
FALSE		0|[fF]alse|FALSE
ID			[a-zA-Z_][a-zA-Z0-9_]*
WS			[ \r\n\t]*

%%

{LPAREN}        { return TOKEN_LPAREN;		}
{RPAREN}        { return TOKEN_RPAREN;		}
{EQUIV}			{ return TOKEN_EQUIV;		}
{IMPLIES}		{ return TOKEN_IMPLIES;		}
{OR}			{ return TOKEN_OR;			}
{AND}			{ return TOKEN_AND;			}
{RELEASE}		{ return TOKEN_RELEASE;		}
{UNTIL}			{ return TOKEN_UNTIL;		}
{WEAK_UNTIL}	{ return TOKEN_WEAK_UNTIL;	}
{FUTURE}		{ return TOKEN_FUTURE;		}
{GLOBALLY}		{ return TOKEN_GLOBALLY;	}
{NEXT}			{ return TOKEN_NEXT;		}
{WEAK_NEXT}		{ return TOKEN_WEAK_NEXT;	}
{NOT}			{ return TOKEN_NOT;			}

{WS}            { /* Skip blanks. */		}
{TRUE}			{ return TOKEN_TRUE;		}
{FALSE}			{ return TOKEN_FALSE;		}
{ID}			{ yylval->var_name = yytext; return TOKEN_VARIABLE; }

.				{ fprintf(stderr, "\033[31mERROR\033[0m: Unrecognized symbol: \033[34m%s\033[0m\n", yytext); exit(1); }
%%
/*
int yyerror(const char *msg) {
    fprintf(stderr,"Error:%s\n",msg);
	exit(1);
	return 0;
}*/

#ifdef __cplusplus
}
#endif
