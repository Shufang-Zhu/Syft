/*
 * added by Jianwen LI on January 2nd, 2015
 * utility funtions for ltl_formula
*/

#include "utility.h"
#include <stdlib.h>
#include <stdio.h>
#include <set>
#include <sstream>
#include <assert.h>

ltl_formula *clone (ltl_formula *root)
{
  ltl_formula *res, *l, *r;
  if (root == NULL)
    return NULL;
  if (root->_var != NULL)
    res = create_var (root->_var);
  else
  {
    l = clone (root->_left);
    r = clone (root->_right);
    res = create_operation (root->_type, l, r);
  }
  return res;
}

std::string to_string (ltl_formula *root)
{
  std::string res = "";
  if (root == NULL)
    return res;
  if (root->_var != NULL)
    res = std::string (root->_var);
  else if (root->_type == eTRUE)
    res = std::string ("true");
  else if (root->_type == eFALSE)
    res = std::string ("false");
  else
    {
      res += "(";
      res += to_string (root->_left);
      switch (root->_type)
        {
        case eNOT:
          res += "! ";
          break;
        case eNEXT:
          res += "X ";
          break;
        case eWNEXT:
          res += "N ";
          break;
        case eGLOBALLY:
          res += "G ";
          break;
        case eFUTURE:
          res += "F ";
          break;
        case eUNTIL:
          res += " U ";
          break;
        case eWUNTIL:
          res += " W ";
          break;
        case eRELEASE:
          res += " V ";
          break;
        case eAND:
          res += " && ";
          break;
        case eOR:
          res += " || ";
          break;
        case eIMPLIES:
          res += " -> ";
          break;
        case eEQUIV:
          res += " <-> ";
          break;
        default:
          fprintf (stderr, "Error formula_utility!");
          exit (0);
        }
      res += to_string (root->_right);
      res += ")";
    }
  return res;
}


std::set<std::string> get_alphabet (ltl_formula *root)
{
  std::set<std::string> res, l, r;
  if (root->_var != NULL)
    res.insert (std::string (root->_var));
  else
  {
  if (root->_left != NULL)
  {
    l = get_alphabet (root->_left);
    res.insert (l.begin (), l.end ());
  }
  if (root->_right != NULL)
  {
    r = get_alphabet (root->_right);
    res.insert (r.begin (), r.end ());
  }
  }
  return res;
}

std::string string_of (int id)
{
  std::stringstream ss;
  ss << id;
  return ss.str ();
}

ltl_formula* expand (ltl_formula *root, int k)
{
  ltl_formula *res = NULL;
  if (root == NULL)
    return res;
  if (root->_var != NULL)
    res = clone (root);
  else if (root->_type == eTRUE)
    res = clone (root);
  else if (root->_type == eFALSE)
    res = clone (root);
  else
    {
      ltl_formula *l, *r, *res2;
      ltl_formula *tmp, *tmp1, *tmpr;
      switch (root->_type)
        {
        case eNOT:
          r = expand (root->_right, k);
          if (r->_type == eNOT)
            res = r->_right;
          else
            res = create_operation (eNOT, NULL, r);
          break;
        case eNEXT:
          r = expand (root->_right, k);
          res = create_operation (eNEXT, NULL, r);
          break;
        case eGLOBALLY:
          r = expand (root->_right, k);
          res = create_operation (eGLOBALLY, NULL, r);
          break;
        case eFUTURE:
          r = expand (root->_right, k);
          tmpr = clone(r);
          for(int i = 1; i < k; i++){
            tmpr = clone(tmpr);
            tmp = create_operation(eNEXT, NULL, tmpr);
            tmpr = create_operation(eOR, r, tmp);
          }
          res = tmpr;
          break;
        case eUNTIL:
          l = expand (root->_left, k);
          r = expand (root->_right, k);
          tmpr = r;
          for(int i = 1; i < k; i++){
            tmp = create_operation(eNEXT, NULL, tmpr);
            tmp1 = create_operation(eAND, l, tmp);
            tmpr = create_operation(eOR, r, tmp1);
          }
          res = tmpr;
          tmp = NULL;
          tmp1 = NULL;
          tmpr = NULL;
          break;
        case eRELEASE:
          l = expand (root->_left, k);
          r = expand (root->_right, k);
          res = create_operation (eRELEASE, l, r);
          break;
        case eAND:
          l = expand (root->_left, k);
          r = expand (root->_right, k);
          res = create_operation (eAND, l, r);
          break;
        case eOR:
          l = expand (root->_left, k);
          r = expand (root->_right, k);
          res = create_operation (eOR, l, r);
          break;
        case eIMPLIES:
          l = expand (root->_left, k);
          r = expand (root->_right, k);
          res = create_operation (eIMPLIES, l, r);
          delete l;
          delete r;
          break;
        case eEQUIV:
          l = expand (root->_left, k);
          r = expand (root->_right, k);
          res = create_operation (eEQUIV, l, r);
          delete res2;
          delete l;
          delete r;
          break;
        default:
          fprintf (stderr, "111Error formula!");
          exit (0);
        }


    }
  return res;
}

