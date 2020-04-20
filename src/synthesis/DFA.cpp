#include "DFA.h"
#include <string>
using namespace std;
using namespace boost;

//update test

DFA::DFA(Cudd* m){
  mgr = m;
    //ctor
}

DFA::~DFA()
{
    //dtor
    delete mgr;// = NULL;
}
void DFA::initialize(string filename, string partfile){
    //ctor
    read_from_file(filename);
    if(DFAflag == true) {
    cout<<"Number of DFA states: "<<nstates-1<<endl;
        nbits = state2bin(nstates-2).length();
        construct_bdd_new();
        cout << "Number of state variables: " << nbits << endl;
        read_partfile(partfile);

        initbv = new int[nbits];
        int temp = init;
        for (int i = nbits - 1; i >= 0; i--) {
            initbv[i] = temp % 2;
            temp = temp / 2;
        }
    }else{
        cout<<"DFA with no accepting traces"<<endl;
    }
}


void DFA::read_partfile(string partfile){
    ifstream f(partfile.c_str());
    vector<string> inputs;
    vector<string> outputs;
    string line;
    while(getline(f, line)){
        if(f.is_open()){
            if(strfind(line, "inputs")){
                split(inputs, line, is_any_of(" "));
                //print(inputs);
            }
            else if(strfind(line, "outputs")){
                split(outputs, line, is_any_of(" "));
                //print(outputs);
            }
            else{
                cout<<"read partfile error!"<<endl;
                cout<<partfile<<endl;
                cout<<line<<endl;
            }
		}
    }
    f.close();
    set<string> input_set;
    set<string> output_set;
    for(int i = 1; i < inputs.size(); i++){
        string c = boost::algorithm::to_upper_copy(inputs[i]);
        input_set.insert(c);
    }
    for(int i = 1; i < outputs.size(); i++){
        string c = boost::algorithm::to_upper_copy(outputs[i]);
        output_set.insert(c);
    }

    for(int i = 1; i < variables.size(); i++){
        if(input_set.find(variables[i]) != input_set.end())
            input.push_back(nbits+i-1);
        else if(output_set.find(variables[i]) != output_set.end())
            output.push_back(nbits+i-1);
        else if(variables[i] == "ALIVE")
            output.push_back(nbits+i-1);
        else
            cout<<"error: "<<variables[i]<<endl;
    }
    //print_int(input);
    //print_int(output);

}

void DFA::read_from_file(string filename){
ifstream f(filename.c_str());
	if(f.is_open()){
		bool flag = 0;
		string line;
        vector<int> tmp;
        vector <string> fields; //temporary varibale

		while(getline(f, line)){
            if(flag == 0){
                if(strfind(line, "number of variables")){
                    split(fields, line, is_any_of(" "));
                    nvars = stoi(fields[3]);
                    //cout<<nvars<<endl;
                }
                if(strfind(line, "variables") && !strfind(line, "number")){
                    split(variables, line, is_any_of(" "));

                }
                else if(strfind(line, "states")){
                    split(fields, line, is_any_of(" "));
                    nstates = stoi(fields[1]);
                   // cout<<nstates<<endl;
                }
                else if(strfind(line, "initial")){
                    split(fields, line, is_any_of(" "));
                    init = stoi(fields[1]);
                    //cout<<init<<endl;
                }
                else if(strfind(line, "bdd nodes")){
                    split(fields, line, is_any_of(" "));
                    nodes = stoi(fields[2]);
                    //cout<<nodes<<endl;
                }
                else if(strfind(line, "final")){
                    split(fields, line, is_any_of(" "));
                    int i = 1; // start at 1 to ignore "final" token
                    while(i < fields.size()){
                        if(fields[i] == "1") {
                            finalstates.push_back(i - 1);
                            DFAflag = true;
                        }
                        i = i + 1;
                    }
                    //print_int(finalstates);
                }
                else if(strfind(line, "behaviour")){
                    split(fields, line, is_any_of(" "));
                    int i = 1;
                    while(i < fields.size()){
                        behaviour.push_back(stoi(fields[i]));
                        i = i + 1;
                    }
                    //print_int(behaviour);
                }
                else if(strfind(line, "bdd:"))
                    flag = 1;
                else
                    continue;
            }
            else{
                if(strfind(line, "end"))
                    break;
                split(fields, line, is_any_of(" "));
                for(int i = 1; i < fields.size(); i++)
                    tmp.push_back(stoi(fields[i]));
                smtbdd.push_back(tmp);
                tmp.clear();
            }
		}

	}
	f.close();
    //print_vec(smtbdd);
}

void DFA::print(vector<auto> v)
{
  for (size_t n = 0; n < v.size(); n++)
    cout<< v[ n ] << " ";
  cout << endl;
}

