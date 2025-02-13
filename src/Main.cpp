#include <cstring>
#include <iostream>
#include <memory>

#include "Stopwatch.h"

#include "ExplicitStateDfaMona.h"
#include "ReachabilityMaxSetSynthesizer.h"
#include "InputOutputPartition.h"
#include <CLI/CLI.hpp>
#include <istream>


int main(int argc, char ** argv) {

    CLI::App app {
            "Syft interface for maxset"
    };

    std::string formula_file, partition_file;

    app.add_option("-f,--formula-file", formula_file, "Specification file")->
                    required() -> check(CLI::ExistingFile);

    app.add_option("-p,--partition-file", partition_file, "Partition file" )->
                    required () -> check(CLI::ExistingFile);

    bool env_start = false;
    app.add_flag("-e, --environment", env_start, "Environment as the first player");

    bool maxset = false;
    app.add_flag("-m,--maxset", maxset, "Maxset flag (Default: false)");

    bool fanin_encoding = false;
    app.add_flag("--fanin", fanin_encoding, "fanin encoding flag (Default: false)");

    bool fanout_encoding = false;
    app.add_flag("--fanout", fanout_encoding, "fanout encoding flag (Default: false)");


    CLI11_PARSE(app, argc, argv);
    Syft::Stopwatch total_time_stopwatch; // stopwatch for end-to-end execution
    total_time_stopwatch.start();

    Syft::Stopwatch aut_time_stopwatch; // stopwatch for abstract single strategy
    aut_time_stopwatch.start();

    Syft::Player starting_player = env_start? Syft::Player::Environment : Syft::Player::Agent;
    Syft::Player protagonist_player = Syft::Player::Agent;
    bool realizability;
    std::ifstream in(formula_file);
    std::string f;
    std::getline(in, f);

    Syft::InputOutputPartition partition =
            Syft::InputOutputPartition::read_from_file(partition_file);
    std::shared_ptr<Syft::VarMgr> var_mgr = std::make_shared<Syft::VarMgr>();
    var_mgr->create_named_variables(partition.input_variables);
    var_mgr->create_named_variables(partition.output_variables);

    Syft::ExplicitStateDfaMona explicit_dfa_mona = Syft::ExplicitStateDfaMona::dfa_of_formula(f);
    Syft::ExplicitStateDfa explicit_dfa =  Syft::ExplicitStateDfa::from_dfa_mona(var_mgr, explicit_dfa_mona);

    // explicit_dfa_mona.dfa_print();

    Syft::SymbolicStateDfa symbolic_dfa =
    (fanout_encoding) ? Syft::SymbolicStateDfa::from_explicit_fanout_encoding(std::move(explicit_dfa))
    : (fanin_encoding) ? Syft::SymbolicStateDfa::from_explicit_fanin_encoding(std::move(explicit_dfa))
    : Syft::SymbolicStateDfa::from_explicit(std::move(explicit_dfa));


    auto aut_time = aut_time_stopwatch.stop();
    std::cout << "DFA construction time: "
              << aut_time.count() << " ms" << std::endl;

    std::cout << "Number of BDD nodes: "
          << symbolic_dfa.bdd_nodes_num() << std::endl;
    std::cout << "Number of BDD nodes in transitions: "
          << symbolic_dfa.bdd_nodes_num_transitions() << std::endl;
    std::cout << "Number of BDD nodes in final states: "
          << symbolic_dfa.bdd_nodes_num_final_states() << std::endl;

    // symbolic_dfa.dump_dot("dfa-o.dot");
    // for (auto i = 0; i < symbolic_dfa.initial_state().size(); ++i) {
    //     std::cout << symbolic_dfa.initial_state()[i] << " ";
    // }
    // std::cout << "Final states: " << symbolic_dfa.final_states() << std::endl;


    Syft::Stopwatch nondef_strategy_time_stopwatch; // stopwatch for strategy_generator construction
    nondef_strategy_time_stopwatch.start();

    Syft::Stopwatch syn_time_stopwatch; // stopwatch for strategy_generator construction
    syn_time_stopwatch.start();

    var_mgr->partition_variables(partition.input_variables,
                                 partition.output_variables);

    Syft::ReachabilityMaxSetSynthesizer synthesizer(symbolic_dfa, starting_player,
                                              protagonist_player, symbolic_dfa.final_states(),
                                  var_mgr->cudd_mgr()->bddOne());
    Syft::SynthesisResult result = synthesizer.run();

    realizability = result.realizability;
    if (realizability == true) {
        std::cout << "The problem is Realizable" << std::endl;

        auto nondef_strategy_time = nondef_strategy_time_stopwatch.stop();
        std::cout << "Nondeferring strategy generator construction time: "
                  << nondef_strategy_time.count() << " ms" << std::endl;

        if (!maxset) {
            Syft::Stopwatch abstract_single_strategy_time_stopwatch; // stopwatch for abstract single strategy
            abstract_single_strategy_time_stopwatch.start();

            auto transducer = synthesizer.AbstractSingleStrategy(std::move(result));
//            transducer->dump_dot("strategy.dot");

            auto abstract_single_strategy_time = abstract_single_strategy_time_stopwatch.stop();
            std::cout << "Abstract single strategy time: "
                      << abstract_single_strategy_time.count() << " ms" << std::endl;
        } else {
            Syft::Stopwatch def_strategy_time_stopwatch; // stopwatch for abstract single strategy
            def_strategy_time_stopwatch.start();

            Syft::MaxSet maxset = synthesizer.AbstractMaxSet(std::move(result));

            auto def_strategy_time = def_strategy_time_stopwatch.stop();
            std::cout << "Deferring strategy generator construction time: "
                      << def_strategy_time.count() << " ms" << std::endl;

            Syft::Stopwatch abstract_single_strategy_time_stopwatch; // stopwatch for abstract single strategy
            abstract_single_strategy_time_stopwatch.start();

//            auto transducer_pair = synthesizer.AbstractSingleStrategyFromMaxSet(std::move(maxset));
////            transducer_pair.first->dump_dot("deferring_strategy.dot");
////            transducer_pair.second->dump_dot("nondeferring_strategy.dot");
//
//            auto abstract_single_strategy_time = abstract_single_strategy_time_stopwatch.stop();
//            std::cout << "Abstract single strategy time: "
//                      << abstract_single_strategy_time.count() << " ms" << std::endl;
        }
    }
    else{
        std::cout << "The problem is Unrealizable" << std::endl;
    }
    auto synthesis_time = syn_time_stopwatch.stop();

    std::cout << "Synthesis time: "
          << synthesis_time.count() << " ms" << std::endl;
  auto total_time = total_time_stopwatch.stop();

  std::cout << "Total time: "
	    << total_time.count() << " ms" << std::endl;

  return 0;
}

