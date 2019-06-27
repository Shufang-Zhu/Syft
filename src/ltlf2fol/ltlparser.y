%{
/*
 * ltlparser.y file
 * To generate the parser run: "bison ltlparser.y"
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "ltl_formula.h"
#include "ltlparser.h"
#include "ltllexer.h"

int yyerror(ltl_formula **formula, yyscan_t scanner, const char *msg) {
	fprintf (stderr, "\033[31mERROR\033[0m: %s\n", msg);
	exit(1);
	return 0;
}

%}

%code requires {
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
}
 
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { ltl_formula **formula }
%parse-param { yyscan_t scanner }

%union {
	char* var_name;
	ltl_formula *formula;
}

%left TOKEN_EQUIV
%right TOKEN_IMPLIES
%left TOKEN_OR
%left TOKEN_AND
%left TOKEN_RELEASE
%left TOKEN_UNTIL
%left TOKEN_WEAK_UNTIL
%right TOKEN_FUTURE
%right TOKEN_GLOBALLY
%right TOKEN_NEXT
%right TOKEN_WEAK_NEXT
%right TOKEN_NOT

%token TOKEN_TRUE
%token TOKEN_FALSE
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token <var_name> TOKEN_VARIABLE
 
%type <formula> expr

%%

input
	: expr { *formula = $1; }
	;

expr
	: TOKEN_VARIABLE	{ $$ = create_var($1);	}
	| TOKEN_TRUE		{ $$ = create_operation( eTRUE, NULL, NULL );	}
	| TOKEN_FALSE		{ $$ = create_operation( eFALSE, NULL, NULL );	}
	| TOKEN_LPAREN expr TOKEN_RPAREN { $$ = $2;	}
	| expr TOKEN_EQUIV expr		{ $$ = create_operation( eEQUIV, $1, $3 );		}
	| expr TOKEN_IMPLIES expr	{ $$ = create_operation( eIMPLIES, $1, $3 );	}
	| expr TOKEN_OR expr		{ $$ = create_operation( eOR, $1, $3 );			}
	| expr TOKEN_AND expr		{ $$ = create_operation( eAND, $1, $3 );		}
	| expr TOKEN_RELEASE expr	{ $$ = create_operation( eRELEASE, $1, $3 );	}
	| expr TOKEN_UNTIL expr		{ $$ = create_operation( eUNTIL, $1, $3 );		}
	| expr TOKEN_WEAK_UNTIL expr		{ $$ = create_operation( eWUNTIL, $1, $3 );		}
	| TOKEN_FUTURE expr			{ $$ = create_operation( eFUTURE, NULL, $2 );	}
	| TOKEN_GLOBALLY expr		{ $$ = create_operation( eGLOBALLY, NULL, $2 );	}
	| TOKEN_NEXT expr			{ $$ = create_operation( eNEXT, NULL, $2 );		}
	| TOKEN_WEAK_NEXT expr			{ $$ = create_operation( eWNEXT, NULL, $2 );		}
	| TOKEN_NOT expr			{ $$ = create_operation( eNOT, NULL, $2 );		}
	;

%%

#ifdef __cplusplus
}
#endif