bool DFA::strfind(string str, string target){
	size_t found = str.find(target);
	if(found != string::npos)
		return true;
	else
		return false;
}

string DFA::state2bin(int n){
    string res;
    while (n)
    {
        res.push_back((n & 1) + '0');
        n >>= 1;
    }

    if (res.empty())
        res = "0";
    else
        reverse(res.begin(), res.end());
    //cout<<res<<endl;
   return res;
}

//return positive or nagative bdd variable index
BDD DFA::var2bddvar(int v, int index){
    if(v == 0){
        return !bddvars[index];
    }
    else{
        return bddvars[index];
    }
}

void DFA::construct_bdd_new(){
    for(int i = 0; i < nbits+nvars; i++){
        BDD b = mgr->bddVar();
        bddvars.push_back(b);
        //dumpdot(b, to_string(i));
    }

    for(int i = 0; i < nbits; i++){
        BDD d = mgr->bddZero();
        res.push_back(d);
    }
    tBDD.resize(smtbdd.size());
    for(int i = 0; i < tBDD.size(); i++){
        if(tBDD[i].size() == 0){
            vbdd b = try_get(i);
        }
    }

    for(int i = 0; i < nbits; i++){
        for(int j = 1; j < nstates; j++){
            BDD tmp = mgr->bddOne();
            string bins = state2bin(j-1);
            int offset = nbits - bins.size();
            for(int m = 0; m < offset; m++){
                tmp = tmp * var2bddvar(0, m);
            }
            for(int m = 0; m < bins.size(); m++){
                tmp = tmp * var2bddvar(int(bins[m])-48, m + offset);
            }
            //dumpdot(tmp, "res-state "+to_string(behaviour[j])+to_string(i));
            //dumpdot(tBDD[behaviour[j]][i], "res-bdd "+to_string(behaviour[j])+to_string(i));
            tmp = tmp * tBDD[behaviour[j]][i];
            res[i] = res[i] + tmp;
            //dumpdot(res[i], "res "+to_string(i));
        }
        //dumpdot(res[i], "res "+to_string(i));
    }

    finalstatesBDD = mgr->bddZero();
    for(int i = 0; i < finalstates.size(); i++){
        BDD ac = state2bdd(finalstates[i]-1);
        finalstatesBDD += ac;
    }
}

BDD DFA::state2bdd(int s){
    string bin = state2bin(s);
    BDD b = mgr->bddOne();
    int nzero = nbits - bin.length();
    //cout<<nzero<<endl;
    for(int i = 0; i < nzero; i++){
        b *= !bddvars[i];
    }
    for(int i = 0; i < bin.length(); i++){
        if(bin[i] == '0')
            b *= !bddvars[i+nzero];
        else
            b *= bddvars[i+nzero];
    }
    return b;

}

vbdd DFA::try_get(int index){
    if(tBDD[index].size() != 0)
        return tBDD[index];
    vbdd b;
    if(smtbdd[index][0] == -1){
        int s = smtbdd[index][1];
        string bins = state2bin(s);
        for(int m = 0; m < nbits - bins.size(); m++){
            b.push_back(mgr->bddZero());
        }
        for(int i = 0; i < bins.size(); i++){
            if(bins[i] == '0')
                b.push_back(mgr->bddZero());
            else if(bins[i] == '1')
                b.push_back(mgr->bddOne());
            else
                cout<<"error binary state"<<endl;
        }
        tBDD[index] = b;
        return b;
    }
    else{
        int rootindex = smtbdd[index][0];
        int leftindex = smtbdd[index][1];
        int rightindex = smtbdd[index][2];
        BDD root = bddvars[rootindex+nbits];
        //dumpdot(root, "test");
        vbdd left = try_get(leftindex);
        //for(int l = 0; l < left.size(); l++)
           // dumpdot(left[l], "left"+to_string(l));
        vbdd right = try_get(rightindex);
        //for(int l = 0; l < left.size(); l++)
           // dumpdot(right[l], "right"+to_string(l));
        assert(left.size() == right.size());
        for(int i = 0; i < left.size(); i++){
            BDD tmp;
            tmp = root.Ite(right[i], left[i]);//Assume this is correct
            //dumpdot(tmp, "tmp");
            b.push_back(tmp);
        }
        tBDD[index] = b;
        return b;
    }
}

void DFA::dumpdot(BDD &b, string filename){
    FILE *fp = fopen(filename.c_str(), "w");
    CUDD::ADD a=b.Add();

    vector<CUDD::ADD> single(1);
    single[0] = a;
    this->mgr->DumpDot(single, NULL, NULL, fp);
    fclose(fp);
}














