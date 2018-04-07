#ifndef DETAIL_CVAR_SEQUENCES_HPP
#define DETAIL_CVAR_SEQUENCES_HPP

#include "var_sequences.hpp"
#include <algorithm> // `std::max`

namespace detail {
namespace cvar {

// Fonction $\Lambda$ du poly (page 176).
template<class Float>
auto Lambda(Float state, Float x, Float alpha) -> Float {
    return state + 1 / (1 - alpha) * std::max(x - state, static_cast<Float>(0));
}

// Encapsule la suite $C_n$ du poly (page 176).
template<class Float, class Gamma, class Beta>
class approx_sequence {
    private:
        Float alpha, C = 0;
        const Beta & beta;
        
        // On calcule $\xi_n$ à la volée.
        detail::var::approx_sequence<Float, Gamma> xi;
        int n = 0;

    public:
        using result_type = Float;

        approx_sequence(Float alpha, const Gamma & gamma, const Beta & beta) :
            alpha { alpha }, xi { alpha, gamma }, beta { beta }
        {
        }

        template<class Distribution, class Generator>
        auto next(Distribution & d, Generator & g) -> result_type {
            if (n == 0) {
                ++n;
                return C;
            }
            auto lambda = Lambda(xi.next(d, g), d(g), alpha);
            C -= beta(n) * (C - lambda);
            ++n;
            return C;
        }
};

}
}

#endif
