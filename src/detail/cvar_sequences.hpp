#ifndef DETAIL_CVAR_SEQUENCES_HPP
#define DETAIL_CVAR_SEQUENCES_HPP

#include "var_sequences.hpp"
#include <algorithm> // `std::max`

namespace detail {
namespace cvar {

// Fonction $v$ de l'article, définie dans la proposition 2.1.
template<class RealType>
auto v(RealType xi, RealType x, RealType alpha) -> RealType {
    return xi + 1 / (1 - alpha) * std::max(x - xi, static_cast<RealType>(0));
}

// Encapsule la suite $C_n$ de l'article pour l'algorithme naïf de gradient stochastique de la
// section 2.2. On prendra toujours $\beta_n = \gamma_n$ pour simplifier.
template<class RealType, class Gamma>
class approx_sequence {
    private:
        RealType alpha, C = 0; // Idem que pour $\xi_0$, on choisit $C_0 = 0$.
        const Gamma & gamma;
        
        // On calcule $\xi_n$ à la volée.
        detail::var::approx_sequence<RealType, Gamma> xi;
        int n = 0;

    public:
        using result_type = RealType;

        approx_sequence(RealType alpha, const Gamma & gamma) :
            alpha { alpha }, xi { alpha, gamma }, gamma { gamma }
        {
        }

        template<class Distribution, class Generator>
        auto next(Distribution & d, Generator & g) -> result_type {
            if (n == 0) {
                ++n;
                return C;
            }

            C -= gamma(n) * (C - v(xi.next(d, g), d(g), alpha));
            ++n;
            return C;
        }
};

}
}

#endif
