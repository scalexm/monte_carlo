#ifndef DETAIL_CVAR_SEQUENCE_HPP
#define DETAIL_CVAR_SEQUENCE_HPP

#include "var_sequence.hpp"
#include <algorithm> // `std::max`

namespace detail {

// Fonction $\Lambda$ du poly (page 176).
template<class Float>
auto Lambda(Float state, Float x, Float alpha) -> Float {
    return state + 1 / (1 - alpha) * std::max(x - state, static_cast<Float>(0));
}

// Encapsule la suite $C_n$ du poly (page 176).
template<class Float, class Gamma, class Beta>
class cvar_sequence {
    private:
        Float alpha, state = 0;
        const Beta & beta;
        
        // On calcule $\xi_n$ à la volée.
        detail::var_sequence<Float, Gamma> var_state;
        int n = 0;

    public:
        using result_type = Float;

        cvar_sequence(Float alpha, const Gamma & gamma, const Beta & beta) :
            alpha { alpha }, var_state { alpha, gamma }, beta { beta }
        {
        }

        template<class Distribution, class Generator>
        auto next(Distribution & d, Generator & g) -> result_type {
            if (n == 0) {
                ++n;
                return state;
            }
            auto lambda = Lambda(var_state.next(d, g), d(g), alpha);
            state -= beta(n) * (state - lambda);
            ++n;
            return state;
        }
};

}

#endif
