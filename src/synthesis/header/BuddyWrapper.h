//
// Created by Shufang Zhu on 01/12/2025.
//

#pragma once

// Include the main BuDDy header normally — no extern "C" needed
#include <bddx.h>

// Declare the C-style wrapper for bdd_setbddpairs defined in bdd_link_shim.c
#ifdef __cplusplus
extern "C" {
#endif

  // This function will be defined in a separate C file (bdd_link_shim.c)
  int bdd_setbddpairs_c(s_bddPair* pair, int* varvec, const bdd* bddvec, int size);

#ifdef __cplusplus
}
#endif