#ifndef EXPLICITSTATEDFAMONA_H
#define EXPLICITSTATEDFAMONA_H

extern "C" {
#include <mona/bdd.h>
#include <mona/dfa.h>
#include <mona/mem.h>
}

#include "lydia/dfa/mona_dfa.hpp"
#include "VarMgr.h"

namespace Syft {

/*
 * Wrapper to Lydia DFA.
 */
    class ExplicitStateDfaMona : public whitemech::lydia::mona_dfa {
    public:

        ExplicitStateDfaMona(DFA* dfa, int nb_variables)
        : whitemech::lydia::mona_dfa(dfa, nb_variables) {
        }

        ExplicitStateDfaMona(DFA* dfa, const std::vector<std::string>& names)
                : whitemech::lydia::mona_dfa(dfa, names) {
        }

        ExplicitStateDfaMona(const ExplicitStateDfaMona& other)
        : whitemech::lydia::mona_dfa(dfaCopy(other.dfa_), other.names) {

        }

        ExplicitStateDfaMona& operator=(ExplicitStateDfaMona other)
        {
//            std::cout << "copy assignment of ExplicitStateDfaMona\n";
            this->names = other.names;
            this->dfa_ = dfaCopy(other.get_dfa());
            return *this;
        }
        // ExplicitStateDfaMona& operator=(const ExplicitStateDfaMona& other);

        ~ExplicitStateDfaMona(){
            
        }

        // rewrite this function, since nb_variables_ in mona_dfa is private
        int get_nb_variables() {
            return this->names.size();
        }

        /**
         * \brief Print the DFA.
         */
        void dfa_print();

        /**
         * \brief Construct DFA from a given formula
         *
         *
         * \param formula An LTLf formula.
         * \return The corresponding explicit-state DFA.
         */
        static ExplicitStateDfaMona dfa_of_formula(const std::string& formula);




    };

}
#endif //EXPLICITSTATEDFAMONA_H
