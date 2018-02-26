/*
 * added by Shufang ZHU on Jan 10th, 2017
 * translate ltlf formulas to fol, the input of MONA
*/

#include "ltlf2fol.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <set>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>

using namespace std;

void ltlf2fol (ltl_formula *root)
{
  int c = 1;
  trans_fol(root, 1, c);
  set<string> P = get_alphabet (root);
  if(!P.empty()){
    cout<<"var2 ALIVE, ";
    print_alphabet_no_comma(root);
    cout<<";"<<endl;
    // cout<<"allpos ALIVE;"<<endl;
    cout<<"0 in ALIVE;"<<endl;
  }
  
// cout<<"0 in ALIVE;"<<endl;
cout<<"all1 p: (p in ALIVE => "<<endl;
cout<<"         all1 l:( l <= p => l in ALIVE));"<<endl;
if(!P.empty()){
  set<string>::iterator it = P.begin ();
  while (it != P.end ()){
    cout<<"all1 p: (p in "<<up (*it)<<" => "<<endl;
    cout<<"         all1 l:( l <= p => l in ALIVE));"<<endl;
    it++;
  }
}
// cout<<"export(\"tmp.dfa\", F1(0, ALIVE";

  cout<<"F1(0, ALIVE";
  print_alphabet(root);
  // cout<<"));"<<endl;
  cout<<");"<<endl;

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

void trans_fol(ltl_formula* root, int t, int& c){
  int cur;
  switch(root->_type)
  {
        case eNOT:
          c = c + 1;
          cur = c;
          trans_fol(root->_right, cur, c);
          cout<<"# F"<<t<<" denotes "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE";
          printvars(root);
          cout<<") = "<<endl;
          cout<<"     ~F"<<cur<<" (p, ALIVE";
          print_alphabet(root->_right);
          cout<<")"<<endl;
          cout<<"       & (p in ALIVE)"<<endl;
          cout<<"       & all1 l:(l <= p => l in ALIVE);"<<endl<<endl;
          break;
        case eNEXT:
          c = c + 1;
          cur = c;
          trans_fol(root->_right, cur, c);
          cout<<"# F"<<t<<" denotes "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE";
          printvars(root);
          cout<<") = "<<endl;
          cout<<"     F"<<cur<<" (p+1, ALIVE ";
          print_alphabet(root->_right);
          cout<<");"<<endl<<endl;
          break;
        case eWNEXT:
          c = c + 1;
          cur = c;
          trans_fol(root->_right, cur, c);
          cout<<"# F"<<t<<" denotes "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE";
          printvars(root);
          cout<<") = "<<endl;
          cout<<"     F"<<cur<<" (p+1, ALIVE ";
          print_alphabet(root->_right);
          cout<<")"<<endl;
          cout<<"     |"<<endl;
          cout<<"     (p = max ALIVE);"<<endl<<endl;
          break;
        case eFUTURE:
          c = c + 1;
          cur = c;
          trans_fol(root->_right, cur, c);
          cout<<"# F"<<t<<" denotes "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE";
          printvars(root);
          cout<<") = "<<endl;
          cout<<"     ex1 q:((q >= p & q <= max ALIVE)"<<endl;
          cout<<"             & F"<<cur<<" (q, ALIVE";
          print_alphabet(root->_right);
          cout<<")"<<endl;
          cout<<"            );"<<endl<<endl;
          break;
        case eGLOBALLY:
          c = c + 1;
          cur = c;
          trans_fol(root->_right, cur, c);
          cout<<"# F"<<t<<" denotes "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE";
          printvars(root);
          cout<<") = "<<endl;
          cout<<"     all1 q:((p<= q & q <= max ALIVE)"<<endl;
          cout<<"             => F"<<cur<<" (q, ALIVE";
          print_alphabet(root->_right);
          cout<<")"<<endl;
          cout<<"            );"<<endl<<endl;
          break;
        case eUNTIL:
          c = c + 2;
          cur = c;
          trans_fol(root->_right, cur-1, c);
          trans_fol(root->_left, cur, c);
          cout<<"# F"<<t<<" denotes "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE ";
          printvars(root);
          cout<<") = "<<endl;
          cout<<"     ex1 q:( q >= p & q <= max ALIVE"<<endl;
          cout<<"             & F"<<cur-1<<" (q, ALIVE";
          print_alphabet(root->_right);
          cout<<")"<<endl;
          cout<<"             &"<<endl;
          cout<<"             all1 k:(p<= k & k < q) => F"<<cur<<" (k, ALIVE";
          print_alphabet(root->_left);
          cout<<")"<<endl;
          cout<<"            );"<<endl<<endl;
          break;
        case eRELEASE:
          c = c + 2;
          cur = c;
          trans_fol(root->_right, cur-1, c);
          trans_fol(root->_left, cur, c);
          cout<<"# F"<<t<<" denotes "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE ";
          printvars(root);
          cout<<") = "<<endl;
          cout<<"     (ex1 q:( q >= p"<<endl;
          cout<<             "& F"<<cur-1<<" (q, ALIVE";
          print_alphabet(root->_left);
          cout<<")"<<endl;
          cout<<"             &"<<endl;
          cout<<"             all1 k:(p<= k & k <= q) => F"<<cur<<" (k, ALIVE";
          print_alphabet(root->_right);
          cout<<")"<<endl;
          cout<<"            ));"<<endl<<endl;
          cout<<"      |"<<endl;
          cout<<"      (all1 q:(p<=q & q<=max ALIVE) => F"<<cur<<" (k, ALIVE";
          print_alphabet(root->_right);
          cout<<"));"<<endl;
          break;
        case eOR:
          c = c + 2;
          cur = c;
          trans_fol(root->_right, cur-1, c);
          trans_fol(root->_left, cur, c);
          cout<<"# F"<<t<<" denotes "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE";
          printvars(root);
          cout<<") = "<<endl;
          cout<<"     F"<<cur-1<<" (p, ALIVE";
          print_alphabet(root->_right);
          cout<<")"<<endl;
          cout<<"     |"<<endl;
          cout<<"     F"<<cur<<" (p, ALIVE";
          print_alphabet(root->_left);
          cout<<");"<<endl<<endl;
          break;
        case eAND:
          c = c + 2;
          cur = c;
          trans_fol(root->_right, cur-1, c);
          trans_fol(root->_left, cur, c);
          cout<<"# F"<<t<<" denotes "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE";
          printvars(root);
          cout<<") = "<<endl;
          cout<<"     F"<<cur-1<<" (p, ALIVE";
          print_alphabet(root->_right);
          cout<<")"<<endl;
          cout<<"     &"<<endl;
          cout<<"     F"<<cur<<" (p, ALIVE";
          print_alphabet(root->_left);
          cout<<");"<<endl<<endl;
          break;
        case eTRUE:
          cout<<"# F"<<t<<" denotes TRUE "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE) = "<<endl;
          cout<<"     true;"<<endl<<endl;
          break;
        case eFALSE:
          cout<<"# F"<<t<<" denotes TRUE "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE) = "<<endl;
          cout<<"     false;"<<endl<<endl;
          break;
        case 3:
          cout<<"# F"<<t<<" denotes Atom "<<to_string (root).c_str ()<<endl;
          cout<<"pred F"<<t<<" (var1 p, var2 ALIVE";
          printvars(root);
          cout<<") = "<<endl;
          cout<<"     p >= 0 & p in ";
          print_alphabet_no_comma(root);
          // cout<<";"<<endl;
          cout<<" & p in ALIVE"<<endl;
          cout<<"     & all1 l:(l <= p => l in ALIVE);"<<endl<<endl;
          break;
        default:
          break;
  }
}

string up(string a){
  return boost::to_upper_copy<std::string>(a);
}








