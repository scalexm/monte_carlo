#ifndef DETAIL_IMPORTANCE_SAMPLING_HPP
#define DETAIL_IMPORTANCE_SAMPLING_HPP

#include "importance_sampling_parameters.hpp"
#include <tuple>

namespace detail {

// Fonction $L3$ de l'article, définie dans la section 3.1.
// En plus de $\xi$, $\theta$ et $x$, on prend aussi en argument les paramètres $\rho$,
// $b$ etc de la distribution choisie via un objet de type `IS_params<Distribution>`.
template<class InputType, class Phi, class Distribution>
auto L3(
    double xi,
    InputType theta,
    InputType x,
    const Phi & phi,
    IS_params<Distribution> p
) -> double {
    if (phi(x - theta) < xi)
        return 0;
    return std::exp(-2 * p.rho() * std::pow(std::abs(theta), p.b())) * p.W(x, theta);
}

// Fonction $L4$ de l'article, définie dans la section 3.1, avec un contrôle
// exponentiel sur $x \longmapsto \phi^2(x)$ de la forme $C e^{a|x|}$, la constante
// $a$ étant décrite ici par le paramètre du même nom.
template<class InputType, class Phi, class Distribution>
auto L4(
    double xi,
    const InputType & mu,
    const InputType & x,
    double a,
    const Phi & phi,
    IS_params<Distribution> p
) -> double {
    auto diff =  phi(x - mu) - xi;
    auto norm = std::abs(mu);
    return std::exp(-2 * a * (norm * norm + 1)) * L3(xi, mu, x, phi, p) * diff * diff;
}

// Phase 1 de l'algorithme d'importance sampling: calcule une première estimation
// de (\xi_\alpha^*, \theta^*, \mu^*) en faisant `M` itérations avec un niveau `alpha`
// adaptatif.
template<class Phi, class Gamma, class Distribution, class Generator>
class IS_phase1_sequence {
    private:
        const Phi & phi;
        using input_type = decltype(phi(0));

        double alpha, a, xi = 0;
        input_type theta { }, mu { };
        const Gamma & gamma;
        int M;
        int n = 0;

        Distribution & d;
        Generator & g;
        IS_params<Distribution> params;

    public:
        using result_type = std::tuple<double, double, double>;

        // Paramètres du constructeur:
        // * `alpha`, `phi`, `gamma`, `d`, `g`: cf les paramètres de
        //   `src/detail/stochastic_gradient.hpp/approx_sequence::approx_sequence`
        // * `a`: constante dams le contrôle exponentiel de $x \longmapsto \phi^2(x)$
        // * `M`: nombre d'itérations pour cette phase, à connaître pour régler le seuil
        //   `alpha`
        IS_phase1_sequence(
            double alpha,
            double a,
            const Phi & phi,
            const Gamma & gamma,
            int M,
            Distribution & d,
            Generator & g
        ) :
            alpha { alpha }, a { a }, phi { phi }, gamma { gamma }, M { M }, d { d },
            g { g }, params { d }
        {
        }

        // Chaque appel à `next` renvoie la valeur suivante de la suite
        // $n \longmapsto (\xi_n, \theta_n, \mu_n)$, calculée avec un niveau
        // de confiance adaptatif.
        auto next() -> result_type {
            if (n == 0) {
                ++n;
                return std::make_tuple(xi, theta, mu);
            }
            
            // Niveau de confiance adaptatif
            double alpha_n;
            int treshold = M / 3;
            if (alpha > 0.5 && n <= treshold)
                alpha_n = 0.5;
            else if (alpha > 0.8 && n <= 2 * treshold)
                alpha_n = 0.8;
            else
                alpha_n = alpha;

            auto x = d(g);
            theta -= gamma(n) * L3(xi, theta, x, phi, params);
            mu -= gamma(n) * L4(xi, mu, x, a, phi, params);
            xi -= gamma(n) * H1(xi, x, alpha_n);
            ++n;
            return std::make_tuple(xi, theta, mu);
        }
};

// Fonction $L1$ de l'article, définie dans la section 2.1.
template<class InputType, class Phi, class Distribution>
auto L1(
    double xi,
    const InputType & theta,
    const InputType & x,
    double alpha,
    const Phi & phi,
    IS_params<Distribution> p
) -> double {
    auto factor = std::exp(-p.rho() * std::pow(std::abs(theta), p.b()));
    if (phi(x + theta) < xi)
        return factor;
    return factor * (1 - 1 / (1 - alpha) * p.incr(x, theta));
}

// Fonction $L2$ de l'article, définie dans la section 2.1.
template<class InputType, class Phi, class Distribution>
auto L2(
    double xi,
    double C,
    const InputType & mu,
    const InputType & x,
    double alpha,
    const Phi & phi,
    IS_params<Distribution> p
) -> double {
    auto result = C - xi;
    auto val = phi(x + mu);
    if (val < xi)
        return result;
    return result - 1 / (1 - alpha) * (val - xi) * p.incr(x, mu);
}

// Phase 2 de l'algorithme d'importance sampling: on part des valeurs de $\xi_\alpha^*$,
// $\theta^*$ et $\mu^*$ estimées dans la phase 1. À noter qu'on ne fera plus évoluer les
// estimations de $\theta^*$ et $\mu^*$, seulement celles de $\xi_\alpha^*$ et bien sûr
// $C_\alpha^*$.
template<class Phi, class Gamma, class Distribution, class Generator>
class IS_phase2_sequence {
    private:
        const Phi & phi;
        using input_type = decltype(phi(0));

        double alpha, xi, C = 0;
        input_type theta, mu;
        const Gamma & gamma;
        int n = 0;

        Distribution & d;
        Generator & g;
        IS_params<Distribution> params;

    public:
        using result_type = std::tuple<double, double>;

        // Paramètres du constructeur:
        // * `alpha`, `phi`, `gamma`, `d`, `g`: cf les paramètres de
        //   `src/detail/stochastic_gradient.hpp/approx_sequence::approx_sequence`
        // * `xi`, `theta`, `mu`: valeurs estimées dans la phase 1
        IS_phase2_sequence(
            double alpha,
            double xi,
            input_type theta,
            input_type mu,
            const Phi & phi,
            const Gamma & gamma,
            Distribution & d,
            Generator & g
        ) :
            alpha { alpha }, xi { xi }, theta { theta }, mu { mu }, phi { phi },
            gamma { gamma }, d { d }, g { g }, params { d }
        {
        }

        // Chaque appel à `next` renvoie la valeur suivante de la suite
        // $n \longmapsto (\xi_n, C_n)$.
        auto next() -> result_type {
            if (n == 0) {
                ++n;
                return std::make_tuple(xi, C);
            }

            auto x = d(g);
            C -= gamma(n) * L2(xi, C, mu, x, alpha, phi, params);
            xi -= gamma(n) * L1(xi, theta, x, alpha, phi, params);
            ++n;
            return std::make_tuple(xi, C);
        }
};

}

#endif
