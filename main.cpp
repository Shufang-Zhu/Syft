#include <iostream>
#include <string>
#include "syn.h"

using namespace std;

int main(int argc, char ** argv){
    string filename;
    string partfile;
    assert(argc == 3);
    filename = argv[1];
    partfile = argv[2];
    syn test(filename, partfile);

    bool res = test.realizablity();

    if(res)
        cout<<"realizable"<<endl;
    else
        cout<<"unrealizable"<<endl;
    return 0;

}
//solveeqn
