#ifndef DETAIL_IMPORTANCE_SAMPLING_PARAMETERS_HPP
#define DETAIL_IMPORTANCE_SAMPLING_PARAMETERS_HPP

#include <random>
#include <cmath> // `std::exp`

namespace detail {

// Classe générique servant uniquement de squelette. On spécialise la classe
// pour chaque distribution voulue, en donnant les paramètres de cette distribution
// pour l'algorithme d'importance sampling, à savoir:
// * $b$ et $\rho$
// * $(x, \theta) \longmapsto \frac{p(x+\theta)}{p(x)}$, représentée ici par la méthode `incr`
// * $(x, \theta) \longmapsto \frac{p^2(x-\theta)}{p(x)p(x-2\theta)} \frac{\nabla p(x-2\theta)}{p(x-2\theta)}}$,
//   qui intervient dans le gradient des fonctions à optimiser $Q_1$ et $Q_2$, représentée ici
//   par la méthode `W`
template<class Distribution>
class IS_params {
    private:
        const Distribution & d;

        // Type inference hack: permet de récupérer le type de retour de la distribution
        // choisie sans avoir à rajouter un paramètre template supplémentaire. Pourrait
        // être utile notamment pour spécialiser avec des distributions à valeurs
        // multi-dimensionnelles.
        using input_type = decltype(d(std::mt19937 { std::random_device { }() }));

    public:
        IS_params(const Distribution & d) : d { d }
        {
            static_assert(d == d, "distribution not supported");
        }

        auto b() const -> double {
            static_assert(d == d, "distribution not supported");
        }

        auto rho() const -> double {
            static_assert(d == d, "distribution not supported");
        }

        auto incr(const input_type & x, const input_type & theta) const -> double {
            static_assert(d == d, "distribution not supported");
        }

        auto W(const input_type & x, const input_type & theta) const -> double {
            static_assert(d == d, "distribution not supported");
        }
};

// Paramètres décrits plus haut pour la loi normale (a priori pour une moyenne et un
// écart-type quelconque).
template<>
class IS_params<std::normal_distribution<>> {
    private:
        const std::normal_distribution<> & d;

    public:
        IS_params(const std::normal_distribution<> & d) : d { d }
        {
        }

        auto b() const -> double {
            return 2;
        }

        auto rho() const -> double {
            auto stddev = d.stddev();
            return 0.5 / stddev / stddev;
        }

        auto incr(const double & x, const double & theta) const -> double {
            auto mu = d.mean();
            auto stddev = d.stddev();
            auto y = x - mu;
            auto z = y + theta;
            return std::exp(0.5 / stddev / stddev * (y * y - z * z));
        }

        auto W(const double & x, const double & theta) const -> double {
            auto mu = d.mean();
            auto stddev = d.stddev();
            auto q = theta / stddev;
            return std::exp(q * q) * (2 * theta - x + mu);
        }
};

template<>
class IS_params<std::exponential_distribution<>> {
    private:
        const std::exponential_distribution<> & d;
    
    public:
        IS_params(const std::exponential_distribution<> & d) : d { d }
        {
        }

        auto b() const -> double {
            return 1.0;
        }

        auto rho() const -> double {
            return d.lambda();
        }

        auto incr(const double & x, const double & theta) const -> double {
            if (x + theta < 0)
                return 0;
            return std::exp(-d.lambda() * theta);
        }

        auto W(const double & x, const double & theta) const -> double {
            if (x - theta < 0)
                return 0;
            if (x - 2 * theta < 0)
                return 2 * d.lambda() * std::exp(-2 * d.lambda() * (x - 2 * theta));
            return -2 * d.lambda();
        }
};

}

#endif
