#include "Quantification.h"

#include <memory>

namespace Syft {

CUDD::BDD NoQuantification::apply(const CUDD::BDD& bdd) const {
  return bdd;
}

Forall::Forall(CUDD::BDD universal_variables)
  : universal_variables_(std::move(universal_variables))
{}

CUDD::BDD Forall::apply(const CUDD::BDD& bdd) const {
  return bdd.UnivAbstract(universal_variables_);
}

Exists::Exists(CUDD::BDD existential_variables)
  : existential_variables_(std::move(existential_variables))
{}

CUDD::BDD Exists::apply(const CUDD::BDD& bdd) const {
  return bdd.ExistAbstract(existential_variables_);
}

ForallExists::ForallExists(CUDD::BDD universal_variables,
			   CUDD::BDD existential_variables)
  : forall_(std::move(universal_variables))
  , exists_(std::move(existential_variables))
{}

CUDD::BDD ForallExists::apply(const CUDD::BDD& bdd) const {
  return forall_.apply(exists_.apply(bdd));
}

ExistsForall::ExistsForall(CUDD::BDD existential_variables,
                           CUDD::BDD universal_variables)
        : exists_(std::move(existential_variables))
        , forall_(std::move(universal_variables))
{}

CUDD::BDD ExistsForall::apply(const CUDD::BDD& bdd) const {
    return exists_.apply(forall_.apply(bdd));
}

}
