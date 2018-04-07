#ifndef ESTIMATE_HPP
#define ESTIMATE_HPP

#include "detail/stochastic_gradient.hpp"
#include "detail/importance_sampling.hpp"
#include "detail/iterate.hpp"
#include "detail/averaging.hpp"
#include "steps.hpp"
#include "averaging.hpp"

// Calcul de la VaR et de la CVaR qui suit l'approche par gradient stochastique présentée en
// section 2.2. Pour simplifier, on n'offre pas la possibilité de calculer la $\Psi$-CVaR,
// ou bien pour résumer on fixe $\Psi = Id$.
// En pratique, ne converge bien que pour un niveau de confiance proche de 1/2.
template<class Gamma>
class approx_kernel {
    private:
        const Gamma & gamma;
        double alpha;
        Averaging avg;
        int iterations;

    public:
        // Paramètres du constructeur:
        // - `alpha`: niveau de confiance
        // - `gamma`: foncteur `int -> double`, `gamma(n)` représentant la suite $\gamma_n$
        //            de l'article
        // - `avg`: appliquer ou non la moyennisation de Ruppert et Polyak (théorème 2.3)
        // - `iterations`: nombre d'itérations de l'algorithme
        approx_kernel(double alpha, const Gamma & gamma, Averaging avg, int iterations) :
            alpha { alpha }, gamma { gamma }, avg { avg }, iterations { iterations }
        {
        }

        // Paramètres génériques d'un noyau de calcul:
        // - `d`: foncteur `Generator -> double` représentant la distribution de $\phi(X)$,
        //        usuellement on prend un objet défini dans le header <random>, ou du moins
        //        une fonction appliquée à un tel objet
        // - `g`: générateur de nombre aléatoires, usuellement on prend aussi un objet défini
        //        dans le header <random>
        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> std::pair<double, double> {
            auto sequence = detail::approx_sequence<Gamma> { alpha, gamma };
            if (avg == Averaging::No) {
                return detail::iterate(sequence, iterations, d, g);
            } else {
                auto avg_sequence = detail::averaging<decltype(sequence)> { std::move(sequence) };
                return detail::iterate(avg_sequence, iterations, d, g);
            }
        }
};

template<class Gamma = decltype(steps::inverse)>
inline auto stochastic_gradient(
    double alpha,
    int iterations,
    const Gamma & gamma = steps::inverse, // par défaut, on prend $\gamma_n = \frac{1}{n}$
    Averaging avg = Averaging::No
) -> approx_kernel<Gamma>
{
    return approx_kernel<Gamma> { alpha, gamma, avg, iterations };
}

template<class Phi, class Gamma>
class IS_kernel {
    private:
        const Phi & phi;
        const Gamma & gamma;
        double alpha, a;
        int iterations;
    
    public:
        IS_kernel(
            double alpha,
            double a,
            const Phi & phi,
            const Gamma & gamma,
            int iterations
        ) :
            alpha { alpha }, a { a }, phi { phi }, gamma { gamma }, iterations { iterations }
        {
        }

        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> std::pair<double, double> {
            auto M = iterations / 100;
            auto phase1 = detail::IS_phase1_sequence<Phi, Gamma> {
                alpha,
                a,
                phi,
                gamma,
                M
            };

            auto phase1_result = detail::iterate(phase1, M, d, g);

            auto phase2 = detail::IS_phase2_sequence<Phi, Gamma> {
                alpha,
                std::get<0>(phase1_result),
                std::get<1>(phase1_result),
                phi,
                gamma
            };

            return detail::iterate(phase2, iterations, d, g);
        }
};

auto identity(double x) -> double {
    return x;
}

template<
    class Phi = decltype(identity),
    class Gamma = decltype(steps::inverse)
>
auto importance_sampling(
    double alpha,
    double a,
    int iterations,
    const Phi & phi = identity,
    const Gamma & gamma = steps::inverse
) -> IS_kernel<Phi, Gamma>
{
    return IS_kernel<Phi, Gamma> {
        alpha,
        a,
        phi,
        gamma,
        iterations
    };
}

#endif
