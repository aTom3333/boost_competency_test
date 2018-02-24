#ifndef COMPETENCY_TEST_CPP11_HPP
#define COMPETENCY_TEST_CPP11_HPP

#include <tuple>
#include <vector>


namespace test
{
    /*
     * Implementations of the exercises of the competency test for Boost.SafeFloat using c++11
     * https://github.com/boostorg/boost/wiki/Boost-Google-Summer-of-Code-2018
     * Almost indentical to the c++17 implementation but with some more helper functions that 
     * aren't in standard library and some hack to get the thing work without some c++17 facilities
     * Comments can be found in the c++17 implementation
     */
    
    
    namespace detail
    {
        template<std::size_t... I>
        struct index_sequence{};
        
        template<std::size_t N, std::size_t... I>
        struct make_index_sequence_helper
        {
            using type = typename make_index_sequence_helper<N-1, N, I...>::type;
        };
        
        template<std::size_t... I>
        struct make_index_sequence_helper<0, I...>
        {
            using type = index_sequence<0, I...>;
        };
        
        template<std::size_t N>
        using make_index_sequence = typename make_index_sequence_helper<N-1>::type;
        
        template<typename T>
        std::vector<T> make_vector(std::size_t N, T t)
        {
            return std::vector<T>(N, t);
        }

        template<typename Tuple, std::size_t... I>
        auto vectorize_impl(std::size_t N, Tuple&& t, index_sequence<I...>)
            ->decltype(std::make_tuple(make_vector(N, std::get<I>(std::forward<Tuple>(t)))...))
        {
            return std::make_tuple(make_vector(N, std::get<I>(std::forward<Tuple>(t)))...);
        }
    }

    template<typename Tuple>
    auto vectorize(std::size_t N, Tuple&& t)
        ->decltype(detail::vectorize_impl(N, std::forward<Tuple>(t),
                                          detail::make_index_sequence<std::tuple_size<typename std::remove_reference<Tuple>::type>::value>{}))
    {
        return detail::vectorize_impl(N, std::forward<Tuple>(t),
                                      detail::make_index_sequence<std::tuple_size<typename std::remove_reference<Tuple>::type>::value>{});
    }

    
    namespace detail
    {
        template<bool FOUND, std::size_t POS, typename T, std::size_t CUR_POS>
        constexpr std::size_t pos_by_type_impl()
        {
            static_assert(FOUND, "Type not in tuple");
            return POS;
        }
        
        template<bool FOUND, std::size_t POS, typename T, std::size_t CUR_POS, typename FIRST, typename... REST>
        constexpr std::size_t pos_by_type_impl()
        {
            static_assert(!(FOUND and std::is_same<T, FIRST>::value), "Duplicate type in tuple");
            return pos_by_type_impl<FOUND or std::is_same<T, FIRST>::value,
                                    std::is_same<T, FIRST>::value ? CUR_POS : POS,
                                    T,
                                    CUR_POS+1,
                                    REST...>();
        }
        
        template<typename T, typename... Args>
        struct pos_by_type
        {
            static constexpr std::size_t value = pos_by_type_impl<false, 0, T, 0, Args...>();
        };

        template<typename T, typename... Args>
        constexpr T& get(std::tuple<Args...>& t)
        {
            return std::get<pos_by_type<T, Args...>::value>(t);
        }

        template<typename T, typename... Args>
        constexpr const T& get(const std::tuple<Args...>& t)
        {
            return std::get<pos_by_type<T, Args...>::value>(t);
        }
    }

    template<typename T, typename Tuple>
    std::vector<T>& get_vector(Tuple& t)
    {
        return detail::get<std::vector<T>>(t);
    }

    template<typename T, typename Tuple>
    const std::vector<T>& get_vector(const Tuple& t)
    {
        return detail::get<std::vector<T>>(t);
    }


   
    namespace detail
    {
        #define DEC_PARSE_ERROR "Not a valid floating point number literal"
        #define VALUE_ERROR "Floating point number isn't a positive power of 0.5"

        template<typename T>
        struct always_false
        { static constexpr bool value = false; };

        constexpr bool isdigit(char c)
        {
            return (c >= '0' and c <= '9');
        }

        constexpr int decvalue(char digit)
        {
            return (digit >= '0' and digit <= '9') ? digit-'0' : 0;
        }

        template<typename T>
        constexpr T trivial_pow(T x, int e)
        {
            return e < 0 ? 1 / trivial_pow(x, -e)
                         : e == 0 ? 1
                                  : x * trivial_pow(x, e-1);
        }
        

        template<typename RT>
        constexpr bool is_pow_of_0_5_impl(RT value)
        {
            return value == 1 ? true
                  : value > 1 ? false
                  : value == 0 ? false
                  : is_pow_of_0_5_impl(2*value);
        }

        template<typename RT>
        constexpr bool is_pow_of_0_5(RT value)
        {
            return is_pow_of_0_5_impl(2*value);
        }

