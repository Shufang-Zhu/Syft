/*
 * added by Jianwen LI on January 2nd, 2015
 * utility funtions for ltl_formula
*/

#ifndef UTILITY_H
#define UTILITY_H

#include "ltl_formula.h"
#include "trans.h"
#include <string>
#include <set>


/*
 * get the bnf of the input formula
*/
ltl_formula* bnf (ltl_formula*);

/*
 * expand the input formula
*/
ltl_formula* expand (ltl_formula*, int);

/*
 * get the nnf of the input formula
*/
ltl_formula* nnf (ltl_formula*);

/*
 * for not operater in nnf
*/
ltl_formula* nnf_not (ltl_formula*);

/*
 * get the string of input ltl formula
*/
std::string to_string (ltl_formula *);

/*
 *clone an ltl formula
*/
ltl_formula* clone (ltl_formula*);

/*
 *get the alphabet of ltl formula
*/
std::set<std::string> get_alphabet (ltl_formula*);

/*
 *convert an integer to string
*/
std::string string_of (int);


#endif
