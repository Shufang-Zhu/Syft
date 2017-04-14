#include "Common.h"

#include <vector>
#include <unordered_map>

class InputFirstSynthesis
{
	Cudd m;
	BDD defaultConstant;

	BDD ifStep(int index, const BDD& b, std::unordered_map<DdNode*, BDD>& table);
	BDD ifRec(int index, const BDD& b, std::unordered_map<DdNode*, BDD>& table);
	BDD ifRun(unsigned int index, const BDD& b);
	
	public:
	
	InputFirstSynthesis(Cudd mgr, bool defaultValue = true);
	std::unordered_map<unsigned int, BDD> synthesize(BDD b, const BDD& outputCube);
};
