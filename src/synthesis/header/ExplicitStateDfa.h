#ifndef EXPLICIT_STATE_DFA_H
#define EXPLICIT_STATE_DFA_H

#include <exception>
#include <istream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "VarMgr.h"
// #include "ExplicitStateDfaMona.h"
#include <spot/twaalgos/ltlf2dfa.hh>
#include <spot/twa/twagraph.hh>

namespace Syft {

  // struct Terminal {
  //   enum Kind { NUMERIC, FALSE, TRUE } kind;
  //   int value;
  //
  //   bool operator==(const Terminal& other) const {
  //     return kind == other.kind && value == other.value;
  //   }
  // };

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


    // static CUDD::ADD build_add_from_dfa_mona(
    //         unsigned node_index,
    //         const std::shared_ptr<VarMgr>& var_mgr,
    //         const std::vector<std::string>& variable_names,
    //         const ExplicitStateDfaMona& explicit_dfa,
    //         std::unordered_map<std::size_t, CUDD::ADD>& add_table);

  static CUDD::ADD bdd_to_cudd_add(std::shared_ptr<VarMgr> var_mgr,
                            spot::bdd_dict_ptr bdd_dict_ptr,
                            const bdd& f,
                            std::unordered_map<size_t, CUDD::ADD>& add_table,
                            const std::vector<std::string>& variable_names,
                            std::queue<size_t>& fifo_queue,
                            size_t& false_state,
                            size_t& true_state,
                            size_t& next_state,
                            std::unordered_map<size_t, size_t>& final_states_to_root);

  // static std::vector<CUDD::ADD> build_cudd_adds_for_mtdfa(std::shared_ptr<VarMgr> var_mgr,
  //   const std::vector<bdd>& mtdfa_transition,
  //   const std::vector<std::string>& variable_names,
  //   const std::unordered_map<Terminal, int>& terminal_to_id,
  //   const std::unordered_map<int, const bdd*>& succ_id_to_logic_root,
  //   int total_states);

 public:

  // static ExplicitStateDfa from_dfa_mona(std::shared_ptr<VarMgr> var_mgr,
                                           // const ExplicitStateDfaMona& explicit_dfa);

  static ExplicitStateDfa from_dfa_spot(std::shared_ptr<VarMgr> var_mgr,
                                          spot::bdd_dict_ptr bdd_dict_ptr,
                                          const spot::mtdfa_ptr mtdfa);

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

// Specialize std::hash<Syft::Terminal> in namespace std
// namespace std {
//   template<>
//   struct hash<Syft::Terminal> {
//     size_t operator()(const Syft::Terminal& t) const noexcept {
//       size_t h1 = ::std::hash<int>()(static_cast<int>(t.kind));
//       size_t h2 = ::std::hash<int>()(t.value);
//       return h1 ^ (h2 << 1);
//     }
//   };
// }

#endif // EXPLICIT_STATE_DFA_H
