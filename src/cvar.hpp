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
template<class RealType, class Gamma, class Beta>
class approx_kernel {
    private:
        const Gamma & gamma;
        const Beta & beta;
        RealType alpha;
        Averaging avg;
        int iterations;

    public:
        // Cf `var.hpp/var_seq_kernel::var_seq_kernel` (mêmes paramètres).
        approx_kernel(
            RealType alpha,
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
        auto compute(Distribution & d, Generator & g) -> RealType {
            auto sequence = detail::cvar::approx_sequence<RealType, Gamma, Beta> {
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
    class RealType = double,
    class Gamma = decltype(steps::inverse<RealType>),
    class Beta = Gamma
>
inline auto stochastic_approx(
    RealType alpha,
    int iterations,
    Averaging avg = Averaging::No,
    const Gamma & gamma = steps::inverse<RealType>,
    const Beta & beta = steps::inverse<RealType>
) -> approx_kernel<RealType, Gamma, Beta>
{
    return approx_kernel<RealType, Gamma, Beta> { alpha, gamma, beta, avg, iterations };
}

// Noyau de calcul de la CV@R par Monte Carlo. Nécessite une estimation de la V@R.
template<class RealType>
class monte_carlo_kernel {
    private:
        RealType alpha, estimated_var;
        int iterations;

    public:
        monte_carlo_kernel(RealType alpha, RealType estimated_var, int iterations) :
            alpha { alpha }, estimated_var { estimated_var }, iterations { iterations }
        {
        }

        // Paramètres génériques d'un noyau de calcul (cf `var.hpp`).
        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> RealType {
            RealType sum = 0;
            int n = 0;
            while (n < iterations) {
                RealType x = d(g);
                if (x < estimated_var)
                    continue;
                ++n;
                sum += x;
            }
            return sum / iterations;
        }
};

template<class RealType = double>
inline auto monte_carlo(
    RealType alpha,
    RealType estimated_var,
    int iterations
) -> monte_carlo_kernel<RealType>
{
    return monte_carlo_kernel<RealType> { alpha, estimated_var, iterations };
}

}

#endif
