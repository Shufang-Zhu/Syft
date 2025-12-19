//
// Created by Shufang Zhu on 01/12/2025.
//
#include <bddx.h>

// C wrapper function that just forwards the call
int bdd_setbddpairs_c(bddPair* pair, int* varvec, const bdd* bddvec, int size) {
  return bdd_setbddpairs(pair, varvec, bddvec, size);
}
