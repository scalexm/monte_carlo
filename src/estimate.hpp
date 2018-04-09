#ifndef ESTIMATE_HPP
#define ESTIMATE_HPP

#include "detail/stochastic_gradient.hpp"
#include "detail/importance_sampling.hpp"
#include "detail/iterate.hpp"
#include "detail/averaging.hpp"
#include "steps.hpp"

enum class averaging {
    yes,
    no,
};

// Calcul de la VaR et de la CVaR qui suit l'approche par gradient stochastique présentée en
// section 2.2. Pour simplifier, on n'offre pas la possibilité de calculer la $\Psi$-CVaR,
// ou bien pour résumer on fixe $\Psi = Id$.
// En pratique, ne converge bien que pour un niveau de confiance proche de 1/2.
template<class Phi, class Gamma>
class approx_kernel {
    private:
        const Phi & phi;
        const Gamma & gamma;
        double alpha;
        averaging avg;
        int iterations;

    public:
        // Paramètres du constructeur:
        // - `alpha`: niveau de confiance
        // - `phi`: foncteur `* -> double` représentant la fonction de perte $\phi$
        // - `gamma`: foncteur `int -> double`, `gamma(n)` représentant la suite $\gamma_n$
        //            de l'article
        // - `avg`: appliquer ou non la moyennisation de Ruppert et Polyak (théorème 2.3)
        // - `iterations`: nombre d'itérations de l'algorithme
        approx_kernel(
            double alpha,
            const Phi & phi,
            const Gamma & gamma,
            averaging avg,
            int iterations
        ) :
            alpha { alpha }, phi { phi }, gamma { gamma }, avg { avg },
            iterations { iterations }
        {
        }

        // Paramètres génériques d'un noyau de calcul:
        // - `d`: foncteur `Generator -> *` représentant la distribution de $X$,
        //        usuellement on prend un objet défini dans le header <random>
        // - `g`: générateur de nombre aléatoires, usuellement on prend aussi un objet
        //        défini dans le header <random>
        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> std::pair<double, double> {
            auto seq = detail::approx_sequence<Phi, Gamma, Distribution, Generator> {
                alpha,
                phi,
                gamma,
                d,
                g
            };

            if (avg == averaging::no) {
                return detail::iterate(seq, iterations);
            } else {
                auto avg_seq = detail::averaging<decltype(seq)> { std::move(seq) };
                return detail::iterate(avg_seq, iterations);
            }
        }
};

template<class Phi, class Gamma>
class IS_kernel {
    private:
        const Phi & phi;
        const Gamma & gamma;
        double alpha, a;
        averaging avg;
        int iterations;
    
    public:
        IS_kernel(
            double alpha,
            double a,
            const Phi & phi,
            const Gamma & gamma,
            averaging avg,
            int iterations
        ) :
            alpha { alpha }, a { a }, phi { phi }, gamma { gamma }, avg { avg },
            iterations { iterations }
        {
        }

        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> std::pair<double, double> {
            auto M = iterations / 100;
            auto phase1 = detail::IS_phase1_sequence<Phi, Gamma, Distribution, Generator> {
                alpha,
                a,
                phi,
                gamma,
                M,
                d,
                g
            };

            auto phase1_result = detail::iterate(phase1, M);

            auto phase2 = detail::IS_phase2_sequence<Phi, Gamma, Distribution, Generator> {
                alpha,
                std::get<0>(phase1_result),
                std::get<1>(phase1_result),
                std::get<2>(phase1_result),
                phi,
                gamma,
                d,
                g
            };

            if (avg == averaging::no) {
                return detail::iterate(phase2, iterations);
            } else {
                auto avg_seq = detail::averaging<decltype(phase2)> { std::move(phase2) };
                return detail::iterate(avg_seq, iterations);
            }
        }
};

inline auto identity(double x) -> double {
    return x;
}

template<
    class Phi = decltype(identity),
    class Gamma = decltype(steps::inverse)
>
auto stochastic_gradient(
    double alpha,
    int iterations,
    const Phi & phi = identity,
    const Gamma & gamma = steps::inverse, // par défaut, on prend $\gamma_n = \frac{1}{n}$
    averaging avg = averaging::no
) -> approx_kernel<Phi, Gamma>
{
    return approx_kernel<Phi, Gamma> { alpha, phi, gamma, avg, iterations };
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
    const Gamma & gamma = steps::inverse,
    averaging avg = averaging::no
) -> IS_kernel<Phi, Gamma>
{
    return IS_kernel<Phi, Gamma> {
        alpha,
        a,
        phi,
        gamma,
        avg,
        iterations,
    };
}

#endif
