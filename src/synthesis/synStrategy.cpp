#include <iostream>
#include <string>
#include <algorithm>
#include "syn.h"

using namespace std;

void printVector(std::vector<int>& state){
  for(auto & s : state)
    std::cout<<s;
  std::cout<<std::endl;
}

void playGame(DFA& game, std::unordered_map<unsigned, BDD>& strategy){
  std::vector<int> cur(game.nbits);

  bool cont = true;
  std::vector<int> input;
  while (1){
    std::cout<<"State: ";
    int state_int = 0;
    for (int i=0; i<cur.size(); i++){
      state_int*=2;
      state_int += cur[i];
    }
    std::cout<<state_int<<std::endl;

    if (std::find(game.finalstates.begin(), game.finalstates.end(), state_int) != game.finalstates.end()){
      std::cout<<"robot wins"<<std::endl;
      return;
    }

    std::vector<int> zxy = cur;
    zxy.resize(game.nbits+game.nvars);

    // set the outputs from the strategy
    for (auto & o : game.output){
      std::cout<<"Value for output "<<game.variables[o-game.nbits+1]<<": ";
      if (strategy[o].Eval(&cur[0]).IsOne())
	zxy[o] = 1;
      else
	zxy[o] = 0;
      std::cout<<zxy[o]<<std::endl;
    }
    
    for (auto & i : game.input){
      int input;
      // +1 because the first field for variables is "variables:"
      std::cout<<"Value for input "<<game.variables[i-game.nbits+1]<<": ";
      std::cin>>input;
      if (input==0)
	zxy[i] = 0;
      else if (input==1)
	zxy[i] = 1;
      else
	return;
    }

    std::cout<<"Z-XY:";
    printVector(zxy);

    for (int i=0; i<game.nbits; i++){
      if (game.res[i].Eval(&zxy[0]).IsOne())
	cur[i] = 1;
      else
	cur[i] = 0;	
    }
				     
  }
      
}

int main(int argc, char ** argv){
    string filename;
    string partfile;
    string flag;
    if(argc != 3){
        cout<<"Usage: ./Syft DFAfile Partfile"<<endl;
        return 0;
    }
    else{
        filename = argv[1];
        partfile = argv[2];
    }
    syn test(filename, partfile);

    std::unordered_map<unsigned, BDD> S2O;
    bool res = test.realizablity(S2O);

    if(res){
        cout<<"realizable"<<endl;
	for (auto & pair : S2O){
	  std::cout<<pair.first<<": ";
	  test.printBDDSat(pair.second);
	}
	playGame(test.bdd, S2O);
    }
    else
        cout<<"unrealizable"<<endl;
    return 0;

}
//solveeqn
