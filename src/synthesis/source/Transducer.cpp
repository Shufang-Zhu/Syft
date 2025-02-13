#include "Transducer.h"

#include <cstring>

namespace Syft {

Transducer::Transducer(std::shared_ptr<VarMgr> var_mgr,
                       std::vector<int> initial_vector,
                       std::unordered_map<int, CUDD::BDD> output_function,
                       std::vector<CUDD::BDD> transition_function,
                       Player starting_player,
                       Player protagonist_player)
    : var_mgr_(std::move(var_mgr))
    , initial_vector_(std::move(initial_vector))
    , output_function_(std::move(output_function))
    , transition_function_(std::move(transition_function))
    , starting_player_(starting_player)
    , protagonist_player_(protagonist_player)
{}

void Transducer::dump_dot(const std::string& filename) const {
	std::vector<std::string> output_labels;

	if (protagonist_player_ == Player::Environment) {
		output_labels = var_mgr_->input_variable_labels();
	} else {
		output_labels = var_mgr_->output_variable_labels();
	}

  std::size_t output_count = output_function_.size();
  std::vector<CUDD::ADD> output_vector(output_count);

  for (std::size_t i = 0; i < output_count; ++i) {
    std::string label = output_labels[i];
    int index = var_mgr_->name_to_variable(label).NodeReadIndex();
    output_vector[i] = output_function_.at(index).Add();
  }

  var_mgr_->dump_dot(output_vector, output_labels, filename);
}  

}
