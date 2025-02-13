#ifndef SYFT_REACHABILITYMAXSETSYNTHESIZER_H
#define SYFT_REACHABILITYMAXSETSYNTHESIZER_H

#include "DfaGameSynthesizer.h"

namespace Syft {
    struct MaxSet{
        CUDD::BDD deferring_strategy;
        CUDD::BDD nondeferring_strategy;
    };

/**
 * \brief A maxset-synthesizer for a reachability game given as a symbolic-state DFA.
 */
    class ReachabilityMaxSetSynthesizer : public DfaGameSynthesizer {
    private:

        CUDD::BDD goal_states_;
        CUDD::BDD state_space_;

    public:

        /**
         * \brief Construct a maxset-synthesizer for the given reachability game.
         *
         * \param spec A symbolic-state DFA representing the reachability game.
         * \param starting_player The player that moves first each turn.
         * \param goal_states The set of states that the agent must reach to win.
         */
        ReachabilityMaxSetSynthesizer(SymbolicStateDfa spec, Player starting_player, Player protagonist_player,
                                      CUDD::BDD goal_states, CUDD::BDD state_space);

        /**
         * \brief Solves the reachability game.
         *
         * \return The result consists of
         * realizability
         * a set of agent winning states
         * a transducer representing a winning strategy or nullptr if the game is unrealizable.
         */
        virtual SynthesisResult run() const final;

        std::unique_ptr<Transducer> AbstractSingleStrategy(SynthesisResult result) const;

        MaxSet AbstractMaxSet(SynthesisResult result) const;

        std::pair<std::unique_ptr<Transducer>, std::unique_ptr<Transducer>> AbstractSingleStrategyFromMaxSet(MaxSet maxset) const;
    };

}

#endif //SYFT_REACHABILITYMAXSETSYNTHESIZER_H
