#include "ExplicitStateDfa.h"
#include <fstream>
#include <cstddef>
#include <functional>
#include <queue>
#include "String_utilities.h"



namespace Syft {

ExplicitStateDfa::ExplicitStateDfa(std::shared_ptr<VarMgr> var_mgr)
  : var_mgr_(std::move(var_mgr))
{}


ExplicitStateDfa ExplicitStateDfa::from_dfa_spot(std::shared_ptr<VarMgr> var_mgr, spot::bdd_dict_ptr bdd_dict_ptr,
                                                     const spot::mtdfa_ptr mtdfa) {
    std::size_t initial_state = 0; // The initial state of spot_dfa is always 0
    std::vector<spot::formula> variable_names_in_spot= mtdfa->aps;
    std::vector<std::string> variable_names;
    variable_names.reserve(variable_names_in_spot.size());
    for (auto var : variable_names_in_spot) {
        std::string name = var.ap_name();
        variable_names.push_back(var.ap_name());
    }

    var_mgr->create_named_variables(variable_names);

    std::vector<bdd> mtdfa_transition = mtdfa->states;
    // TerminalCollectionResult mtdfa_stats = terminal_and_acc_mtdfa(mtdfa_transition);
    // std::size_t state_count = mtdfa_stats.terminal_to_id.size();



    std::queue<size_t> fifo_queue;
    std::vector<size_t> states;
    for (std::size_t i = 0; i < mtdfa_transition.size(); i++) {
        fifo_queue.push(i);
    }
    std::unordered_map<size_t, CUDD::ADD> add_table;
    std::unordered_map<size_t, size_t> final_states_to_root;
    size_t false_state = mtdfa_transition.size();
    fifo_queue.push(false_state);
    size_t next_state = false_state + 1;
    size_t true_state = -1;
    if (mtdfa->num_roots() != mtdfa->num_states()) {
        assert(mtdfa->num_roots() + 1 == mtdfa->num_states());
        true_state = false_state + 1;
        fifo_queue.push(true_state);
        next_state = true_state + 1;
    }
    std::vector<CUDD::ADD> transition_function;
    while (fifo_queue.size() > 0) {
        size_t state = fifo_queue.front();
        fifo_queue.pop();
        CUDD::ADD add;
        if (state < mtdfa_transition.size()) {
            add = bdd_to_cudd_add(var_mgr, bdd_dict_ptr, mtdfa_transition[state], add_table,
                variable_names, fifo_queue, false_state, true_state, next_state, final_states_to_root);
        } else if (state == false_state) {
            add = var_mgr->cudd_mgr()->constant(false_state);
        } else if (state == true_state) {
            add = var_mgr->cudd_mgr()->constant(true_state);
        } else {
            size_t root = final_states_to_root[state];
            add = transition_function[root];
        }
        transition_function.push_back(add);
    }

    std::vector<size_t> final_states;
    final_states.reserve(final_states_to_root.size());  // optional, for efficiency
    for (const auto& [key, _] : final_states_to_root) {
        final_states.push_back(key);
        // std::cout<<key<<" ";
    }



    ExplicitStateDfa dfa(std::move(var_mgr));
    dfa.initial_state_ = initial_state;
    dfa.state_count_ = next_state;
    dfa.final_states_ = std::move(final_states);
    dfa.transition_function_ = std::move(transition_function);

    return dfa;

}


CUDD::ADD ExplicitStateDfa::bdd_to_cudd_add(std::shared_ptr<VarMgr> var_mgr,
                            spot::bdd_dict_ptr bdd_dict_ptr,
                            const bdd& f,
                            std::unordered_map<size_t, CUDD::ADD>& add_table,
                            const std::vector<std::string>& variable_names,
                            std::queue<size_t>& fifo_queue,
                            size_t& false_state,
                            size_t& true_state,
                            size_t& next_state,
                            std::unordered_map<size_t, size_t>& final_states_to_root){
    int id = f.id();
    if (add_table.count(id))
        return add_table[id]; // already converted

    // Boolean sink
    if (f == bddfalse) {
        CUDD::ADD leaf = var_mgr->cudd_mgr()->constant(false_state);
        add_table[id] = leaf;
        return leaf;
    }

    if (f == bddtrue) {
        CUDD::ADD leaf = var_mgr->cudd_mgr()->constant(true_state);
        add_table[id] = leaf;
        final_states_to_root.insert({true_state, true_state});
        return leaf;
    }

    // Numeric terminal
    if (bdd_is_terminal(f)) {
        int L = bdd_get_terminal(f);
        bool accepting = (L & 1);
        int succ_id;
        if (accepting) {
            succ_id = next_state;
            fifo_queue.push(succ_id);
            size_t root = (L-1)/2;
            final_states_to_root.insert({succ_id, root});
            next_state++;
        } else {
            succ_id = L/2;
        }

        CUDD::ADD leaf = var_mgr->cudd_mgr()->constant(succ_id);
        add_table[id] = leaf;
        return leaf;
    }

    // Internal node: rebuild children
    int name_index = bdd_var(f);
    const spot::bdd_dict::bdd_info& i = bdd_dict_ptr->bdd_map[name_index];
    assert(i.type == spot::bdd_dict::var);
    spot::formula res = i.f;
    std::string variable_name = res.ap_name();
    CUDD::ADD root_node = var_mgr->name_to_variable(variable_name).Add();
    CUDD::ADD low_child = bdd_to_cudd_add(var_mgr, bdd_dict_ptr, bdd_low(f),  add_table, variable_names, fifo_queue,
        false_state, true_state, next_state, final_states_to_root);
    CUDD::ADD high_child = bdd_to_cudd_add(var_mgr, bdd_dict_ptr, bdd_high(f), add_table, variable_names,
        fifo_queue, false_state, true_state, next_state, final_states_to_root);


    CUDD::ADD ite = root_node.Ite(high_child, low_child);

    add_table[id] = ite;
    return ite;
}

ExplicitStateDfa ExplicitStateDfa::complement_dfa(ExplicitStateDfa& d) {
    std::vector<size_t> final_states;
    std::vector<size_t> current_final_states = d.final_states();
    for (size_t i = d.initial_state(); i < d.state_count(); i++){
        if(std::find(current_final_states.begin(), current_final_states.end(), i) == current_final_states.end()){
            final_states.push_back(i);
        }
    }


    ExplicitStateDfa dfa(d.var_mgr());
    dfa.initial_state_ = d.initial_state();
    dfa.state_count_ = d.state_count();
    dfa.final_states_ = std::move(final_states);
    dfa.transition_function_ = d.transition_function();

    return dfa;
}

std::shared_ptr<VarMgr> ExplicitStateDfa::var_mgr() const {
  return var_mgr_;
}

std::size_t ExplicitStateDfa::initial_state() const {
  return initial_state_;
}

std::size_t ExplicitStateDfa::state_count() const {
  return state_count_;
}

std::vector<std::size_t> ExplicitStateDfa::final_states() const {
  return final_states_;
}

std::vector<CUDD::ADD> ExplicitStateDfa::transition_function() const {
  return transition_function_;
}

void ExplicitStateDfa::dump_dot(const std::string& filename) const {
  std::vector<std::string> function_labels(state_count_);

  for (std::size_t i = 0; i < state_count_; ++i) {
	  function_labels[i] = "S" + std::to_string(i);
  }

  var_mgr_->dump_dot(transition_function_, function_labels, filename);
}

};
