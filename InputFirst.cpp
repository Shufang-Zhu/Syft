#include "InputFirst.h"

using std::vector;
using std::unordered_map;

InputFirstSynthesis::InputFirstSynthesis(Cudd mgr, bool defaultValue)
{
	m = mgr;
	defaultConstant = defaultValue ? m.bddOne() : m.bddZero();
}

BDD InputFirstSynthesis::ifRec(int index, const BDD& b, unordered_map<DdNode*, BDD>& table)
{
	DdNode* node = b.getNode();

	unordered_map<DdNode*, BDD>::const_iterator it = table.find(node);

	if (it != table.end())
	{
		return it->second;
	}
	else
	{
		BDD result = ifStep(index, b, table);

		table.emplace(node, result);

		return result;
	}
}

BDD InputFirstSynthesis::ifStep(int index, const BDD& b, unordered_map<DdNode*, BDD>& table)
{
	if (b.IsZero() || b.IsOne())
	{
		return defaultConstant;
	}
	else
	{
		int j = b.NodeReadIndex();

		if (j < index)
		{
			BDD posCofactor = b.Compose(m.bddOne(), j);
			BDD negCofactor = b.Compose(m.bddZero(), j);

			BDD newPos = ifRec(index, posCofactor, table);
			BDD newNeg = ifRec(index, negCofactor, table);

			return m.bddVar(j).Ite(newPos, newNeg);
		}
		else if (j == index && b.Compose(defaultConstant, j).IsZero())
		{
			return !defaultConstant;
		}
		else
		{
			return defaultConstant;
		}
	}
}

BDD InputFirstSynthesis::ifRun(unsigned int index, const BDD& b)
{
	unordered_map<DdNode*, BDD> table;

	return ifStep(index, b, table);
}

unordered_map<unsigned int, BDD> InputFirstSynthesis::synthesize(BDD b, const BDD& outputCube)
{
	unordered_map<unsigned int, BDD> witness;

	for (unsigned int index : outputCube.SupportIndices())
	{
		witness[index] = ifRun(index, b);
		b = b.Compose(witness[index], index);
	}

	return witness;
}
