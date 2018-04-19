#ifndef STEPS_HPP
#define STEPS_HPP

#include <cmath> // `std::pow`
#include <functional> // `std::function`

// Quelques exemples de suites $n \longmapsto \gamma_n$ respectant les hypothèses de l'article.
namespace steps {

// $n \longmapsto \frac{1}{n}$
inline auto inverse(int n) -> double {
    return 1 / static_cast<double>(n);
}

// $n \longmapsto \frac{1}{n^a + offset}$
// FIXME: retourne un objet `std::function<...>`, ce qui peut donner lieu à des allocations.
// Le standard C++11 préconise d'éviter de telles allocations, mais ne l'impose pas.
// En C++14, il suffirait de laisser le compilateur inférer un type à la fonction anonyme
// retournée pour régler ça.
inline auto inverse_pow(double a = 0.75, double offset = 100.0) -> std::function<double(int)> {
    return [a, offset](int n) { return 1 / (std::pow(n, a) + offset); };
}

}

#endif