        template<bool NEED_TO_TEST, typename RT, unsigned long long MANTISSA, int POW, bool SIGN, int EXPONENT>
        constexpr RT validate_dec()
        {
            static_assert(!NEED_TO_TEST or is_pow_of_0_5((POW + (SIGN?EXPONENT:-EXPONENT) < 0 ? MANTISSA / trivial_pow(10.0l, -POW - (SIGN?EXPONENT:-EXPONENT)) : MANTISSA * trivial_pow(10.0l, POW + (SIGN?EXPONENT:-EXPONENT)))), VALUE_ERROR);
            return (POW + (SIGN?EXPONENT:-EXPONENT) < 0 ? MANTISSA / trivial_pow(10.0l, -POW - (SIGN?EXPONENT:-EXPONENT)) : MANTISSA * trivial_pow(10.0l, POW + (SIGN?EXPONENT:-EXPONENT)));
        }


        template<bool NEED_TO_TEST, typename RT, unsigned long long MANTISSA, int POW, bool SIGN, int EXPONENT>
        constexpr RT parse_dec_exponent()
        {
            return validate_dec<NEED_TO_TEST, RT, MANTISSA, POW, SIGN, EXPONENT>();
        }

        template<bool NEED_TO_TEST, typename RT, unsigned long long MANTISSA, int POW, bool SIGN,
            int EXPONENT, char FIRST, char... REST>
        constexpr RT parse_dec_exponent()
        {
            static_assert(!NEED_TO_TEST or isdigit(FIRST), DEC_PARSE_ERROR);
            return parse_dec_exponent<NEED_TO_TEST, RT, MANTISSA, POW, SIGN, NEED_TO_TEST?EXPONENT*10 + decvalue(FIRST):0, REST...>();
        }


        template<bool NEED_TO_TEST, typename RT, unsigned long long MANTISSA, int POW>
        constexpr RT parse_dec_exponent_sign()
        {
            static_assert(!NEED_TO_TEST, DEC_PARSE_ERROR);
            return RT{};
        }

        template<bool NEED_TO_TEST, typename RT, unsigned long long MANTISSA, int POW, char FIRST, char... REST>
        constexpr RT parse_dec_exponent_sign()
        {
            static_assert(!NEED_TO_TEST or (isdigit(FIRST) or ((FIRST == '+' or FIRST == '-') and sizeof...(REST) > 0)),
                          DEC_PARSE_ERROR);
            
            return isdigit(FIRST) ? parse_dec_exponent<NEED_TO_TEST and isdigit(FIRST), RT, MANTISSA, POW, true, decvalue(FIRST), REST...>()
                 : FIRST == '+' ? parse_dec_exponent<NEED_TO_TEST and FIRST == '+', RT, MANTISSA, POW, true, 0, REST...>()
                 : parse_dec_exponent<NEED_TO_TEST and !isdigit(FIRST) and FIRST != '+', RT, MANTISSA, POW, false, 0, REST...>();
        }


        template<bool NEED_TO_TEST, typename RT, unsigned long long MANTISSA, int POW>
        constexpr RT parse_dec_post_period()
        {
            return validate_dec<NEED_TO_TEST, RT, MANTISSA, POW, true, 0>();
        }

        template<bool NEED_TO_TEST, typename RT, unsigned long long MANTISSA, int POW, char FIRST, char... REST>
        constexpr RT parse_dec_post_period()
        {
            static_assert(!NEED_TO_TEST or (isdigit(FIRST) or FIRST == 'e' or FIRST == 'E'),
                          DEC_PARSE_ERROR);
            
            return isdigit(FIRST) ? parse_dec_post_period<NEED_TO_TEST and isdigit(FIRST), RT, MANTISSA*10 + decvalue(FIRST), POW-1, REST...>()
                 : parse_dec_exponent_sign<NEED_TO_TEST and !isdigit(FIRST), RT, MANTISSA, POW, REST...>();
        }


        template<bool NEED_TO_TEST, typename RT, unsigned long long MANTISSA>
        constexpr RT parse_dec_pre_period()
        {
            return validate_dec<NEED_TO_TEST, RT, MANTISSA, 0, true, 0>();
        }

        template<bool NEED_TO_TEST, typename RT, unsigned long long MANTISSA, char FIRST, char... REST>
        constexpr RT parse_dec_pre_period()
        {
            static_assert(!NEED_TO_TEST or 
                              (isdigit(FIRST) or FIRST == '.' or FIRST == 'e' or FIRST == 'E'),
                          DEC_PARSE_ERROR);
            
            return isdigit(FIRST) ? parse_dec_pre_period<NEED_TO_TEST and isdigit(FIRST), RT, MANTISSA*10 + decvalue(FIRST), REST...>()
                 : FIRST == '.' ? parse_dec_post_period<NEED_TO_TEST and FIRST == '.', RT, MANTISSA, 0, REST...>()
                 : parse_dec_exponent_sign<NEED_TO_TEST and !isdigit(FIRST) and FIRST != '.' , RT, MANTISSA, 0, REST...>();
        }


        template<typename RT, char... STR>
        constexpr RT test_dec()
        {
            return parse_dec_pre_period<true, RT, 0, STR...>();
        }

        #undef DEC_PARSE_ERROR
        #undef VALUE_ERROR
    }

    namespace literal
    {
        template<char... STR>
        constexpr float operator""_sf()
        {
            return detail::test_dec<float, STR...>();
        }

        template<char... STR>
        constexpr double operator""_sd()
        {
            return detail::test_dec<double, STR...>();
        }

        template<char... STR>
        constexpr long double operator""_sld()
        {
            return detail::test_dec<long double, STR...>();
        }
    }
}
#endif //COMPETENCY_TEST_CPP11_HPP