ltl_formula* bnf (ltl_formula *root)
{
  ltl_formula *res = NULL;
  if (root == NULL)
    return res;
  if (root->_var != NULL)
    res = clone (root);
  else if (root->_type == eTRUE)
    res = clone (root);
  else if (root->_type == eFALSE)
    res = clone (root);
  else
    {
      ltl_formula *l, *r, *res2;
      switch (root->_type)
        {
        case eNOT:
          r = bnf (root->_right);
          if (r->_type == eNOT)
            res = r->_right;
          else
            res = create_operation (eNOT, NULL, r);
          break;
        case eNEXT:
          r = bnf (root->_right);
          res = create_operation (eNEXT, NULL, r);
          break;
        case eWNEXT:
          r = bnf (root->_right);
          if (r->_type == eNOT)
            r = r->_right;
          else
            r = create_operation (eNOT, NULL, r);
          res = create_operation (eNEXT, NULL, r);
          res = create_operation (eNOT, NULL, res);
          break;
        case eGLOBALLY:
          r = bnf (root->_right);
          if (r->_type == eNOT)
            r = r->_right;
          else
            r = create_operation (eNOT, NULL, r);
          res = create_operation (eFUTURE, NULL, r);
          res = create_operation (eNOT, NULL, res);
          break;
        case eFUTURE:
          r = bnf (root->_right);
          res = create_operation (eFUTURE, NULL, r);
          break;
        case eUNTIL:
          l = bnf (root->_left);
          r = bnf (root->_right);
          res = create_operation (eUNTIL, l, r);
          break;
        case eWUNTIL:
          l = bnf (root->_left);
          r = bnf (root->_right);
          // res2 = l;
          if (l->_type == eNOT)
            l = l->_right;
          else
            l = create_operation (eNOT, NULL, l);
          res = create_operation (eFUTURE, NULL, l);
          res = create_operation (eNOT, NULL, res);
          r = create_operation(eUNTIL, bnf (root->_left), r);
          res = create_operation (eOR, res, r);
          break;
        case eRELEASE:
          l = bnf (root->_left);
          r = bnf (root->_right);
          if (l->_type == eNOT)
            l = l->_right;
          else
            l = create_operation (eNOT, NULL, l);
          if (r->_type == eNOT)
            r = r->_right;
          else
            r = create_operation (eNOT, NULL, r);
          res = create_operation (eUNTIL, l, r);
          res = create_operation (eNOT, NULL, res);
          break;
        case eAND:
          l = bnf (root->_left);
          r = bnf (root->_right);
          if (l->_type == eNOT)
            l = l->_right;
          else
            l = create_operation (eNOT, NULL, l);
          if (r->_type == eNOT)
            r = r->_right;
          else
            r = create_operation (eNOT, NULL, r);
          res = create_operation (eOR, l, r);
          res = create_operation (eNOT, NULL, res);
          break;
        case eOR:
          l = bnf (root->_left);
          r = bnf (root->_right);
          res = create_operation (eOR, l, r);
          break;
        case eIMPLIES:
          l = create_operation (eNOT, NULL, root->_left);
          r = create_operation (eOR, l, root->_right);
          res = bnf (r);
          delete r;
          delete l;
          break;
        case eEQUIV:
          l = create_operation (eIMPLIES, root->_left, root->_right);
          r = create_operation (eIMPLIES, root->_right, root->_left);
          res2 = create_operation (eAND, l, r);
          res = bnf (res2);
          delete res2;
          delete l;
          delete r;
          break;
        default:
          fprintf (stderr, "111Error formula!");
          exit (0);
        }
    }
  return res;
}

