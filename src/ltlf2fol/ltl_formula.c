/* 
 * 实现LTL Formula相关函数
 * File:   ltl_formula.c
 * Author: yaoyinbo
 *
 * Created on October 20, 2013, 20:19 PM
 */

#include "ltl_formula.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * 动态申请Expression类型
 * @return 
 */
static ltl_formula *
allocate_ltl ()
{
  ltl_formula *ret = (ltl_formula *) malloc (sizeof *ret);

  if (ret == NULL)
    { //內存申请失败
      fprintf (stderr, "Memory error\n");
      exit (1);
      return NULL;
    }

  memset (ret, 0, sizeof (ltl_formula));

  return ret;
}

/**
 * 动态申请字符串并赋值
 * @param str
 * @return 
 */
static char *
allocate_cstr (const char *str)
{
  char *s = (char *) malloc ((strlen (str) + 1) * sizeof (char));

  if (s == NULL)
    { //內存申请失败
      fprintf (stderr, "Memory error\n");
      exit (1);
      return NULL;
    }

  strcpy (s, str);
  return s;
}

/**
 * 构建变量表达式
 * @param var 变量名
 * @return 
 */
ltl_formula *
create_var (const char *var)
{
  ltl_formula *ret = allocate_ltl ();

  ret->_type = eLITERAL;
  ret->_var = allocate_cstr (var);

  return ret;
}

/**
 * 构建操作表达式
 * @param type 
 * @param left
 * @param right
 * @return 
 */
ltl_formula *
create_operation (EOperationType type, ltl_formula *left, ltl_formula *right)
{
  ltl_formula *ret = allocate_ltl ();

  ret->_type = type;
  ret->_left = left;
  ret->_right = right;

  return ret;
}

/**
 * 打印以root为根的表达式树
 * @param root
 */
void
print_formula (ltl_formula *root)
{
  if (root == NULL) return;
  if (root->_var != NULL)
    printf ("%s", root->_var);
  else if (root->_type == eTRUE)
    printf ("true");
  else if (root->_type == eFALSE)
    printf ("false");
  else
    {
      printf ("(");
      print_formula (root->_left);
      switch (root->_type)
        {
        case eNOT:
          printf ("! ");
          break;
        case eNEXT:
          printf ("X ");
          break;
        case eWNEXT:
          printf ("N ");
          break;
        case eWUNTIL:
          printf (" W ");
          break;
        case eGLOBALLY:
          printf ("[] ");
          break;
        case eFUTURE:
          printf ("<> ");
          break;
        case eUNTIL:
          printf (" U ");
          break;
        case eRELEASE:
          printf (" R ");
          break;
        case eAND:
          printf (" && ");
          break;
        case eOR:
          printf (" || ");
          break;
        case eIMPLIES:
          printf (" -> ");
          break;
        case eEQUIV:
          printf (" <-> ");
          break;
        default:
          fprintf (stderr, "Error formula!");
        }
      print_formula (root->_right);
      printf (")");
    }
}

/**
 * 销毁以root为根的表达式树
 * @param root
 */
void
destroy_formula (ltl_formula *root)
{
  if (root == NULL) return;

  destroy_formula (root->_left);
  destroy_formula (root->_right);

  destroy_node (root);
}

/**
 * 销毁节点node，不会递归销毁子节点
 * @param node
 */
void
destroy_node (ltl_formula *node)
{
  if (node->_var != NULL) free (node->_var), node->_var = NULL;
  free (node), node = NULL;
}
