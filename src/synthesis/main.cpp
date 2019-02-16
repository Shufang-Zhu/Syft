#include <iostream>
#include <string>
#include "syn.h"

using namespace std;
string get_DFAfile(string LTLFfile){
    string FOL = LTLFfile+".mona";
    string LTLF2FOL = "./ltlf2fol NNF "+LTLFfile+" >"+FOL;
    system(LTLF2FOL.c_str());

    string DFA = LTLFfile+".dfa";
    string FOL2DFA = "mona -u -xw "+FOL+" >"+DFA;
    system(FOL2DFA.c_str());

    return DFA;
}

int main(int argc, char ** argv){
    clock_t c_start = clock();
    auto t_start = chrono::high_resolution_clock::now();
    string filename;
    string partfile;
    string autfile;
    string starting_player;
    if(argc != 4){
        cout<<"Usage: ./Syft DFAfile Partfile Starting_player(0: system, 1: environment)"<<endl;
        return 0;
    }
    else{
        filename = argv[1];
        partfile = argv[2];
        starting_player = argv[3];
    }
    Cudd* mgr = new Cudd();
    autfile = get_DFAfile(filename);
    syn test(mgr, autfile, partfile);

    bool res = 0;
    std::unordered_map<unsigned, BDD> strategy;

    if(starting_player == "1")
        res = test.realizablity_env(strategy);
    else
        res = test.realizablity_sys(strategy);

    if(res)
        cout<<"realizable"<<endl;
    else
        cout<<"unrealizable"<<endl;
    clock_t c_end = clock();
    auto t_end = chrono::high_resolution_clock::now();
    std::cout << "Total CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n"
              << "Total wall clock time passed: "
              << std::chrono::duration<double, std::milli>(t_end-t_start).count()
              << " ms\n";
    return 0;

}
//solveeqn
