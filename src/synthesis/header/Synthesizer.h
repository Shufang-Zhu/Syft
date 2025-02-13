#ifndef SYNTHESIZER_H
#define SYNTHESIZER_H

#include <memory>

#include "Transducer.h"
#include <tuple>

namespace Syft {
    struct SynthesisResult{
        bool realizability;
        CUDD::BDD winning_states;
        CUDD::BDD winning_moves;
        std::unique_ptr<Transducer> transducer;
    };

/**
 * \brief Abstract class for synthesizers.
 *
 * Can be inherited to implement synthesizers for different specification types.
 */
template <class Spec>
class Synthesizer {
 protected:

  Spec spec_;
  
 public:

  Synthesizer(Spec spec)
    : spec_(std::move(spec))
    {}

  virtual ~Synthesizer()
    {}

  /**
   * \brief Solves the synthesis problem of the specification.
   *
   * \return The result consists of
   * realizability
   * a set of agent winning states
   * a transducer representing a winning strategy for the specification or nullptr if the specification is unrealizable.
   */
  virtual SynthesisResult run() const = 0;
};

}

#endif // SYNTHESIZER_H
