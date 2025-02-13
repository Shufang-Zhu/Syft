#include "SymbolicStateDfa.h"
#include <algorithm>

#include <bitset>
#include <CLI/TypeTools.hpp>

namespace Syft {

SymbolicStateDfa::SymbolicStateDfa(std::shared_ptr<VarMgr> var_mgr)
  : var_mgr_(std::move(var_mgr))
{}

std::pair<std::size_t, std::size_t> SymbolicStateDfa::create_state_variables(
    std::shared_ptr<VarMgr>& var_mgr,
    std::size_t state_count) {
  // Largest state index that needs to be represented
  std::size_t max_state = state_count - 1;
  
  std::size_t bit_count = 0;

  // Number of iterations equals the log of the state count
  while (max_state > 0) {
    ++bit_count;
    max_state >>= 1;
  }

  std::size_t automaton_id = var_mgr->create_state_variables(bit_count);
  
  return std::make_pair(bit_count, automaton_id);
}

std::vector<int> SymbolicStateDfa::state_to_binary(std::size_t state,
						   std::size_t bit_count) {
  std::vector<int> binary_representation;

  while (state != 0) {
    // Add the least significant bit of the state to the binary representation
    binary_representation.push_back(state & 1);

    // Shift right
    state >>= 1;
  }

  // Fill rest of the vector with zeroes up to bit_count
  binary_representation.resize(bit_count);

  // Note that the binary representation goes from least to most significant bit

  return binary_representation;
}

CUDD::BDD SymbolicStateDfa::state_to_bdd(
    const std::shared_ptr<VarMgr>& var_mgr,
    std::size_t automaton_id,
    std::size_t state) {
  std::size_t bit_count = var_mgr->state_variable_count(automaton_id);
  std::vector<int> binary_representation = state_to_binary(state, bit_count);
  
  return var_mgr->state_vector_to_bdd(automaton_id, binary_representation);
}

CUDD::BDD SymbolicStateDfa::state_set_to_bdd(
    const std::shared_ptr<VarMgr>& var_mgr,
    std::size_t automaton_id,
    const std::vector<size_t>& states) {
  CUDD::BDD bdd = var_mgr->cudd_mgr()->bddZero();

  for (std::size_t state : states) {
    bdd |= state_to_bdd(var_mgr, automaton_id, state);
  }

  return bdd;
}

std::vector<CUDD::BDD> SymbolicStateDfa::symbolic_transition_function(
    const std::shared_ptr<VarMgr>& var_mgr,
    std::size_t automaton_id,
    const std::vector<CUDD::ADD>& transition_function) {
  std::size_t bit_count = var_mgr->state_variable_count(automaton_id);
  std::vector<CUDD::BDD> symbolic_transition_function(
      bit_count, var_mgr->cudd_mgr()->bddZero());

  for (std::size_t j = 0; j < transition_function.size(); ++j) {
    CUDD::BDD state_bdd = state_to_bdd(var_mgr, automaton_id, j);

    for (std::size_t i = 0; i < bit_count; ++i) {
      // BddIthBit counts from the least-significant bit
      CUDD::BDD jth_component = state_bdd & transition_function[j].BddIthBit(i);

      symbolic_transition_function[i] |= jth_component;
    }
  }

  return symbolic_transition_function;
}

SymbolicStateDfa SymbolicStateDfa::from_explicit(
    const ExplicitStateDfa& explicit_dfa) {
  std::shared_ptr<VarMgr> var_mgr = explicit_dfa.var_mgr();

  auto count_and_id = create_state_variables(var_mgr,
					     explicit_dfa.state_count());
  std::size_t bit_count = count_and_id.first;
  std::size_t automaton_id = count_and_id.second;
  
  std::vector<int> initial_state = state_to_binary(explicit_dfa.initial_state(),
						   bit_count);
  
  CUDD::BDD final_states = state_set_to_bdd(var_mgr, automaton_id,
					    explicit_dfa.final_states());
  
  std::vector<CUDD::BDD> transition_function = symbolic_transition_function(
      var_mgr, automaton_id, explicit_dfa.transition_function());

  SymbolicStateDfa symbolic_dfa(var_mgr);
  symbolic_dfa.automaton_id_ = automaton_id;
  symbolic_dfa.initial_state_ = std::move(initial_state);
  symbolic_dfa.final_states_ = std::move(final_states);
  symbolic_dfa.transition_function_ = std::move(transition_function);

  return symbolic_dfa;
}


SymbolicStateDfa SymbolicStateDfa::from_explicit_fanout_encoding(const ExplicitStateDfa &explicit_dfa) {
  std::shared_ptr<VarMgr> var_mgr = explicit_dfa.var_mgr();

  int state_count = explicit_dfa.state_count();
  auto count_and_id = create_state_variables(var_mgr,
               state_count);
  std::size_t bit_count = count_and_id.first;
  std::size_t automaton_id = count_and_id.second;

  std::vector<std::vector<CUDD::BDD>> state_metrics(state_count);
  std::vector<std::vector<int>> state_connections(state_count);
  std::vector<std::vector<int>> predecessors_vec(state_count);

  std::vector<CUDD::ADD> transition_function_add = explicit_dfa.transition_function();

  for (std::size_t i = 0; i < state_metrics.size(); ++i) {
    std::vector<CUDD::BDD> state_metric(state_count);
    std::vector<int> state_connection(state_count);
    for (std::size_t j = 0; j < state_metrics.size(); ++j) {
      CUDD::BDD transition = transition_function_add[i].BddInterval(j, j);
      // CUDD::BDD transition = var_mgr->cudd_mgr()->bddZero();
      // var_mgr->dump_dot(transition.Add(), std::to_string(i)+"_"+std::to_string(j)+".dot");
      state_metric[j] = transition;
      if (transition == var_mgr->cudd_mgr()->bddZero()) {
        state_connection[j] = 0;
      } else {
        state_connection[j] = 1;
        predecessors_vec[j].push_back(i);
      }
    }
    state_metrics[i] = state_metric;
    state_connections[i] = state_connection;
  }

  std::unordered_map<int, std::string> state_encodings = faninout_encoding(state_connections, bit_count);

  // std::cout << "fanout encoding constructed\n";
  // for (auto it : state_encodings) {
  //   std::cout << it.first << " " << it.second << std::endl;
  // }

  // build transition function
  std::vector<CUDD::BDD> transition_function;
  for (int i = 0; i < bit_count; ++i) {
    std::vector<int> states_with_bit_i;
    for (int j = 0; j < state_count; j++) {
      if (state_encodings[j][i] == '1') {
        states_with_bit_i.push_back(j);
      }
    }

    CUDD::BDD transition = var_mgr->cudd_mgr()->bddZero();
    for (int j = 0; j < states_with_bit_i.size(); j++) {
      int succ = states_with_bit_i[j];
      for (int k = 0; k < predecessors_vec[succ].size(); k++) {
        int curr = predecessors_vec[succ][k];
        CUDD::BDD state_bdd = state_to_bdd_with_encoding(var_mgr, automaton_id, state_encodings, curr);
        CUDD::BDD condition = state_metrics[curr][succ];
        transition = transition + (state_bdd * condition);
        // std::cout << "curr: " << curr << std::endl;
        // std::cout << "curr_bdd: " << state_bdd << std::endl;
        // std::cout << "succ: " << succ << std::endl;
        // std::cout << "condition: " << condition << std::endl;
        // var_mgr->dump_dot(condition.Add(), std::to_string(state)+"-"+std::to_string(states_with_bit_i[j])+".dot");
      }
    }

    transition_function.push_back(transition);
  }
  std::vector<int> initial_state = binary_string_to_vector(state_encodings[explicit_dfa.initial_state()]);
  CUDD::BDD final_states = state_set_to_bdd_with_encoding(var_mgr, automaton_id, state_encodings,
              explicit_dfa.final_states());

  SymbolicStateDfa symbolic_dfa(var_mgr);
  symbolic_dfa.automaton_id_ = automaton_id;
  symbolic_dfa.initial_state_ = std::move(initial_state);
  symbolic_dfa.final_states_ = std::move(final_states);
  symbolic_dfa.transition_function_ = std::move(transition_function);

  return symbolic_dfa;
}

SymbolicStateDfa SymbolicStateDfa::from_explicit_fanin_encoding(const ExplicitStateDfa &explicit_dfa) {
  //TODO
  std::shared_ptr<VarMgr> var_mgr = explicit_dfa.var_mgr();

  int state_count = explicit_dfa.state_count();
  auto count_and_id = create_state_variables(var_mgr,
               state_count);
  std::size_t bit_count = count_and_id.first;
  std::size_t automaton_id = count_and_id.second;

  std::vector<std::vector<CUDD::BDD>> state_metrics(state_count);
  std::vector<std::vector<int>> state_connections(state_count);
  std::vector<std::vector<int>> predecessors_vec(state_count);
  std::vector<std::vector<int>> state_connections_fanin(state_count, std::vector<int>(state_count));

  std::vector<CUDD::ADD> transition_function_add = explicit_dfa.transition_function();

  for (std::size_t i = 0; i < state_metrics.size(); ++i) {
    std::vector<CUDD::BDD> state_metric(state_count);
    std::vector<int> state_connection(state_count);
    for (std::size_t j = 0; j < state_metrics.size(); ++j) {
      CUDD::BDD transition = transition_function_add[i].BddInterval(j, j);
      // CUDD::BDD transition = var_mgr->cudd_mgr()->bddZero();
      // var_mgr->dump_dot(transition.Add(), std::to_string(i)+"_"+std::to_string(j)+".dot");
      state_metric[j] = transition;
      if (transition == var_mgr->cudd_mgr()->bddZero()) {
        state_connection[j] = 0;
        state_connections_fanin[j][i] = 0;
      } else {
        state_connection[j] = 1;
        state_connections_fanin[j][i] = 1;
        predecessors_vec[j].push_back(i);
      }
    }
    state_metrics[i] = state_metric;
    state_connections[i] = state_connection;
  }

  // std::vector<std::vector<int>> rotatedMatrix(state_count, std::vector<int>(state_count));
  //
  // for (int i = 0; i < state_count; ++i) {
  //   for (int j = 0; j < state_count; ++j) {
  //     rotatedMatrix[j][i] = state_connections[i][j];
  //   }
  // }
  // assert(rotatedMatrix == state_connections_fanin);

  std::unordered_map<int, std::string> state_encodings = faninout_encoding(state_connections_fanin, bit_count);

  // std::cout << "fanin encoding constructed\n";
  // for (auto it : state_encodings) {
  //   std::cout << it.first << " " << it.second << std::endl;
  // }

  // build transition function
  std::vector<CUDD::BDD> transition_function;
  for (int i = 0; i < bit_count; ++i) {
    std::vector<int> states_with_bit_i;
    for (int j = 0; j < state_count; j++) {
      if (state_encodings[j][i] == '1') {
        states_with_bit_i.push_back(j);
      }
    }

    CUDD::BDD transition = var_mgr->cudd_mgr()->bddZero();
    for (int j = 0; j < states_with_bit_i.size(); j++) {
      int succ = states_with_bit_i[j];
      for (int k = 0; k < predecessors_vec[succ].size(); k++) {
        int curr = predecessors_vec[succ][k];
        CUDD::BDD state_bdd = state_to_bdd_with_encoding(var_mgr, automaton_id, state_encodings, curr);
        CUDD::BDD condition = state_metrics[curr][succ];
        transition = transition + (state_bdd * condition);
        // std::cout << "curr: " << curr << std::endl;
        // std::cout << "curr_bdd: " << state_bdd << std::endl;
        // std::cout << "succ: " << succ << std::endl;
        // std::cout << "condition: " << condition << std::endl;
        // var_mgr->dump_dot(condition.Add(), std::to_string(state)+"-"+std::to_string(states_with_bit_i[j])+".dot");
      }
    }

    transition_function.push_back(transition);
  }
  std::vector<int> initial_state = binary_string_to_vector(state_encodings[explicit_dfa.initial_state()]);
  CUDD::BDD final_states = state_set_to_bdd_with_encoding(var_mgr, automaton_id, state_encodings,
              explicit_dfa.final_states());

  SymbolicStateDfa symbolic_dfa(var_mgr);
  symbolic_dfa.automaton_id_ = automaton_id;
  symbolic_dfa.initial_state_ = std::move(initial_state);
  symbolic_dfa.final_states_ = std::move(final_states);
  symbolic_dfa.transition_function_ = std::move(transition_function);

  return symbolic_dfa;
}

CUDD::BDD SymbolicStateDfa::state_to_bdd_with_encoding(
  const std::shared_ptr<VarMgr> &var_mgr,
  std::size_t automaton_id,
  std::unordered_map<int, std::string> state_encodings,
  std::size_t state) {

  std::vector<int> binary_representation = binary_string_to_vector(state_encodings[state]);
  return var_mgr->state_vector_to_bdd(automaton_id, binary_representation);
}

CUDD::BDD SymbolicStateDfa::state_set_to_bdd_with_encoding(const std::shared_ptr<VarMgr> &var_mgr, std::size_t automaton_id,
                                                           std::unordered_map<int, std::string> state_encodings,
                                                           const std::vector<size_t> &states) {
    CUDD::BDD bdd = var_mgr->cudd_mgr()->bddZero();

    for (std::size_t state: states) {
      bdd |= state_to_bdd_with_encoding(var_mgr, automaton_id, state_encodings, state);
    }

    return bdd;
  }

std::vector<int> SymbolicStateDfa::binary_string_to_vector(const std::string& binary_string) {
  std::vector<int> result;
  for (char c : binary_string) {
    if (c == '0' || c == '1') {
      result.push_back(c - '0');
    } else {
      throw std::invalid_argument("Invalid character in binary string: " + std::string(1, c));
    }
  }
  return result;
}

std::unordered_map<int, std::string> SymbolicStateDfa::faninout_encoding(std::vector<std::vector<int>>& state_connections, int bit_count) {

  int state_count = state_connections.size();
  std::vector<int> weights = weights_for_faninout_encoding(state_connections, bit_count);

  // Pair each state (index) with its weight
  std::vector<std::pair<int, int>> state_weights;
  for (int i = 0; i < state_count; ++i) {
    state_weights.emplace_back(i, weights[i]);
  }

  // Sort the pairs by weight in descending order
  std::sort(state_weights.begin(), state_weights.end(),
            [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
                return a.second > b.second; // Compare weights
            });

  // for (auto it = state_weights.begin(); it != state_weights.end(); ++it) {
  //   std::cout << it->first << " " << it->second << std::endl;
  // }
  // std::cout << "states sorted by weights" << std::endl;

  std::unordered_map<int, std::string> state_encodings;
  std::unordered_set<int> encoded_states;
  std::unordered_set<std::string> used_encodings;

  for (int i = 0; i < state_weights.size(); ++i) {
    int state_index = state_weights[i].first;
    if (encoded_states.find(state_index) != encoded_states.end()) {
      continue;
    }

    std::string state_encoding = get_encoding_of_new_state(used_encodings, bit_count);
    assert(state_encoding != "");
    state_encodings[state_index] = state_encoding;
    encoded_states.insert(state_index);
    used_encodings.insert(state_encoding);

    // std::cout << state_index << " " << state_encoding << std::endl;
    std::vector<int> successors;
    for (int j = 0; j < state_count; ++j ) {
      if ((state_connections[state_index][j] != 0) && (encoded_states.find(j) == encoded_states.end())) {
        successors.emplace_back(j);
      }
    }

    std::vector<std::string> successor_encodings = get_encodings_of_successors(state_encoding, successors.size(), used_encodings);
    for (int j = 0; j < successors.size(); j++) {
      encoded_states.insert(successors[j]);
      used_encodings.insert(successor_encodings[j]);
      state_encodings[successors[j]] = successor_encodings[j];

    }
    // for (const auto& element : state_encodings) {
    //   std::cout << element.first << " " << element.second << std::endl;
    // }
  }

  return state_encodings;
}

int SymbolicStateDfa::hammingDistance(const std::string& a, const std::string& b) {
  int distance = 0;
  for (size_t i = 0; i < a.size(); ++i) {
    if (a[i] != b[i]) {
      ++distance;
    }
  }
  return distance;
}

std::vector<std::string> SymbolicStateDfa::get_encodings_of_successors(std::string base_encoding, int n,
                                                                       const std::unordered_set<std::string>& used_encodings) {
  std::vector<std::string> new_encodings;
  int bit_count = base_encoding.size();
  int max_value = std::pow(2, bit_count);
  std::vector<std::string> all_encodings;
  for (int i = 0; i < max_value; ++i) {
    std::string binary_encoding = std::bitset<32>(i).to_string().substr(32 - bit_count);
    if (used_encodings.find(binary_encoding) == used_encodings.end()) {
      all_encodings.push_back(binary_encoding);
    }
  }

  std::sort(all_encodings.begin(), all_encodings.end(), [&base_encoding](const std::string& a, const std::string& b) {
      return hammingDistance(a, base_encoding) < hammingDistance(b, base_encoding);
  });

  for (int i = 0; i < n && i < static_cast<int>(all_encodings.size()); ++i) {
    new_encodings.push_back(all_encodings[i]);
  }

  return new_encodings;
}

std::string SymbolicStateDfa::get_encoding_of_new_state(const std::unordered_set<std::string>& used_encodings, int bit_count) {
  int max_value = std::pow(2, bit_count);
  for (int i = 0; i < max_value; ++i) {
    std::string binary_encoding = std::bitset<32>(i).to_string().substr(32 - bit_count);
    if (used_encodings.find(binary_encoding) == used_encodings.end()) {
      return binary_encoding;
    }
  }
  return "";
}



std::vector<int> SymbolicStateDfa::weights_for_faninout_encoding(std::vector<std::vector<int>> state_connections, int bit_count) {
  int state_count = state_connections.size();
  // testing
  // state_connections[0] = std::vector<int>({1, 1, 0});
  // state_connections[1] = std::vector<int>({1, 0, 1});
  // state_connections[2] = std::vector<int>({1, 0, 1});

  std::vector<std::vector<int>> weights_individual(state_count);
  for (std::size_t i = 0; i < state_count; ++i) {
    weights_individual[i].resize(state_count);
    for (std::size_t j = 0; j < state_count; ++j) {
      if (i == j) {
        continue;
      }
      int sum = 0;
      for (std::size_t k = 0; k < state_count; ++k) {
        sum = sum + state_connections[i][k] * state_connections[j][k];
      }
      weights_individual[i][j] = sum * bit_count;
    }
  }

  std::vector<int> weights_sum(state_count);
  for (std::size_t i = 0; i < state_count; ++i) {
    int sum = 0;
    for (std::size_t j = 0; j < state_count; ++j) {
      if (i == j) {
        continue;
      }
      sum = sum + weights_individual[i][j];
      weights_sum[i] = sum;
    }
  }

  return weights_sum;
}




std::shared_ptr<VarMgr> SymbolicStateDfa::var_mgr() const {
  return var_mgr_;
}

std::size_t SymbolicStateDfa::automaton_id() const {
  return automaton_id_;
}
  
std::vector<int> SymbolicStateDfa::initial_state() const {
  return initial_state_;
}

// CUDD::BDD SymbolicStateDfa::initial_state_bdd() const {
//   return state_to_bdd(var_mgr_, automaton_id_, 1);
// }

CUDD::BDD SymbolicStateDfa::final_states() const {
  return final_states_;
}

std::vector<CUDD::BDD> SymbolicStateDfa::transition_function() const {
  return transition_function_;
}

void SymbolicStateDfa::prune_invalid_states(const CUDD::BDD& invalid_states) {
  for (CUDD::BDD& bit_function : transition_function_) {
    // If the current state is an invalid state, send every transition to
    // the sink state 0
    bit_function &= !invalid_states;
  }

  // Remove invalid states from the set of accepting states
  final_states_ &= !invalid_states;
}

void SymbolicStateDfa::restrict_transitions(const CUDD::BDD &feasible_moves) {
    for (CUDD::BDD& bit_function : transition_function_) {
        // Every transition has to be a feasible move
        bit_function &= feasible_moves;
    }
}



void SymbolicStateDfa::dump_dot(const std::string& filename) const {
  std::vector<std::string> function_labels =
	    var_mgr_->state_variable_labels(automaton_id_);
  function_labels.push_back("Final");

  std::vector<CUDD::ADD> adds;
  adds.reserve(transition_function_.size() + 1);

  for (const CUDD::BDD& bdd : transition_function_) {
	  adds.push_back(bdd.Add());
  }

  adds.push_back(final_states_.Add());

  var_mgr_->dump_dot(adds, function_labels, filename);
}

SymbolicStateDfa SymbolicStateDfa::from_predicates(
    std::shared_ptr<VarMgr> var_mgr,
    std::vector<CUDD::BDD> predicates) {
  std::size_t predicate_count = predicates.size();
  std::vector<int> initial_state(predicate_count, 0);
  CUDD::BDD final_states = var_mgr->cudd_mgr()->bddOne();
  std::size_t automaton_id = var_mgr->create_state_variables(predicate_count);

  SymbolicStateDfa dfa(std::move(var_mgr));
  dfa.automaton_id_ = automaton_id;
  dfa.initial_state_ = std::move(initial_state);
  dfa.transition_function_ = std::move(predicates);
  dfa.final_states_ = std::move(final_states);

  return dfa;
}

SymbolicStateDfa SymbolicStateDfa::product(const std::vector<SymbolicStateDfa>& dfa_vector) {
    if (dfa_vector.size() < 1) {
        throw std::runtime_error("Incorrect usage of automata product");
    }

    std::shared_ptr<VarMgr> var_mgr = dfa_vector[0].var_mgr();

    std::vector<std::size_t> automaton_ids;

    std::vector<int> initial_state;

    CUDD::BDD final_states = var_mgr->cudd_mgr()->bddOne();
    std::vector<CUDD::BDD> transition_function;

    for (SymbolicStateDfa dfa : dfa_vector) {
        automaton_ids.push_back(dfa.automaton_id());

        std::vector<int> dfa_initial_state = dfa.initial_state();
        initial_state.insert(initial_state.end(), dfa_initial_state.begin(), dfa_initial_state.end());

        final_states = final_states & dfa.final_states();
        std::vector<CUDD::BDD> dfa_transition_function = dfa.transition_function();
        transition_function.insert(transition_function.end(), dfa_transition_function.begin(), dfa_transition_function.end());
    }

    std::size_t product_automaton_id = var_mgr->create_product_state_space(automaton_ids);

    SymbolicStateDfa product_automaton(var_mgr);
    product_automaton.automaton_id_ = product_automaton_id;
    product_automaton.initial_state_ = std::move(initial_state);
    product_automaton.final_states_ = std::move(final_states);
    product_automaton.transition_function_ = std::move(transition_function);

    return product_automaton;
}

  std::size_t SymbolicStateDfa::bdd_nodes_num_transitions() const {
    return var_mgr()->cudd_mgr()->nodeCount(transition_function_);
  }

  std::size_t SymbolicStateDfa::bdd_nodes_num_final_states() const {
    std::vector<CUDD::BDD> bdd_vec;
    bdd_vec.push_back(final_states_);
    return var_mgr()->cudd_mgr()->nodeCount(bdd_vec);
  }

  std::size_t SymbolicStateDfa::bdd_nodes_num() const {
    std::vector<CUDD::BDD> bdd_vec = transition_function_;
    bdd_vec.push_back(final_states_);
    return var_mgr()->cudd_mgr()->nodeCount(bdd_vec);
  }


}

