#ifndef STEPS_HPP
#define STEPS_HPP

#include <cmath>
#include <functional>

namespace steps {

// $n \longmapsto \frac{1}{n}$
template<class RealType>
auto inverse(int n) -> RealType {
    return 1 / static_cast<RealType>(n);
}

template<class RealType = double, class Exp>
auto inverse_pow(Exp a) -> std::function<RealType(int)> {
    return [a](int n) { return std::pow(inverse<RealType>(n), a); };
}

}

#endif
