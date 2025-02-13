#include "ReachabilityMaxSetSynthesizer.h"

#include <cassert>

namespace Syft {

    ReachabilityMaxSetSynthesizer::ReachabilityMaxSetSynthesizer(SymbolicStateDfa spec,
                                                     Player starting_player, Player protagonist_player,
                                                     CUDD::BDD goal_states,
                                                     CUDD::BDD state_space)
            : DfaGameSynthesizer(spec, starting_player, protagonist_player)
            , goal_states_(goal_states), state_space_(state_space)
    {}


    SynthesisResult ReachabilityMaxSetSynthesizer::run() const {
        SynthesisResult result;
        CUDD::BDD winning_states = state_space_ & goal_states_;
        CUDD::BDD winning_moves = winning_states;

        while (true) {
            CUDD::BDD new_winning_states, new_winning_moves;

            if (starting_player_ == Player::Agent){
                CUDD::BDD quantified_X_transitions_to_winning_states = preimage(winning_states);
                new_winning_moves = winning_moves |
                                          (state_space_ & (!winning_states) & quantified_X_transitions_to_winning_states);

                new_winning_states = project_into_states(new_winning_moves);

                // std::cout << quantified_X_transitions_to_winning_states << std::endl;
                // std::cout << new_winning_moves << std::endl;
                // std::cout << new_winning_states << std::endl;
            } else {
                CUDD::BDD transitions_to_winning_states = preimage(winning_states);
                CUDD::BDD new_collected_winning_states = project_into_states(transitions_to_winning_states);
                new_winning_states = winning_states | new_collected_winning_states;
                new_winning_moves = winning_moves |
                        ((!winning_states) & new_collected_winning_states & transitions_to_winning_states);
            }

            if (includes_initial_state(new_winning_states)) {
                result.realizability = true;
                result.winning_states = new_winning_states;
                result.winning_moves = new_winning_moves;
                result.transducer = nullptr;
                return result;

            } else if (new_winning_states == winning_states) {
                result.realizability = false;
                result.winning_states = new_winning_states;
                result.winning_moves = new_winning_moves;
                result.transducer = nullptr;
                return result;
            }

            winning_moves = new_winning_moves;
            winning_states = new_winning_states;
        }

    }

    std::unique_ptr<Transducer> ReachabilityMaxSetSynthesizer::AbstractSingleStrategy(SynthesisResult result) const {
        std::unordered_map<int, CUDD::BDD> strategy = synthesize_strategy(
                result.winning_moves);

        auto transducer = std::make_unique<Transducer>(
                var_mgr_, initial_vector_, strategy, spec_.transition_function(),
                starting_player_);
        return transducer;
    }

    MaxSet ReachabilityMaxSetSynthesizer::AbstractMaxSet(SynthesisResult result) const {
        MaxSet maxset;
        maxset.nondeferring_strategy = result.winning_moves;
        maxset.deferring_strategy = result.winning_moves | (result.winning_states & preimage(result.winning_states));
        return maxset;
    }

    std::pair<std::unique_ptr<Transducer>, std::unique_ptr<Transducer>>
    ReachabilityMaxSetSynthesizer::AbstractSingleStrategyFromMaxSet(MaxSet maxset) const {
        std::unordered_map<int, CUDD::BDD> df_strategy = synthesize_strategy(
                maxset.deferring_strategy);

        auto df_transducer = std::make_unique<Transducer>(
                var_mgr_, initial_vector_, df_strategy, spec_.transition_function(),
                starting_player_);

        std::unordered_map<int, CUDD::BDD> ndf_strategy = synthesize_strategy(
                maxset.nondeferring_strategy);

        auto ndf_transducer = std::make_unique<Transducer>(
                var_mgr_, initial_vector_, ndf_strategy, spec_.transition_function(),
                starting_player_);

        std::pair<std::unique_ptr<Transducer>, std::unique_ptr<Transducer>> transducer;
        transducer.first = std::move(df_transducer);
        transducer.second = std::move(ndf_transducer);
        return transducer;
    }

}