ltl_formula *nnf (ltl_formula *root)
{
  ltl_formula *res = NULL;
  if (root == NULL)
    return res;
  if (root->_var != NULL)
    res = clone (root);
  else if (root->_type == eTRUE)
    res = clone (root);
  else if (root->_type == eFALSE)
    res = clone (root);
  else
    {
      ltl_formula *l, *r, *res2;
      switch (root->_type)
        {
        case eNOT:
          res = nnf_not (root->_right);
          break;
        case eNEXT:
          r = nnf (root->_right);
          res = create_operation (eNEXT, NULL, r);
          break;
        case eWNEXT:
          r = nnf (root->_right);
          res = create_operation (eWNEXT, NULL, r);
          break;
        case eGLOBALLY:
          r = nnf (root->_right);
          res = create_operation (eGLOBALLY, NULL, r);
          break;
        case eFUTURE:
          r = nnf (root->_right);
          res = create_operation (eFUTURE, NULL, r);
          break;
        case eUNTIL:
          l = nnf (root->_left);
          r = nnf (root->_right);
          res = create_operation (eUNTIL, l, r);
          break;
        case eRELEASE:
          l = nnf (root->_left);
          r = nnf (root->_right);
          res = create_operation (eRELEASE, l, r);
          break;
        case eAND:
          l = nnf (root->_left);
          r = nnf (root->_right);
          res = create_operation (eAND, l, r);
          break;
        case eOR:
          l = nnf (root->_left);
          r = nnf (root->_right);
          res = create_operation (eOR, l, r);
          break;
        case eIMPLIES:
          l = create_operation (eNOT, NULL, root->_left);
          r = create_operation (eOR, l, root->_right);
          res = nnf (r);
          delete r;
          delete l;
          break;
        case eEQUIV:
          l = create_operation (eIMPLIES, root->_left, root->_right);
          r = create_operation (eIMPLIES, root->_right, root->_left);
          res2 = create_operation (eAND, l, r);
          res = nnf (res2);
          delete res2;
          delete l;
          delete r;
          break;
        default:
          fprintf (stderr, "Error formula!");
          exit (0);
        }
    }
  return res;
}

ltl_formula* nnf_not (ltl_formula *root)
{
  assert (root != NULL);
  ltl_formula *res = NULL;
  if (root->_var != NULL)
  {
    ltl_formula *temp = clone (root);
    res = create_operation (eNOT, NULL, temp);
    //delete temp;
  }
  else if (root->_type == eTRUE)
    res = create_operation (eFALSE, NULL, NULL);
  else if (root->_type == eFALSE)
    res = create_operation (eTRUE, NULL, NULL);
  else
    {
      ltl_formula *l, *r, *res2;
      switch (root->_type)
        {
        case eNOT:
          res = nnf (root->_right);
          break;
        case eNEXT:
          r = nnf_not (root->_right);
          res = create_operation (eWNEXT, NULL, r);
          break;
        case eWNEXT:
          r = nnf_not (root->_right);
          res = create_operation (eNEXT, NULL, r);
          break;
        case eGLOBALLY:
          r = nnf_not (root->_right);
          res = create_operation (eFUTURE, NULL, r);
          break;
        case eFUTURE:
          r = nnf_not (root->_right);
          res = create_operation (eGLOBALLY, NULL, r);
          break;
        case eUNTIL:
          l = nnf_not (root->_left);
          r = nnf_not (root->_right);
          res = create_operation (eRELEASE, l, r);
          break;
        case eRELEASE:
          l = nnf_not (root->_left);
          r = nnf_not (root->_right);
          res = create_operation (eFUTURE, l, r);
          break;
        case eAND:
          l = nnf_not (root->_left);
          r = nnf_not (root->_right);
          res = create_operation (eOR, l, r);
          break;
        case eOR:
          l = nnf_not (root->_left);
          r = nnf_not (root->_right);
          res = create_operation (eAND, l, r);
          break;
        case eIMPLIES:
          l = create_operation (eNOT, NULL, root->_left);
          r = create_operation (eOR, l, root->_right);
          res = nnf_not (r);
          delete r;
          delete l;
          break;
        case eEQUIV:
          l = create_operation (eIMPLIES, root->_left, root->_right);
          r = create_operation (eIMPLIES, root->_right, root->_left);
          res2 = create_operation (eAND, l, r);
          res = nnf_not (res2);
          delete res2;
          delete l;
          delete r;
          break;
        default:
          fprintf (stderr, "Error formula!");
          exit (0);
        }
    }
  return res;
}





