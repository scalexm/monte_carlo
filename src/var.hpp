#ifndef VAR_HPP
#define VAR_HPP

#include "detail/var_sequences.hpp"
#include "detail/iterate.hpp"
#include "detail/averaging.hpp"
#include "steps.hpp"
#include "averaging.hpp"

namespace var {

// Noyau de calcul de la V@R qui suit l'approche par gradient stochastique présentée en
// section 2.2.
// En pratique, ne marche bien que pour un niveau de confiance proche de 1/2.
template<class RealType, class Gamma>
class approx_kernel {
    private:
        const Gamma & gamma;
        RealType alpha;
        Averaging avg;
        int iterations;

    public:
        // Paramètres du constructeur:
        // - `alpha`: niveau de confiance
        // - `gamma`: foncteur `int -> RealType`, `gamma(n)` représentant la suite $\gamma_n$
        //            de l'article
        // - `avg`: appliquer ou non la moyennisation de Ruppert et Polyak (théorème 2.3)
        // - `iterations`: nombre d'itérations de l'algorithme
        approx_kernel(RealType alpha, const Gamma & gamma, Averaging avg, int iterations) :
            alpha { alpha }, gamma { gamma }, avg { avg }, iterations { iterations }
        {
        }

        // Paramètres génériques d'un noyau de calcul:
        // - `d`: foncteur `Generator -> RealType` représentant la distribution de $\phi(X)$,
        //        usuellement on prend un objet défini dans le header <random>, ou du moins
        //        une fonction appliquée à un tel objet
        // - `g`: générateur de nombre aléatoires, usuellement on prend aussi un objet défini
        //        dans le header <random>
        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> RealType {
            auto sequence = detail::var::approx_sequence<RealType, Gamma> { alpha, gamma };
            if (avg == Averaging::No) {
                return detail::iterate(sequence, iterations, d, g);
            } else {
                auto avg_sequence = detail::averaging<decltype(sequence)> { std::move(sequence) };
                return detail::iterate(avg_sequence, iterations, d, g);
            }
        }
};

template<class RealType = double, class Gamma = decltype(steps::inverse<RealType>)>
inline auto stochastic_approx(
    RealType alpha,
    int iterations,
    Averaging avg = Averaging::No,

    // par défaut, on prend $\gamma_n = \frac{1}{n}$
    Gamma & gamma = steps::inverse<RealType>
) -> approx_kernel<RealType, Gamma>
{
    return approx_kernel<RealType, Gamma> { alpha, gamma, avg, iterations };
}

}

#endif
