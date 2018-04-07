#ifndef STEPS_HPP
#define STEPS_HPP

#include <cmath> // `std::pow`
#include <functional> // `std::function`

namespace steps {

// $n \longmapsto \frac{1}{n}$
template<class RealType>
auto inverse(int n) -> RealType {
    return 1 / static_cast<RealType>(n);
}

// $n \longmapsto \frac{1}{n^a}$
// FIXME: retourne un objet `std::function<...>`, ce qui peut donner lieu à des allocations.
// Le standard C++11 préconise d'éviter de telles allocations, mais ne l'impose pas.
// En C++14, il suffirait de laisser le compilateur inférer un type de retour pour régler ça.
template<class RealType = double, class Exp>
auto inverse_pow(Exp a) -> std::function<RealType(int)> {
    return [a](int n) { return std::pow(inverse<RealType>(n), a); };
}

}

#endif
