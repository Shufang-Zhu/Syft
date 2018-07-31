#include "DFA.h"
using namespace std;
using namespace boost;

//update test

DFA::DFA(){
    //ctor
}

DFA::~DFA()
{
    //dtor
    delete mgr;// = NULL;
}
void DFA::initialize(string filename, string partfile){
    //ctor
    mgr = new Cudd();
    read_from_file(filename);
    nbits = state2bin(nstates-1).length();

    //get_bdd();
    //print_vec(bdd);
    //construct_bdd();
    construct_bdd_new();
    read_partfile(partfile);
}


void DFA::read_partfile(string partfile){
    ifstream f(partfile);
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
    for(int i = 1; i < inputs.size(); i++)
        input_set.insert(inputs[i]);
    for(int i = 1; i < outputs.size(); i++)
        output_set.insert(outputs[i]);

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
ifstream f(filename);
	if(f.is_open()){
		bool flag = 0;
		string line;
		item tmp; //item: vector<int>
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
                        if(fields[i] == "1")
                            finalstates.push_back(i-1);
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

void DFA::construct_from_components(vbdd& S2S, vbdd& S2P, vbdd& Svars, vbdd& Ivars, vbdd& Ovars, string filename){
  // Construct the BDD for the spec portion
  read_from_file(filename);
  nbits = state2bin(nstates-1).length();
  construct_bdd_new();

  // substitute P from res, first create a substitution/projection vector, then use the batch substitution function
  vbdd subnProj;
  // task dfa states
  for (int i=0; i<nbits; i++){
    subnProj.push_back(bddvars[i]);
  }
  // propositions (aka task variables)
  assert(S2S.size()==nvars);
  for (int i=0; i<nvars; i++){
    // TODO: We need to make sure the variables line up!!!!
    subnProj.push_back(S2S[i]);
  }
  // domain state variables
  for (auto & v : Svars){
    subnProj.push_back(v);
  }
  // human action variables
  for (auto & v : Ivars){
    subnProj.push_back(v);
  }
  // robot action variables
  for (auto & v : Ovars){
    subnProj.push_back(v);
  }
  for (int i=0; i<res.size(); i++){
    res[i] = res[i].VectorCompose(S2P);
  }  

  // append S2S to res
  res.insert(res.end(), S2S.begin(), S2S.end());
  
  // fix the other variables (nvars, nbits, init, etc)
  bddvars.insert(bddvars.end(), Svars.begin(), Svars.end());
  bddvars.insert(bddvars.end(), Ivars.begin(), Ovars.end());
  bddvars.insert(bddvars.end(), Ivars.begin(), Ovars.end());
  nbits = nbits + nvars + Svars.size(); // TODO: can we eliminate the propositions completely?
  nvars = Ivars.size() + Ovars.size();
  // TODO: make init and final states formulas
  input.clear();
  output.clear();
  for (int i=0; i<Ivars.size(); i++){
    input.push_back(i+nbits);
  }
  for (int i=0; i<Ovars.size(); i++){
    output.push_back(i+nbits+Ivars.size());
  }

}

void DFA::recur(int index, item tmp){
    if(smtbdd[index][0] == -1){
        while(tmp.size() < nbits + nvars)
            tmp.push_back(2); //0:false 1:true 2:don't care
        push_states(smtbdd[index][1], tmp);
        bdd.push_back(tmp);
        //print_vec(bdd);
        tmp.clear();
    }
    else{
        int left = smtbdd[index][1];
        int right = smtbdd[index][2];
        int v = smtbdd[index][0];
        recur_left(left, tmp, v);
        recur_right(right, tmp, v);
    }
}

void DFA::recur_left(int index, item tmp, int v){
	while(tmp.size() < (nbits + v))
		tmp.push_back(2); //0:false 1:true 2:don't care
	tmp.push_back(0);
	recur(index, tmp);
}

void DFA::recur_right(int index, item tmp, int v){
	while(tmp.size() < (nbits+v))//
		tmp.push_back(2); //0:false 1:true 2:don't care
	tmp.push_back(1);
	recur(index, tmp);
}

void DFA::get_bdd(){
    for(int i = 0; i < nstates; i++){
        int index = behaviour[i];
        item tmp;
        push_states(i, tmp);
        recur(index, tmp);
    }
}

void DFA::push_states(int i, item & tmp){
    string s = state2bin(i);
    for(int j = 0; j < nbits - s.length(); j++)
        tmp.push_back(0);
    for(int j = 0; j < s.length(); j++){
        int t = int(s[j]) - 48;
        tmp.push_back(t);
    }
}

void DFA::print( vector <string> & v )
{
  for (size_t n = 0; n < v.size(); n++)
    cout << v[ n ] << " ";
  cout << endl;
}

void DFA::print_int( vector <int> & v )
{
  for (size_t n = 0; n < v.size(); n++)
    cout<< v[ n ] << " ";
  cout << endl;
}

void DFA::print_vec(vector<item> & v){
    for (size_t n = 0; n < v.size(); n++)
        print_int(v[n]);
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

void DFA::bdd2dot(){
    for(int i = 0; i < res.size(); i++){
        string filename = to_string(i);
        dumpdot(res[i], filename);
    }
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


void DFA::construct_bdd(){

    for(int i = 0; i < nbits+nvars; i++){
        BDD b = mgr->bddVar();
        bddvars.push_back(b);
    }

    for(int i = 0; i < nbits; i++){
        BDD d = mgr->bddZero();
        res.push_back(d);
    }
    //cout<<"bddvars.length: "<<bddvars.size()<<endl;

    for(int i = 0; i < bdd.size(); i++){
        for(int j = 0; j < nbits; j++){
            if(bdd[i][nbits+nvars+j] == 1){
                BDD tmp = mgr->bddOne();
                for(int m = 0; m < nbits+nvars; m++)
                {
                    if(bdd[i][m] != 2){
                        tmp *= var2bddvar(bdd[i][m], m);
                    }
                }
                res[j] += tmp;
            }
        }
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
            //dumpdot(tBDD[i][0], "test");
            vbdd b = try_get(i);
        }
    }


    for(int i = 0; i < nbits; i++){
        for(int j = 0; j < nstates; j++){
            BDD tmp = mgr->bddOne();
            string bins = state2bin(j);
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
         //dumpdot(root, "res "+to_string(i));
    }


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
    vector<BDD> single(1);
    single[0] = b;
	this->mgr->DumpDot(single, NULL, NULL, fp);
	fclose(fp);
}














