#ifndef VAR_HPP
#define VAR_HPP

#include "detail/var_sequence.hpp"
#include "detail/iterate.hpp"
#include "detail/averaging.hpp"
#include "steps.hpp"
#include "averaging.hpp"

namespace var {

// Noyau de calcul de la V@R qui suit l'approche récursive du poly présentée page 175.
// En pratique, ne marche bien que pour un niveau de confiance proche de 1/2.
template<class Float, class Gamma>
class sequential_kernel {
    private:
        Gamma & gamma;
        Float alpha;
        Averaging avg;
        int iterations;

    public:
        // Paramètres du constructeur:
        // - `alpha`: niveau de confiance
        // - `iterations`: nombre d'itérations de l'algorithme
        // - `gamma`: foncteur `int -> float`, `gamma(n)` représentant la suite $\gamma_n$ du poly
        sequential_kernel(Float alpha, Gamma & gamma, Averaging avg, int iterations) :
            alpha { alpha }, gamma { gamma }, avg { avg }, iterations { iterations }
        {
        }

        // Paramètres génériques d'un noyau de calcul:
        // - `d`: foncteur `Generator -> Float` représentant la distribution de $X$, usuellement
        //   on prend un objet défini dans le header <random>
        // - `g`: générateur de nombre aléatoires, usuellement on prend aussi un objet défini dans
        //   le header <random>
        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> Float {
            auto sequence = detail::var_sequence<Float, Gamma> { alpha, gamma };
            if (avg == Averaging::No) {
                return detail::iterate(sequence, iterations, d, g);
            } else {
                auto avg_sequence = detail::averaging<decltype(sequence)> { std::move(sequence) };
                return detail::iterate(avg_sequence, iterations, d, g);
            }
        }
};

template<class Float = double, class Gamma = decltype(steps::inverse<Float>)>
inline auto sequential(
    Float alpha,
    int iterations,
    Averaging avg = Averaging::No,
    Gamma & gamma = steps::inverse<Float>
) -> sequential_kernel<Float, Gamma>
{
    return sequential_kernel<Float, Gamma> { alpha, gamma, avg, iterations };
}

}

#endif
