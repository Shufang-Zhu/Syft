#ifndef EXPLICIT_STATE_DFA_H
#define EXPLICIT_STATE_DFA_H

#include <exception>
#include <istream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "VarMgr.h"
#include "ExplicitStateDfaMona.h"

namespace Syft {

/**
 * \brief A DFA with explicit states and symbolic transitions.
 */
class ExplicitStateDfa {
 private:

  std::shared_ptr<VarMgr> var_mgr_;

  size_t initial_state_;
  size_t state_count_;
  std::vector<size_t> final_states_;
  std::vector<CUDD::ADD> transition_function_;

  ExplicitStateDfa(std::shared_ptr<VarMgr> var_mgr);

  static std::runtime_error bad_file_format_exception(std::size_t line_number);


    static CUDD::ADD build_add_from_dfa_mona(
            unsigned node_index,
            const std::shared_ptr<VarMgr>& var_mgr,
            const std::vector<std::string>& variable_names,
            const ExplicitStateDfaMona& explicit_dfa,
            std::unordered_map<std::size_t, CUDD::ADD>& add_table);

 public:

  static ExplicitStateDfa from_dfa_mona(std::shared_ptr<VarMgr> var_mgr,
                                           const ExplicitStateDfaMona& explicit_dfa);

    /**
     * \brief Complements a DFA.
     *
     * Basically exchange accepting states and non-accepting states.
     *
     * \param var_mgr The variable manager for managing transition variables.
     * \return The complemented DFA.
     */
  static ExplicitStateDfa complement_dfa(ExplicitStateDfa& d);

  /**
   * \brief Returns the variable manager.
   */
  std::shared_ptr<VarMgr> var_mgr() const;

  /**
   * \brief Returns the initial state of the DFA.
   */
  std::size_t initial_state() const;

  /**
   * \brief Returns the number of states in the DFA.
   */
  std::size_t state_count() const;

  /**
   * \brief Returns the list of indices of final states of the DFA.
   */
  std::vector<std::size_t> final_states() const;

  /**
   * \brief Returns the transition function of the DFA as a vector of ADDs.
   *
   * The ADD in index \a i represents the transition function for state \a i.
   */
  std::vector<CUDD::ADD> transition_function() const;

  /**
   * \brief Saves the transition function of the DFA in a .dot file.
   *
   * \param filename The name of the file to save the transition function to.
   */
  void dump_dot(const std::string& filename) const;
};

}

#endif // EXPLICIT_STATE_DFA_H
