#ifndef STEPS_HPP
#define STEPS_HPP

#include <cmath>
#include <functional>

namespace steps {

// $n \longmapsto \frac{1}{n}$
template<class Float>
auto inverse(int n) -> Float {
    return 1 / static_cast<Float>(n);
}

template<class Float, class Exp>
auto inverse_pow(Exp a) -> std::function<Float(int)> {
    return [a](int n) { return std::pow(inverse<Float>(n), a); };
}

}

#endif
