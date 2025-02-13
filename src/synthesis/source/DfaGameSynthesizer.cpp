#include "DfaGameSynthesizer.h"

#include <cassert>

namespace Syft {

DfaGameSynthesizer::DfaGameSynthesizer(SymbolicStateDfa spec,
				       Player starting_player,
                       Player protagonist_player)
    : Synthesizer<SymbolicStateDfa>(spec)
    , starting_player_(starting_player)
    , protagonist_player_(protagonist_player){
  var_mgr_ = spec_.var_mgr();
  
  // Make versions of the initial state and transition function that can be used
  // with CUDD::BDD::Eval and CUDD::BDD::VectorCompose, respectively
  initial_vector_ = var_mgr_->make_eval_vector(spec_.automaton_id(),
					       spec_.initial_state());  
  transition_vector_ = var_mgr_->make_compose_vector(
      spec_.automaton_id(), spec_.transition_function());
  
  CUDD::BDD input_cube = var_mgr_->input_cube();
  CUDD::BDD output_cube = var_mgr_->output_cube();

  // quantify_independent_variables_ quantifies all variables that the outputs
  // don't depend on (input variables if the agent plays first, or no variables
  // if the environment plays first). quantify_non_state_variables_ quantifies
  // all remaining variables that are not state variables.
  if (starting_player_ == Player::Environment) {
      if (protagonist_player_ == Player::Environment) {
          quantify_independent_variables_ = std::make_unique<Forall>(output_cube);
          quantify_non_state_variables_ = std::make_unique<Exists>(input_cube);
      } else {
          quantify_independent_variables_ = std::make_unique<NoQuantification>();
          quantify_non_state_variables_ = std::make_unique<ForallExists>(input_cube,
                                                                         output_cube);
      }
  } else {
      if (protagonist_player_ == Player::Environment) {
          quantify_independent_variables_ = std::make_unique<NoQuantification>();
          quantify_non_state_variables_ = std::make_unique<ForallExists>(output_cube,
                                                                         input_cube);
      } else {
          quantify_independent_variables_ = std::make_unique<Forall>(input_cube);
          quantify_non_state_variables_ = std::make_unique<Exists>(output_cube);
      }
  }


}

CUDD::BDD DfaGameSynthesizer::preimage(
    const CUDD::BDD& winning_states) const {
  // Transitions that move into a winning state
  CUDD::BDD winning_transitions =
    winning_states.VectorCompose(transition_vector_);

  // Quantify all variables that the outputs don't depend on
  return quantify_independent_variables_->apply(winning_transitions);
}

CUDD::BDD DfaGameSynthesizer::project_into_states(
    const CUDD::BDD& winning_moves) const {
  return quantify_non_state_variables_->apply(winning_moves);
}

bool DfaGameSynthesizer::includes_initial_state(
    const CUDD::BDD& winning_states) const {
  // Need to create a copy if we want to define the function as const, since
  // CUDD::BDD::Eval does not take the data as const
  std::vector<int> copy(initial_vector_);
  
  return winning_states.Eval(copy.data()).IsOne();
}

std::unordered_map<int, CUDD::BDD> DfaGameSynthesizer::synthesize_strategy(
    const CUDD::BDD& winning_moves) const {
  std::vector<CUDD::BDD> parameterized_output_function;
  int* output_indices;
  CUDD::BDD output_cube = var_mgr_->output_cube();
  std::size_t output_count = var_mgr_->output_variable_count();

  // Need to negate the BDD because b.SolveEqn(...) solves the equation b = 0
  CUDD::BDD pre = (!winning_moves).SolveEqn(output_cube,
					    parameterized_output_function,
					    &output_indices,
					    output_count);

  // Copy the index since it will be necessary in the last step
  std::vector<int> index_copy(output_count);

  for (std::size_t i = 0; i < output_count; ++i) {
    index_copy[i] = output_indices[i];
  }

  // Verify that the solution is correct, also frees output_index
  CUDD::BDD verified = (!winning_moves).VerifySol(parameterized_output_function,
						  output_indices);

  assert(pre == verified);

  std::unordered_map<int, CUDD::BDD> output_function;
  
  // Let y_i be the i-th output variable in the BDD ordering. The parameterized
  // output function for y_i is of the form f_i(x_1, ..., x_m, p_i, ..., p_n)
  // where p_i, ..., p_n are parameters taking the place of y_i, ..., y_n. All
  // f_i are such that no matter what we replace p_i, ..., p_n with, the result
  // is a valid output function. We replace the parameters with 1 so that all
  // f_i are dependent only on the input and state variables.
  for (int i = output_count - 1; i >= 0; --i) {
      int output_index = index_copy[i];

      output_function[output_index] = parameterized_output_function[i];

      // TODO(Lucas): Replace inner loop with CUDD::BDD::VectorCompose
      for (int j = output_count - 1; j >= i; --j) {
          int parameter_index = index_copy[j];

	  // Can be anything, set to the constant 1 for simplicity
	  CUDD::BDD parameter_value = var_mgr_->cudd_mgr()->bddOne();

	  output_function[output_index] =
	    output_function[output_index].Compose(parameter_value,
						  parameter_index);
      }
  }

  return output_function;
}

}
