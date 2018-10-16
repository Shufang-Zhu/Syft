/*
 * added by Shufang ZHU on Jan 10th, 2017
 * translate ltlf formulas to fol, the input of MONA
*/

#include "ltlf2fol.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <set>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>

using namespace std;
#define MAXN 1000000


void ltlf2fol (ltl_formula *root)
{
  int c = 1;
  string res;
  
  set<string> P = get_alphabet (root);
  if(!P.empty()){
    // cout<<"var2 ALIVE, ";
    cout<<"m2l-str;"<<endl;
    cout<<"var2 ";
    print_alphabet_no_comma(root);
    cout<<";"<<endl;
    // cout<<"allpos ALIVE;"<<endl;
    // cout<<"0 in ALIVE;"<<endl;
  }
  
  res = trans_fol(root, 0, c);
  cout<<res<<";"<<endl;
  

}

void print_alphabet_no_comma (ltl_formula* root){
  set<string> P = get_alphabet (root);
  if(!P.empty()){
    set<string>::iterator it = P.begin ();
    // cout<<toupper(*it);
    cout<<up(*it);
    it++;
    while (it != P.end ()){
      cout<<", "<<up(*it);
      it++;
    }
  }
}

string alphabet_no_comma (ltl_formula* root){
  string res = "";
  set<string> P = get_alphabet (root);
  if(!P.empty()){
    set<string>::iterator it = P.begin ();
    // cout<<toupper(*it);
    res += up(*it);
    it++;
    while (it != P.end ()){
      res += ", "+up(*it);
      it++;
    }
  }
  return res;
}

void print_alphabet (ltl_formula* root){
  set<string> P = get_alphabet (root);
  if(!P.empty()){
    set<string>::iterator it = P.begin ();
    // cout<<toupper(*it);
    cout<<", "<<up(*it);
    it++;
    while (it != P.end ()){
      cout<<", "<<up(*it);
      it++;
    }
  }
}

void print_alphabet_not (ltl_formula* root){
  set<string> P = get_alphabet (root);
  if(!P.empty()){
    set<string>::iterator it = P.begin ();
    // cout<<toupper(*it);
    cout<<", "<<up(*it)<<"\\{p}";
    it++;
    while (it != P.end ()){
      cout<<", "<<up(*it);
      it++;
    }
  }
}

void printvars (ltl_formula* root){
  set<string> P = get_alphabet (root);
  if(!P.empty()){
    set<string>::iterator it = P.begin ();
    // cout<<toupper(*it);
    cout<<", var2 "<<up(*it);
    it++;
    while (it != P.end ()){
      cout<<", var2 "<<up(*it);
      it++;
    }
  }
}

string trans_fol(ltl_formula* root, int t, int& c){
  string curs, ts;
  string exs, alls;
  string res;
  int cur;
  switch(root->_type)
  {
        case eNOT:
          res = "~(";
          res += trans_fol(root->_right, t, c);
          res += ")";
          break;
        case eNEXT:
          exs = "x"+to_string(t+1);
          if (t == 0)
            ts = to_string(t);
          else
            ts = "x"+to_string(t);
          res = "(ex1 "+exs+": ("+exs+"="+ts+"+1 & (";
          res += trans_fol(root->_right, t+1, c);
          res += ")))";
          break;
        case eWNEXT:
          exs = "x"+to_string(t+1);
          if (t == 0)
            ts = to_string(t);
          else
            ts = "x"+to_string(t);
          res = "((ex1 "+exs+": ("+exs+"="+ts+"+1 & (";
          res += trans_fol(root->_right, t+1, c);
          res += "))) | ("+ts+" = max $))";
          break;
        case eFUTURE:
          exs = "x"+to_string(t+1);
          if (t == 0)
            ts = to_string(t);
          else
            ts = "x"+to_string(t);
          res = "(ex1 "+exs+": ("+ts+" <= "+exs+" & (";
          res += trans_fol(root->_right, t+1, c);
          res += ")))";
          break;
        case eGLOBALLY:
          alls = "x"+to_string(t+1);
          if (t == 0)
            ts = to_string(t);
          else
            ts = "x"+to_string(t);
          res = "(all1 "+alls+": (("+ts+" <= "+alls+") => (";
          res += trans_fol(root->_right, t+1, c);
          res += ")))";
          break;
        case eUNTIL:
          exs = "x"+to_string(t+1);
          alls = "x"+to_string(t+2);
          if (t == 0)
            ts = to_string(t);
          else
            ts = "x"+to_string(t);
          res = "(ex1 "+exs+": ("+ts+" <= "+exs+" & (";
          res += trans_fol(root->_right, t+1, c);
          res += ") & (all1 "+alls+": ("+ts+" <= "+alls+" & "+alls;
          res += " < "+exs+" => (";
          res += trans_fol(root->_left, t+2, c);
          res += ")))))";
          break;
        case eRELEASE: //New
          exs = "x"+to_string(t+1);
          alls = "x"+to_string(t+2);
          if (t == 0)
            ts = to_string(t);
          else
            ts = "x"+to_string(t);
          res = "((ex1 "+exs+": ("+ts+" <= "+exs+" & (";
          res += trans_fol(root->_left, t+1, c);
          res += ") & (all1 "+alls+": ("+ts+" <= "+alls+" & "+alls;
          res += " <= "+exs+" => (";
          res += trans_fol(root->_right, t+2, c);
          res += ")))))";
          res += "| (all1 "+alls+": (("+ts+" <= "+alls+" & "+alls+" <= max $) => (";
          res += trans_fol(root->_right, t+2, c);
          res += "))))";
          break;
        case eOR:
          res += "(("+trans_fol(root->_right, t, c);
          res += ") | (";
          res += trans_fol(root->_left, t, c)+"))";
          break;
        case eAND:
          res += "(("+trans_fol(root->_right, t, c);
          res += ") & (";
          res += trans_fol(root->_left, t, c)+"))";
          break;
        case eTRUE:
          res += "(true)";
          break;
        case eFALSE:
          res += "(false)";
          break;
        case 3:
          if (t == 0)
            ts = "("+to_string(t);
          else
            ts = "(x"+to_string(t);
          res += ts+" in ";
          res += alphabet_no_comma(root);
          res +=")";
          break;
        default:
          break;
  }
  // cout<<res<<endl;
  return res;
}

string up(string a){
  return boost::to_upper_copy<std::string>(a);
}



char in[MAXN];

int main (int argc, char ** argv)
{
  
		string StrLine;
		std::string input;
    std::string format;
		if(argc != 3){
        cout<<"Usage: ./ltlf2fol format(NNF, BNF) filename"<<endl;
        return 0;
    }
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






