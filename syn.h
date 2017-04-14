#ifndef SYN_H
#define SYN_H
#include "Common.h"
#include "DFA.h"
#include "InputFirst.h"
using namespace std;

class syn
{
    public:
        syn(string filename, string partfile);
        bool realizablity();
        virtual ~syn();
    protected:
    private:
        Cudd mgr;
        DFA bdd;
        int cur = 0;
        bool fixpoint();
        vector<BDD> W;
        vector<BDD> Wprime;
        string state2bin(int n);
        void initializer();
        BDD state2bdd(int s);
        int* state2bit(int n);
        int** outindex();
        void dumpdot(BDD &b, string filename);
        BDD prime(BDD orign);
        BDD univsyn();
        BDD existsyn();
        void strategy(vector<BDD>& S2O);
};

#endif // SYN_H
