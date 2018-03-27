#ifndef VAR_HPP
#define VAR_HPP

#include "detail/var_sequence.hpp"

namespace gamma_sequences {

    // $n \longmapsto \frac{1}{n}$
    template<class Float>
    auto inverse(int n) -> Float {
        return 1 / static_cast<Float>(n);
    }

}

// Noyau de calcul de la V@R qui suit l'approche récursive du poly présentée page 175.
// En pratique, ne marche bien que pour un niveau de confiance proche de 1/2.
template<class Float = double, class Gamma = decltype(gamma_sequences::inverse<Float>)>
class var_seq_kernel {
    private:
        Gamma & gamma;
        Float alpha;
        int iterations;

    public:
        // Paramètres du constructeur:
        // - `alpha`: niveau de confiance
        // - `iterations`: nombre d'itérations de l'algorithme
        // - `gamma`: foncteur `int -> float`, `gamma(n)` représentant la suite $\gamma_n$ du poly
        var_seq_kernel(
            Float alpha,
            int iterations,
            Gamma & gamma = gamma_sequences::inverse<Float>
        ) : alpha { alpha }, iterations { iterations }, gamma { gamma }
        {
        }

        var_seq_kernel(const var_seq_kernel &) = default;
        var_seq_kernel(var_seq_kernel &&) = default;
        auto operator =(const var_seq_kernel &) -> var_seq_kernel & = default;
        auto operator =(var_seq_kernel &&) -> var_seq_kernel & = default;

        // Paramètres génériques d'un noyau de calcul:
        // - `d`: foncteur `Generator -> Float` représentant la distribution de $X$, usuellement
        //   on prend un objet défini dans le header <random>
        // - `g`: générateur de nombre aléatoires, usuellement on prend aussi un objet défini dans
        //   le header <random>
        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> Float {
            auto sequence = detail::var_sequence<Float> { alpha };

            Float state;
            for (int n = 0; n < iterations; ++n)
                state = sequence.next(d, g, gamma);
            return state;
        }
};

#endif
