#include "var.hpp"
#include "detail/cvar_sequence.hpp"

// Noyau de calcul de la CV@R qui suit l'approche décrite page 176 où on se sert du calcul
// de la V@R à la volée.
// En pratique, ne marche bien que pour un niveau de confiance proche de 1/2.
template<class Float = double, class Gamma = decltype(gamma_sequences::inverse<Float>)>
class cvar_seq_kernel {
    private:
        Gamma & gamma;
        Float alpha;
        int iterations;

    public:
        // Cf `var.hpp/var_seq_kernel::var_seq_kernel` (mêmes paramètres).
        cvar_seq_kernel(
            Float alpha,
            int iterations,
            Gamma & gamma = gamma_sequences::inverse<Float>
        ) : alpha { alpha }, iterations { iterations }, gamma { gamma }
        {
        }

        cvar_seq_kernel(const cvar_seq_kernel &) = default;
        cvar_seq_kernel(cvar_seq_kernel &&) = default;
        auto operator =(const cvar_seq_kernel &) -> cvar_seq_kernel & = default;
        auto operator =(cvar_seq_kernel &&) -> cvar_seq_kernel & = default;

        // Idem, voir les paramètres "génériques" décrits dans `var.hpp`.
        template<class Distribution, class Generator>
        auto compute(Distribution & d, Generator & g) -> Float {
            auto sequence = detail::cvar_sequence<Float> { alpha };

            Float state;
            for (int n = 0; n < iterations; ++n)
                state = sequence.next(d, g, gamma);
            return state;
        }
};

// Noyau de calcul de la CV@R par Monte Carlo. Nécessite une estimation de la V@R.
template<class Float = double>
class cvar_monte_carlo_kernel {
    private:
        Float alpha, estimated_var;
        int iterations;

    public:
        cvar_monte_carlo_kernel(
            Float alpha,
            Float estimated_var,
            int iterations
        ) : alpha { alpha }, estimated_var { estimated_var }, iterations { iterations }
        {
        }

        cvar_monte_carlo_kernel(const cvar_monte_carlo_kernel &) = default;
        cvar_monte_carlo_kernel(cvar_monte_carlo_kernel &&) = default;
        auto operator =(const cvar_monte_carlo_kernel &) -> cvar_monte_carlo_kernel & = default;
        auto operator =(cvar_monte_carlo_kernel &&) -> cvar_monte_carlo_kernel & = default;

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
