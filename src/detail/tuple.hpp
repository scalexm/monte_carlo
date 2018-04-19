#ifndef DETAIL_TUPLE_HPP
#define DETAIL_TUPLE_HPP

#include <tuple>

// Opérations génériques sur les tuple (absentes de la bibliothèque standard): addition
// et soustraction terme à terme, multiplication et division par un scalaire. Beaucoup de
// template meta-programming laborieux, pas forcément très intéressant à regarder.
//
// On s'en sert pour `src/detail/averaging.hpp/averaging::next` (en effet c'est beaucoup
// pour pas grand chose, mais à peu près indispensable si on veut garder la généricité de
// cette méthode).
namespace detail {

template<class Tuple, size_t N>
struct TupleAdd {
    static void add(Tuple & l, const Tuple & r) {
        std::get<N - 1>(l) += std::get<N - 1>(r);
        TupleAdd<Tuple, N - 1>::add(l, r);
    }
};

template<class Tuple>
struct TupleAdd<Tuple, 1> {
    static void add(Tuple & l, const Tuple & r) {
        std::get<0>(l) += std::get<0>(r);
    }
};

// Addition terme à terme.
template<class... T>
auto operator +(
    const std::tuple<T...> & l,
    const std::tuple<T...> & r
) -> std::tuple<T...>
{
    auto result = l;
    TupleAdd<std::tuple<T...>, sizeof...(T)>::add(result, r);
    return result;
}

template<class Tuple, size_t N>
struct TupleSub {
    static void sub(Tuple & l, const Tuple & r) {
        std::get<N - 1>(l) -= std::get<N - 1>(r);
        TupleSub<Tuple, N - 1>::sub(l, r);
    }
};

template<class Tuple>
struct TupleSub<Tuple, 1> {
    static void sub(Tuple & l, const Tuple & r) {
        std::get<0>(l) -= std::get<0>(r);
    }
};

// Soustraction terme à terme.
template<class... T>
auto operator -(
    const std::tuple<T...> & l,
    const std::tuple<T...> & r
) -> std::tuple<T...>
{
    auto result = l;
    TupleSub<std::tuple<T...>, sizeof...(T)>::sub(result, r);
    return result;
}

template<class Tuple, class Arg, size_t N>
struct TupleMul {
    static void mul(Tuple & l, const Arg & r) {
        std::get<N - 1>(l) *= r;
        TupleMul<Tuple, Arg, N - 1>::mul(l, r);
    }
};

template<class Tuple, class Arg>
struct TupleMul<Tuple, Arg, 1> {
    static void mul(Tuple & l, const Arg & r) {
        std::get<0>(l) *= r;
    }
};

// Multiplication à droite par un scalaire.
template<class Arg, class... T>
auto operator *(
    const std::tuple<T...> & l,
    const Arg & r
) -> std::tuple<T...>
{
    auto result = l;
    TupleMul<std::tuple<T...>, Arg, sizeof...(T)>::mul(result, r);
    return result;
}

// Mutiplication à gauche par un scalaire.
template<class Arg, class... T>
auto operator *(
    const Arg & l,
    const std::tuple<T...> & r
) -> std::tuple<T...>
{
    return r * l;
}

template<class Tuple, class Arg, size_t N>
struct TupleDiv {
    static void div(Tuple & l, const Arg & r) {
        std::get<N - 1>(l) /= r;
        TupleDiv<Tuple, Arg, N - 1>::div(l, r);
    }
};

template<class Tuple, class Arg>
struct TupleDiv<Tuple, Arg, 1> {
    static void div(Tuple & l, const Arg & r) {
        std::get<0>(l) /= r;
    }
};

// Division (uniquement à droite) par un scalaire.
template<class Arg, class... T>
auto operator /(
    const std::tuple<T...> & l,
    const Arg & r
) -> std::tuple<T...>
{
    auto result = l;
    TupleDiv<std::tuple<T...>, Arg, sizeof...(T)>::div(result, r);
    return result;
}

}

#endif
