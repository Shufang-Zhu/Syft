#include <fstream>
#include <iostream>
#include <string>
#include <SymbolicStateDfa.h>
#include "VarMgr.h"
#include "Player.h"
#include "InputOutputPartition.h"
#include <spot/tl/parse.hh>
#include <spot/twaalgos/ltlf2dfa.hh>
#include <spot/twa/twagraph.hh>
#include <StopWatch.h>

#include "ReachabilityMaxSetSynthesizer.h"

struct Args {
    std::string ltlf_file;
    std::string tlsf_file;
    std::string partition_file;
    bool agent_starting = true;  // true for Agent, false for Environment
    bool help = false;
};

Args parse_args(int argc, char* argv[]) {
    Args args;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            args.help = true;
        } else if (arg == "-f" || arg == "--ltlf") {
            if (i + 1 < argc) {
                args.ltlf_file = argv[++i];
            }
        } else if (arg == "-t" || arg == "--tlsf") {
            if (i + 1 < argc) {
                args.tlsf_file = argv[++i];
            }
        } else if (arg == "-p" || arg == "--partition") {
            if (i + 1 < argc) {
                args.partition_file = argv[++i];
            }
        } else if (arg == "-e" || arg == "--environment-first") {
            args.agent_starting = false;
        }
    }

    return args;
}

void print_usage() {
    std::cout << "Usage: syn [options]\n"
              << "Options:\n"
              << "  -h, --help                   Show this help message\n"
              << "  -f, --ltlf FILE              Input LTLf formula file\n"
              << "  -p, --partition FILE         Input partition file\n"
              << "  -t, --tlsf FILE              Input TLSF specification file\n"
              << "  -e, --environment-first      Environment plays first (default: Agent)\n";
}

int main(int argc, char* argv[]) {
    Syft::Stopwatch total_time_stopwatch; // stopwatch for end-to-end execution
    total_time_stopwatch.start();

    Syft::Stopwatch aut_time_stopwatch; // stopwatch for abstract single strategy
    aut_time_stopwatch.start();

    Syft::Stopwatch sym_time_stopwatch;
    sym_time_stopwatch.start();

    Args args = parse_args(argc, argv);

    if (args.help) {
        print_usage();
        return 0;
    }

    if (args.ltlf_file.empty()) {
        std::cerr << "Error: LTLf formula file is required\n";
        print_usage();
        return 1;
    }

    if (args.partition_file.empty()) {
        std::cerr << "Error: Partition file is required\n";
        print_usage();
        return 1;
    }

    std::string ltlf_formula_str;
    std::ifstream ltlf_formula_stream(args.ltlf_file);
    if (!ltlf_formula_stream.is_open()) {
        std::cerr << "Error: Cannot open LTLf formula file: " << args.ltlf_file << std::endl;
        return 1;
    }
    getline(ltlf_formula_stream, ltlf_formula_str);
    // std::cout << "LTLf formula: " << ltlf_formula_str << std::endl;

    Syft::Player starting_player = args.agent_starting ? Syft::Player::Agent : Syft::Player::Environment;
    // std::cout << "Starting player: " << (args.agent_starting ? "Agent" : "Environment") << std::endl;


    spot::formula pf = spot::parse_formula(ltlf_formula_str); // "G(a) | G(b) | F(!a) | F(!b)"
    // std::cout << pf << std::endl;


    spot::bdd_dict_ptr bdd_dict_ptr = spot::make_bdd_dict();
    spot::ltlf_translator translator(bdd_dict_ptr);
    spot::mtdfa_ptr mtdfa = translator.ltlf_to_mtdfa(pf, true);

    auto aut_time = aut_time_stopwatch.stop();
    std::cout << "SPOT DFA construction time: "
              << aut_time.count() << " ms" << std::endl;


    // TODO if mtdfa is valid, then realizable.
    if (mtdfa->is_empty()) {
        std::cout << "The problem is Unrealizable" << std::endl;
        return 0;
    }


    // std::ofstream file("mtdfa.dot");
    //
    // mtdfa->print_dot(file, -1, false);

    // bddExtCache* cache = translator.get_cache();


    std::shared_ptr<Syft::VarMgr> var_mgr = std::make_shared<Syft::VarMgr>();
    Syft::ExplicitStateDfa explicit_dfa = Syft::ExplicitStateDfa::from_dfa_spot(var_mgr, bdd_dict_ptr, mtdfa);
    // explicit_dfa.dump_dot("dfa.dot");

    // Syft::SymbolicStateDfa symbolic_state_dfa = Syft::SymbolicStateDfa::from_spot_mtdfa(var_mgr, mtdfa);

    // Syft::ExplicitStateDfaMona explicit_dfa_mona = Syft::ExplicitStateDfaMona::dfa_of_formula(ltlf_formula_str);
    // Syft::ExplicitStateDfa explicit_dfa = Syft::ExplicitStateDfa::from_dfa_mona(var_mgr, explicit_dfa_mona);
    Syft::SymbolicStateDfa symbolic_state_dfa = Syft::SymbolicStateDfa::from_explicit(explicit_dfa);

    auto sym_time = sym_time_stopwatch.stop();
    std::cout << "Symbolic DFA construction time: "
              << sym_time.count() << " ms" << std::endl;

    Syft::Stopwatch syn_time_stopwatch;
    syn_time_stopwatch.start();

    Syft::InputOutputPartition partition =
        Syft::InputOutputPartition::read_from_file(args.partition_file);

    // partition.print();
    var_mgr->partition_variables(partition.input_variables, partition.output_variables);



    Syft::ReachabilityMaxSetSynthesizer synthesizer(symbolic_state_dfa, starting_player, Syft::Player::Agent,
                                                       symbolic_state_dfa.final_states(), var_mgr->cudd_mgr()->bddOne());
    Syft::SynthesisResult result = synthesizer.run();

    bool realizability = result.realizability;
    if (realizability == true) {
        std::cout << "The problem is Realizable" << std::endl;
    } else {
        std::cout << "The problem is Unrealizable" << std::endl;
    }

    auto syn_time = syn_time_stopwatch.stop();
    std::cout << "Synthesis time: "
              << syn_time.count() << " ms" << std::endl;

    auto total_time = total_time_stopwatch.stop();
    std::cout << "Total time: "
              << total_time.count() << " ms" << std::endl;

}