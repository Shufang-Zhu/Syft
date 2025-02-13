#ifndef DFA_GAME_SYNTHESIZER_H
#define DFA_GAME_SYNTHESIZER_H

#include "Quantification.h"
#include "SymbolicStateDfa.h"
#include "Synthesizer.h"
#include "Transducer.h"

namespace Syft {

/**
 * \brief A synthesizer for a game whose arena is a symbolic-state DFA.
 */
class DfaGameSynthesizer : public Synthesizer<SymbolicStateDfa> {
 protected:
  
  std::shared_ptr<VarMgr> var_mgr_;
  Player starting_player_;
  Player protagonist_player_;
  std::vector<int> initial_vector_;
  std::vector<CUDD::BDD> transition_vector_;
  std::unique_ptr<Quantification> quantify_independent_variables_;
  std::unique_ptr<Quantification> quantify_non_state_variables_;

  CUDD::BDD preimage(const CUDD::BDD& winning_states) const;

  CUDD::BDD project_into_states(const CUDD::BDD& winning_moves) const;
  
  std::unordered_map<int, CUDD::BDD> synthesize_strategy(
      const CUDD::BDD& winning_moves) const;
  
  bool includes_initial_state(const CUDD::BDD& winning_states) const;
  
 public:

  /**
   * \brief Construct a synthesizer for a given DFA game.
   *
   * The winning condition is unspecified and should be defined by the subclass.
   *
   * \param spec A symbolic-state DFA representing the game's arena.
   * \param starting_player The player that moves first each turn.
   */
  DfaGameSynthesizer(SymbolicStateDfa spec, Player starting_player, Player protagonist_player);


  /**
   * \brief Computes a winning strategy for the game.
   *
   * \return A transducer representing the winning strategy.
   */
  virtual SynthesisResult run()
      const override = 0;
};

}

#endif // DFA_GAME_SYNTHESIZER_H
