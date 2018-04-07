#ifndef CVAR_HPP
#define CVAR_HPP

#include "detail/cvar_sequences.hpp"
#include "detail/iterate.hpp"
#include "detail/averaging.hpp"
#include "steps.hpp"
#include "averaging.hpp"

namespace cvar {

// Noyau de calcul de la CV@R qui suit l'approche décrite page 176 où on se sert du calcul
// de la V@R à la volée.
// En pratique, ne marche bien que pour un niveau de confiance proche de 1/2.
template<class Float, class Gamma, class Beta>
class approx_kernel {
    private:
        const Gamma & gamma;
        const Beta & beta;
        Float alpha;
        Averaging avg;
        int iterations;

    public:
        // Cf `var.hpp/var_seq_kernel::var_seq_kernel` (mêmes paramètres).
        approx_kernel(
            Float alpha,
            const Gamma & gamma,
            const Beta & beta,
            Averaging avg,
            int iterations
        ) :
            alpha { alpha }, gamma { gamma }, beta { beta }, avg { avg },
            iterations { iterations }
        {
        }

        // Idem, voir les paramètres "génériques" décrits dans `var.hpp`.
        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> Float {
            auto sequence = detail::cvar::approx_sequence<Float, Gamma, Beta> {
                alpha,
                gamma,
                beta
            };
            if (avg == Averaging::No) {
                return detail::iterate(sequence, iterations, d, g);
            } else {
                auto avg_sequence = detail::averaging<decltype(sequence)> { std::move(sequence) };
                return detail::iterate(avg_sequence, iterations, d, g);
            }
        }
};

template<
    class Float = double,
    class Gamma = decltype(steps::inverse<Float>),
    class Beta = Gamma
>
inline auto stochastic_approx(
    Float alpha,
    int iterations,
    Averaging avg = Averaging::No,
    const Gamma & gamma = steps::inverse<Float>,
    const Beta & beta = steps::inverse<Float>
) -> approx_kernel<Float, Gamma, Beta>
{
    return approx_kernel<Float, Gamma, Beta> { alpha, gamma, beta, avg, iterations };
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
