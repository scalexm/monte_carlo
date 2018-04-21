#ifndef ESTIMATE_HPP
#define ESTIMATE_HPP

#include "detail/stochastic_gradient.hpp"
#include "detail/importance_sampling.hpp"
#include "detail/iterate.hpp"
#include "detail/averaging.hpp"
#include "steps.hpp"
#include "averaging.hpp"

// Calcul de la V@R et de la CV@R qui suit l'approche par gradient stochastique présentée en
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
        // * `alpha`: niveau de confiance
        // * `phi`: foncteur `* -> double` représentant la fonction de perte $\phi$
        // * `gamma`: foncteur `int -> double`, `gamma(n)` représentant la suite $\gamma_n$
        //            de l'article
        // * `avg`: appliquer ou non la moyennisation de Ruppert et Polyak (théorème 2.3)
        // * `iterations`: nombre d'itérations de l'algorithme
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
        // * `d`: foncteur `Generator -> *` représentant la distribution de $X$,
        //        usuellement on prend un objet défini dans le header <random>
        // * `g`: générateur de nombre aléatoires, usuellement on prend aussi un objet
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

            std::tuple<double, double> result;
            if (avg == averaging::no) {
                result = detail::iterate(seq, iterations);
            } else {
                auto avg_seq = detail::averaging<decltype(seq)> { std::move(seq) };
                result = detail::iterate(avg_seq, iterations);
            }
            return std::make_pair(std::get<0>(result), std::get<1>(result));
        }
};

// Calcul de la V@R et CV@R avec la technique d'importance sampling de la section 3.
template<class Phi, class Gamma>
class IS_kernel {
    private:
        const Phi & phi;
        const Gamma & gamma;
        double alpha, a;
        averaging avg;
        int iterations;
    
    public:
        // Paramètres du constructeur:
        // * `alpha`, `phi`, `gamma`, `avg`, `gamma`: cf `approx_kernel::approx_kernel`
        // * `a`: paramètre du contrôle exponentiel sur $x \longmapsto \phi^2(x)$
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

        // Paramètres génériques d'un noyau de calcul: cf `approx_kernel::compute`.
        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> std::pair<double, double> {
            // On fixe le nombre d'itérations pour la première phase à `iterations / 100`.
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

            // On réinjecte les paramètres estimés dans la première phase pour la deuxième phase.
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

            std::tuple<double, double> result;
            if (avg == averaging::no) {
                result = detail::iterate(phase2, iterations);
            } else {
                auto avg_seq = detail::averaging<decltype(phase2)> { std::move(phase2) };
                result = detail::iterate(avg_seq, iterations);
            }
            return std::make_pair(std::get<0>(result), std::get<1>(result));
        }
};

inline auto identity(double x) -> double {
    return x;
}

// Fonction utilitaire pour ne pas à avoir à écrire explicitement les paramètres template
// de la classe `approx_kernel`: en effet, le compilateur peut inférer les paramètres template
// d'une fonction (mais pas d'une classe).
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

// Cf plus haut, idem mais pour `IS_kernel`.
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
