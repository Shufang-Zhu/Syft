#include "Common.h"

#ifndef DFA_H
#define DFA_H

//update test

using namespace std;

typedef vector<int> item;
typedef vector<BDD> vbdd;
class DFA
{
    public:
        DFA(Cudd* m);
	DFA();
        virtual ~DFA();
        void initialize(string filename, string partfile);
        //void initialize(string filename, string partfile, Cudd& manager);
        vector<item> bdd;
        void print_vec( vector <item> & v );
        void construct_bdd();
        void bdd2dot();
        void dumpdot(BDD &b, string filename);
	BDD state2bdd(int s);
        int nbits;
        int init;
	int* initbv;
        int nstates;

        int nvars;
        vector<int> finalstates;
	BDD finalstatesBDD;
        vector<BDD> res;
        vector<BDD> bddvars;
        vector<int> input;
        vector<int> output;

	vector<string> variables;
	
        //new bdd constructer
        void construct_bdd_new();

        Cudd* mgr;

	// domain-spec separate construction
	// Front need to be called before variable construction for domain
	// back is called after the components are constructed
	void construct_from_comp_front(string filename);
	void construct_from_comp_back(vbdd& S2S, vbdd& S2P, vbdd& Svars, vbdd& Ivars, vbdd& Ovars, std::vector<int> IS);

    protected:
    private:
		int nodes;
		vector<int> behaviour;
		//		vector<string> variables;
		vector<item> smtbdd;
        void read_from_file(string filename); //read the ltlf formula
        void read_partfile(string partfile); //read the partfile
        void get_bdd();
        void recur(int index, item tmp);
        void recur_left(int index, item tmp, int v);
        void recur_right(int index, item tmp, int v);
        void print( vector <string> & v );
        void print_int( vector <int> & v );
        bool strfind(string str, string target);
        void push_states(int i, item& tmp);
        string state2bin(int n);
        BDD var2bddvar(int v, int index);

        //new bdd constructer
        vector<vbdd> tBDD;
        vbdd try_get(int index);


};

#endif // DFA_H
