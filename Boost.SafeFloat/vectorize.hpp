//
// Created by thomas on 16/02/18.
//

#ifndef BOOST_TEST_VECTORIZE_HPP
#define BOOST_TEST_VECTORIZE_HPP

#include <tuple>
#include <vector>
#include <functional>
#include <type_traits>


template<typename T>
auto make_vector(unsigned int N, T t)
{
    return std::vector<T>(N, t);
}


template<typename Tuple, std::size_t... I>
auto vectorize_impl(unsigned int N, Tuple&& t, std::index_sequence<I...>)
{
    return std::make_tuple(make_vector(N, std::get<I>(std::forward<Tuple>(t)))...);
}


template<typename Tuple>
auto vectorize(unsigned int N, Tuple&& t)
{
    return vectorize_impl(N, std::forward<Tuple>(t), std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}


template<typename T, typename Tuple>
auto& get_vector(Tuple&& t)
{
    return std::get<std::vector<T>>(std::forward<Tuple>(t));
}

#endif //BOOST_TEST_VECTORIZE_HPP
