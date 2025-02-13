#include "ExplicitStateDfaMona.h"

#include <iostream>
#include <istream>
#include <set>
#include <lydia/mona_ext/mona_ext_base.hpp>
#include <lydia/dfa/mona_dfa.hpp>
#include <lydia/parser/ltlf/driver.cpp>
#include <lydia/to_dfa/core.hpp>
#include <lydia/to_dfa/strategies/compositional/base.hpp>
#include <lydia/utils/print.hpp>
#include <sstream>

#include "String_utilities.h"

#include "cudd.h"

namespace Syft{

    void ExplicitStateDfaMona::dfa_print() {
        std::cout<< "Number of states " +
                    std::to_string(get_nb_states())<<"\n";



        std::cout<<"Computed automaton: ";
        whitemech::lydia::dfaPrint(get_dfa(),
                                   get_nb_variables(),
                                   names, indices.data());
    }




    ExplicitStateDfaMona ExplicitStateDfaMona::dfa_of_formula(const std::string& formula) {
//        std::string formula_formatted = boost::algorithm::replace_all_copy(formula, "X", "X[!]");

        std::shared_ptr<whitemech::lydia::AbstractDriver> driver;
        driver = std::make_shared<whitemech::lydia::parsers::ltlf::LTLfDriver>();

        std::stringstream formula_stream(formula);
//        logger.info("Parsing {}", formula);
        driver->parse(formula_stream);
        auto parsed_formula = driver->get_result();

//        logger.info("Apply no-empty semantics.");
        auto context = driver->context;
        auto end = context->makeLdlfEnd();
        auto not_end = context->makeLdlfNot(end);
        parsed_formula = context->makeLdlfAnd({parsed_formula, not_end});

        auto dfa_strategy = whitemech::lydia::CompositionalStrategy();
        auto translator = whitemech::lydia::Translator(dfa_strategy);


        auto t_start = std::chrono::high_resolution_clock::now();

//        logger.info("Transforming to DFA...");
        auto t_dfa_start = std::chrono::high_resolution_clock::now();


        auto my_dfa = translator.to_dfa(*parsed_formula);

        auto my_mona_dfa =
                std::dynamic_pointer_cast<whitemech::lydia::mona_dfa>(my_dfa);

        DFA* d = dfaCopy(my_mona_dfa->dfa_);

        ExplicitStateDfaMona exp_dfa(d, my_mona_dfa->names);


//        std::cout<< "Number of states " +
//                    std::to_string(exp_dfa.get_nb_variables())<<"\n";


        return exp_dfa;
    }






}
