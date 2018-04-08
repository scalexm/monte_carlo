#ifndef DETAIL_STOCHASTIC_GRADIENT_HPP
#define DETAIL_STOCHASTIC_GRADIENT_HPP

#include <tuple>
#include <algorithm> // `std::max`

namespace detail {

// Fonction $H1$ de l'article.
inline auto H1(double xi, double x, double alpha) -> double {
    if (x < xi)
        return 1;
    return 1 - 1 / (1 - alpha);
}

// Fonction $v$ de l'article, définie dans la proposition 2.1.
inline auto v(double xi, double x, double alpha) -> double {
    return xi + 1 / (1 - alpha) * std::max(x - xi, 0.0);
}

// Encapsule la suite $(\xi_n, C_n)$ de l'article pour l'algorithme naïf de gradient stochastique
// de la section 2.2. On prendra toujours $\beta_n = \gamma_n$ pour simplifier.
template<class Phi, class Gamma, class Distribution, class Generator>
class approx_sequence {
    private:
        const Phi & phi;
        double alpha, xi = 0, C = 0; // On choisit $\xi_0 = C_0 = 0$.
        const Gamma & gamma;
        int n = 0;

        Distribution & d;
        Generator & g;

    public:
        using result_type = std::tuple<double, double>;

        approx_sequence(
            double alpha,
            const Phi & phi,
            const Gamma & gamma,
            Distribution & d,
            Generator & g
        ) : alpha { alpha }, phi { phi }, gamma { gamma }, d { d }, g { g }
        {
        }

        auto next() -> result_type {
            if (n == 0) {
                ++n;
                return std::make_tuple(xi, C);
            }

            double x = phi(d(g));
            C -= gamma(n) * (C - v(xi, x, alpha));
            xi -= gamma(n) * H1(xi, x, alpha);
            ++n;
            return std::make_tuple(xi, C);
        }
};

}

#endif
