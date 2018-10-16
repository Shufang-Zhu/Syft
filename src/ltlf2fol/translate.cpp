#include "ltlf2fol.h"
#include <assert.h>
#include <fstream>

#define MAXN 1000000
char in[MAXN];

using namespace std;

int main (int argc, char ** argv)
{
  
		string StrLine;
		std::string input;
    std::string format;
		assert(argc == 3);
		input = argv[2];
    format = argv[1];
		ifstream myfile(input);
		if (!myfile.is_open()) //判断文件是否存在及可读
		{
		    printf("unreadable file!");
		    return -1;
		}
		getline(myfile, StrLine);
		myfile.close(); //关闭文件
    strcpy (in, StrLine.c_str());
    printf ("#%s\n", in);
    
    ltl_formula *root = getAST (in);
    ltl_formula *bnfroot = bnf (root);
    ltl_formula *newroot;
    printf ("#%s\n", to_string (bnfroot).c_str ());
    if(format == "NNF"){
      printf ("#NNF format\n");
      newroot = nnf (bnfroot) ;   
    }
    else{
      printf ("#BNF format\n");
      newroot = bnfroot;
    }
    
    printf ("#%s\n", to_string (newroot).c_str ());
    ltlf2fol (newroot);
    
    

    // printf ("%s\n", res.c_str ());
    destroy_formula (root);
    destroy_formula (newroot);
    //destroy_formula (nnfroot);
}
