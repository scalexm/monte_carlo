#ifndef DETAIL_ITERATE_HPP
#define DETAIL_ITERATE_HPP

namespace detail {

// Fonction utilitaire qui retourne le $n$-ième terme d'une suite donnée, $n$ étant ici
// décrit par le paramètre `iterations`.
template<class Sequence>
auto iterate(
    Sequence sequence,
    int iterations
) -> typename Sequence::result_type
{
    typename Sequence::result_type state;
    for (int n = 0; n < iterations; ++n)
        state = sequence.next();
    return state;
}

}

#endif
