#ifndef TRANSDUCER_H
#define TRANSDUCER_H

#include <memory>
#include <unordered_map>
#include <vector>

#include <cuddObj.hh>

#include "Player.h"
#include "VarMgr.h"

namespace Syft {

/**
 * \brief A symbolic tranducer representing a winning strategy for a game.
 *
 * May be either a Moore or Mealy machine.
 */
class Transducer {
 private:

  std::shared_ptr<VarMgr> var_mgr_;

  std::vector<int> initial_vector_;
  std::unordered_map<int, CUDD::BDD> output_function_;
  std::vector<CUDD::BDD> transition_function_;
  Player starting_player_;
  Player protagonist_player_;

 public:

  Transducer(std::shared_ptr<VarMgr> var_mgr,
             std::vector<int> initial_vector,
             std::unordered_map<int, CUDD::BDD> output_function,
             std::vector<CUDD::BDD> transition_function,
             Player starting_player,
             Player protagonist_player = Player::Agent);

  /**
   * \brief Saves the output function of the transducer in a .dot file.
   */
  void dump_dot(const std::string& filename) const;
};

}

#endif // TRANSDUCER_H
