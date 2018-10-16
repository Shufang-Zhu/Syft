/* 
 * parser部分主函数
 * File:   main.c
 * Author: yaoyinbo
 *
 * Created on October 20, 2013, 9:11 PM
 */

#include "ltl_formula.h"
#include "trans.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#define MAXN 40000

using namespace std;

char in[MAXN];

int
main (int argc, char** argv)
{
	  FILE *fp; 
      char* filename = argv[1];
      char StrLine[MAXN];             //每行最大读取的字符数
      if((fp = fopen(filename,"r")) == NULL) //判断文件是否存在及可读
      { 
          printf("error!"); 
          return -1; 
      } 
  
      while (!feof(fp)) 
      { 
          fgets(StrLine,MAXN,fp);  //读取一行
      } 
      fclose(fp);                     //关闭文件
      ltl_formula *formula = getAST(StrLine);
      print_formula(formula);
      puts("");
      destroy_formula(formula);
 //   }

  return (EXIT_SUCCESS);
}

