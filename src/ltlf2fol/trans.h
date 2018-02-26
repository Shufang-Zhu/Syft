/* 
 * 将表达式字符串转化为AST结构
 * File:   trans.h
 * Author: yaoyinbo
 *
 * Created on October 20, 2013, 3:28 PM
 */

#ifndef TRANS_H
#define	TRANS_H

#include "ltl_formula.h"
#include "ltlparser.h"
#include "ltllexer.h"

#ifdef	__cplusplus
extern "C"
{
#endif

  ltl_formula *getAST (const char *input);

#ifdef	__cplusplus
}
#endif

#endif	/* TRANS_H */

