#ifndef DETAIL_AVERAGING_HPP
#define DETAIL_AVERAGING_HPP

#include <utility> // `std::move`

namespace detail {

template <class T, class U>                                                   
inline std::pair<T, U> operator +(const std::pair<T, U> & l, const std::pair<T, U> & r) {
    return std::make_pair(l.first + r.first, l.second + r.second);
}

template <class T, class U>                                                   
inline std::pair<T, U> operator -(const std::pair<T, U> & l, const std::pair<T, U> & r) {
    return std::make_pair(l.first - r.first, l.second - r.second);
}

template <class T, class U, class V>                                                   
inline std::pair<T, U> operator *(const V & r, const std::pair<T, U> & l) {
    return std::make_pair(r * l.first, r * l.second);
}

template <class T, class U, class V>                                                   
inline std::pair<T, U> operator *(const std::pair<T, U> & l, const V & r) {
    return r * l;
}

template <class T, class U, class V>                                                   
inline std::pair<T, U> operator /(const std::pair<T, U> & l, const V & r) {
    return std::make_pair(l.first / r, l.second / r);
}

// Moyennise une suite donnée.
template<class Sequence>
class averaging {
private:
    Sequence state;
    typename Sequence::result_type avg_state;
    int n = 0;

public:
    using result_type = typename Sequence::result_type;

    averaging(Sequence state) : state { std::move(state) }
    {
    }

    auto next() -> result_type {
        if (n == 0) {
            avg_state = state.next();
            ++n;
            return avg_state;
        }
        avg_state = avg_state - (avg_state - state.next()) / (n + 1);
        ++n;
        return avg_state;
    }
};

}

#endif
