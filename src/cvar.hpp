#ifndef CVAR_HPP
#define CVAR_HPP

#include "detail/cvar_sequence.hpp"
#include "detail/iterate.hpp"
#include "gamma_sequences.hpp"

namespace cvar {

// Noyau de calcul de la CV@R qui suit l'approche décrite page 176 où on se sert du calcul
// de la V@R à la volée.
// En pratique, ne marche bien que pour un niveau de confiance proche de 1/2.
template<class Float, class Gamma, class Beta>
class sequential_kernel {
    private:
        Gamma & gamma;
        Beta & beta;
        Float alpha;
        int iterations;

    public:
        // Cf `var.hpp/var_seq_kernel::var_seq_kernel` (mêmes paramètres).
        sequential_kernel(Float alpha, Gamma & gamma, Beta & beta, int iterations) :
            alpha { alpha }, gamma { gamma }, beta { beta }, iterations { iterations }
        {
        }

        // Idem, voir les paramètres "génériques" décrits dans `var.hpp`.
        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> Float {
            auto sequence = detail::cvar_sequence<Float, Gamma, Beta> { alpha, gamma, beta };
            return detail::iterate(sequence, iterations, d, g);
        }
};

template<
    class Float = double,
    class Gamma = decltype(gamma_sequences::inverse<Float>),
    class Beta = Gamma
>
inline auto sequential(
    Float alpha,
    int iterations,
    Gamma & gamma = gamma_sequences::inverse<Float>,
    Beta & beta = gamma_sequences::inverse<Float>
) -> sequential_kernel<Float, Gamma, Beta>
{
    return sequential_kernel<Float, Gamma, Beta> { alpha, gamma, beta, iterations };
}

// Noyau de calcul de la CV@R par Monte Carlo. Nécessite une estimation de la V@R.
template<class Float>
class monte_carlo_kernel {
    private:
        Float alpha, estimated_var;
        int iterations;

    public:
        monte_carlo_kernel(Float alpha, Float estimated_var, int iterations) :
            alpha { alpha }, estimated_var { estimated_var }, iterations { iterations }
        {
        }

        // Paramètres génériques d'un noyau de calcul (cf `var.hpp`).
        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> Float {
            Float sum = 0;
            int n = 0;
            while (n < iterations) {
                Float x = d(g);
                if (x < estimated_var)
                    continue;
                ++n;
                sum += x;
            }
            return sum / iterations;
        }
};

template<class Float = double>
inline auto monte_carlo(
    Float alpha,
    Float estimated_var,
    int iterations
) -> monte_carlo_kernel<Float>
{
    return monte_carlo_kernel<Float> { alpha, estimated_var, iterations };
}

}

#endif
