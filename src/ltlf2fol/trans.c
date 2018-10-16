/* 
 * 定义AST转化函数
 * File:   trans.c
 * Author: yaoyinbo
 *
 * Created on October 20, 2013, 3:32 PM
 */
#include "trans.h"

#include <stdlib.h>

ltl_formula *
getAST (const char *input)
{
  ltl_formula *formula;
  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if (yylex_init (&scanner))
    {
      // couldn't initialize
      return NULL;
    }

  state = yy_scan_string (input, scanner);
  if (yyparse (&formula, scanner))
    {
      // error parsing
      return NULL;
    }

  yy_delete_buffer (state, scanner);
  yylex_destroy (scanner);

  return formula;
}
