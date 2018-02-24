#ifndef COMPETENCY_TEST_CPP17_HPP
#define COMPETENCY_TEST_CPP17_HPP

#include <tuple>
#include <vector>


namespace test
{
    /*
     * Implementations of the exercises of the competency test for Boost.SafeFloat using c++17
     * https://github.com/boostorg/boost/wiki/Boost-Google-Summer-of-Code-2018
     */
    
    /*
     * Provide a function receiving an integer (X) and a tuple (std::tuple<type1, type2, type3…>) 
     * into a tuple of vectors (std::tuple<std::vector,std::vector, std::vector, …>)
     * where each vector has X elements of each originally received in each tuple_element. 
     * E.g. for X=2 and the tuple {1, 1.0, ‘a’} , the result type is 
     * std::tuple<std::vector, std::vector, std::vector> and the values are: 
     * {{1, 1},{1.0, 1.0},{‘a’, ‘a’}}
     */
    // The implementation is inpired by the proposed implementation of std::apply : 
    // http://en.cppreference.com/w/cpp/utility/apply
    namespace detail
    {
        // Utility function that return a vector of T with N t, just call the constructor
        // Useful as the type is automatically deduced
        template<typename T>
        auto make_vector(std::size_t N, T t)
        {
            return std::vector<T>(N, t);
        }

        // Call make_vector for the Ith element for every I and pack the vectors in a tuple
        template<typename Tuple, std::size_t... I>
        auto vectorize_impl(std::size_t N, Tuple&& t, std::index_sequence<I...>)
        {
            return std::make_tuple(make_vector(N, std::get<I>(std::forward<Tuple>(t)))...);
        }
    }
    
    // Feed the vectorize_impl function with the number, the tuple 
    // and an index sequence of the size of the tuple
    template<typename Tuple>
    auto vectorize(std::size_t N, Tuple&& t)
    {
        return detail::vectorize_impl(N, std::forward<Tuple>(t), 
                                      std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
    }
    
    
    /*
     * Provide a template function “get_vector”, similar to std::get, 
     * that given the type X as parameter, returns editable access to 
     * the vector of type X in a tuple of vectors.
     */
    // Using std::get, the implementation is trivial
    template<typename T, typename Tuple>
    auto& get_vector(Tuple& t)
    {
        return std::get<std::vector<T>>(t);
    }
    
    template<typename T, typename Tuple>
    const auto& get_vector(const Tuple& t)
    {
        return std::get<std::vector<T>>(t);
    }
    
    
    /*
     * Define a user defined literal assignable to float that fails compilation
     * when the value provided cannot be expressed as an positive integer power
     * of 0.5 (e.g. 0.5, 0.25, 0.125).
     */
    /* Hardest but most interesting exercise in my eyes
     * I started by thinking it will be easy, just using an user-defined literal taking
     * a long double as parameter but the need to raise a compilation error
     * obliged me to use a templated UDL
     */
    namespace detail
    {
        #define HEX_PARSE_ERROR "Not a valid hex floating point number literal"
        #define DEC_PARSE_ERROR "Not a valid dec floating point number literal"
        #define VALUE_ERROR "Floating point number isn't a positive power of 0.5" 
        
        // A struct to be used in static_assert
        template<typename T>
        struct always_false
        { static constexpr bool value = false; };

        // A constexpr version of isxdigit working on ASCII
        constexpr bool isxdigit(char c)
        {
            return ((c >= '0' and c <= '9')
                    or (c >= 'a' and c <= 'f')
                    or (c >= 'A' and c <= 'F'));
        }

        // A constexpr version of isdigit working on ASCII
        constexpr bool isdigit(char c)
        {
            return (c >= '0' and c <= '9');
        }

        // constexpr function to get the represented value of a hex digit
        constexpr int hexvalue(char xdigit)
        {
            return (xdigit >= '0' and xdigit <= '9') ? xdigit - '0'
                                                     : (xdigit >= 'a' and xdigit <= 'f') ? xdigit - 'a' + 10
                                                                                         : (xdigit >= 'A' and xdigit <= 'F') ? xdigit - 'A' + 10
                                                                                                                             : 0;
        }

        // constexpr function to get the represented value of a digit
        constexpr int decvalue(char digit)
        {
            return (digit >= '0' and digit <= '9') ? digit-'0' : 0;
        }
        
        // constexpr function that count bit equal to 1
        constexpr int count_bit(unsigned long long a)
        {
            int count = 0;
            while(a != 0)
            {
                if(a & 0x1)
                    count++;
                a >>= 1;
            }
            
            return count;
        }
        
        // A constexpr function that count the number of bit with value 0 before a bit with 1
        constexpr int count_shift(unsigned long long a)
        {
            int count = 0;
            while(a != 0)
            {
                if(a & 0x1)
                    return count;
                count++;
                a >>= 1;
            }
            
            return count;
        }
        
        template<typename T>
        constexpr T trivial_pow(T x, int e)
        {
            return e < 0 ? 1 / trivial_pow(x, -e)
                : e == 0 ? 1
                : x * trivial_pow(x, e-1);
        }
        
        // A function to know is a literal begins with an hex prefix
        template<char FIRST>
        constexpr bool has_hex_prefix()
        {
            return false;
        }
        
        template<char FIRST, char SECOND, char... REST>
        constexpr bool has_hex_prefix()
        {
            return FIRST == '0' and (SECOND == 'x' or SECOND == 'X');
        }
        
        /*
         * Hex parsing related functions (better understanding by reading from bottom to top)
         */
        
        // Function used to validate the number and returning its value
        template<typename RT, unsigned long long MANTISSA, int POW, bool SIGN, int EXPONENT>
        constexpr RT validate_hex()
        {
            // Is it a power of two ?
            static_assert(count_bit(MANTISSA) == 1, VALUE_ERROR);
            // Is the exponent negative (positive power of 0.5 means negative power of 2)
            static_assert(count_shift(MANTISSA) + POW + (SIGN? EXPONENT : -EXPONENT) < 0,
                          VALUE_ERROR);
            return static_cast<RT>(MANTISSA) * trivial_pow(2.0l, POW + (SIGN? EXPONENT : -EXPONENT));
        }
        
        
        // Function used to parse the digit of the exponent
        template<typename RT, unsigned long long MANTISSA, int POW, bool SIGN, int EXPONENT>
        constexpr RT parse_hex_exponent()
        {
            return validate_hex<RT, MANTISSA, POW, SIGN, EXPONENT>();
        }
        
        template<typename RT, unsigned long long MANTISSA, int POW, bool SIGN, 
                 int EXPONENT, char FIRST, char... REST>
        constexpr RT parse_hex_exponent()
        {
            static_assert(isdigit(FIRST), HEX_PARSE_ERROR);
            return parse_hex_exponent<RT, MANTISSA, POW, SIGN, EXPONENT*10 + decvalue(FIRST), REST...>();
        }
        
        
        // This function parses the first character after the 'p'
        template<typename RT, unsigned long long MANTISSA, int POW>
        constexpr RT parse_hex_exponent_sign()
        {
            static_assert(always_false<RT>::value, HEX_PARSE_ERROR);
            return RT{};
        }
        
        template<typename RT, unsigned long long MANTISSA, int POW, char FIRST, char... REST>
        constexpr RT parse_hex_exponent_sign()
        {
            static_assert(isxdigit(FIRST) or FIRST == '+' or FIRST == '-',
                          HEX_PARSE_ERROR);
            if constexpr(isxdigit(FIRST))
                return parse_hex_exponent<RT, MANTISSA, POW, true, hexvalue(FIRST), REST...>();
            else
            {
                // If we have a sign, we need to make there is at least one other character after
                static_assert(sizeof...(REST) > 0, HEX_PARSE_ERROR);
                if constexpr(FIRST == '+')
                    return parse_hex_exponent<RT, MANTISSA, POW, true, 0, REST...>();
                else
                    return parse_hex_exponent<RT, MANTISSA, POW, false, 0, REST...>();
            }
        }

        
        // Function used to parse the characters of a hex float literal that appear after the period
        template<typename RT, unsigned long long MANTISSA, int POW>
        constexpr RT parse_hex_post_period()
        {
            static_assert(always_false<RT>::value, HEX_PARSE_ERROR);
            return RT{};
        }
        
        template<typename RT, unsigned long long MANTISSA, int POW, char FIRST, char... REST>
        constexpr RT parse_hex_post_period()
        {
            static_assert(isxdigit(FIRST) or FIRST == 'p' or FIRST == 'P',
                          HEX_PARSE_ERROR);
            if constexpr(isxdigit(FIRST))
                return parse_hex_post_period<RT, MANTISSA*16 + hexvalue(FIRST), POW-4, REST...>();
            else
                return parse_hex_exponent_sign<RT, MANTISSA, POW, REST...>();
        }

        
        // Function used to parse the characters of a hex float literal that appear before the period
        template<typename RT, unsigned long long MANTISSA>
        constexpr RT parse_hex_pre_period()
        {
            // If they are no more characters and we're here it means that is not a good hex float literal
            static_assert(always_false<RT>::value, HEX_PARSE_ERROR);
            return RT{};
        }
        
        template<typename RT, unsigned long long MANTISSA, char FIRST, char... REST>
        constexpr RT parse_hex_pre_period()
        {
            static_assert(isxdigit(FIRST) or FIRST == '.' or FIRST == 'p' or FIRST == 'P',
                          HEX_PARSE_ERROR);
            if constexpr(isxdigit(FIRST))
                return parse_hex_pre_period<RT, MANTISSA*16 + hexvalue(FIRST), REST...>();
            else if constexpr(FIRST == '.')
                return parse_hex_post_period<RT, MANTISSA, 0, REST...>();
            else
                return parse_hex_exponent_sign<RT, MANTISSA, 0, REST...>();
        }
        
        
        template<typename RT, char FIRST, char SECOND, char... REST>
        constexpr RT test_hex()
        {
            // Discard 2 first char as they are the hex marker
            return parse_hex_pre_period<RT, 0, REST...>();
        }



        /*
         * Dec parsing related functions (better understanding by reading from bottom to top)
         */
        
        // Test if value is power of 0.5 by recursively multiplying it by 2 and compare with 1
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
        
        // Validate value and returns it
        template<typename RT, unsigned long long MANTISSA, int POW, bool SIGN, int EXPONENT>
        constexpr RT validate_dec()
        {
            constexpr RT value = (POW + (SIGN?EXPONENT:-EXPONENT) < 0 ? MANTISSA / trivial_pow(10.0l, -POW - (SIGN?EXPONENT:-EXPONENT)) : MANTISSA * trivial_pow(10.0l, POW + (SIGN?EXPONENT:-EXPONENT)));
            static_assert(is_pow_of_0_5(value), VALUE_ERROR);
            return value;
        }
        
        
        // This function parses the exponent past the first character
        template<typename RT, unsigned long long MANTISSA, int POW, bool SIGN, int EXPONENT>
        constexpr RT parse_dec_exponent()
        {
            return validate_dec<RT, MANTISSA, POW, SIGN, EXPONENT>();
        }
        
        template<typename RT, unsigned long long MANTISSA, int POW, bool SIGN,
                 int EXPONENT, char FIRST, char... REST>
        constexpr RT parse_dec_exponent()
        {
            static_assert(isdigit(FIRST), DEC_PARSE_ERROR);
            return parse_dec_exponent<RT, MANTISSA, POW, SIGN, EXPONENT*10 + decvalue(FIRST), REST...>();
        }
        
        
        // This function parses the first character of the exponent
        template<typename RT, unsigned long long MANTISSA, int POW>
        constexpr RT parse_dec_exponent_sign()
        {
            static_assert(always_false<RT>::value, DEC_PARSE_ERROR);
            return RT{};
        }
        
        template<typename RT, unsigned long long MANTISSA, int POW, char FIRST, char... REST>
        constexpr RT parse_dec_exponent_sign()
        {
            static_assert(isdigit(FIRST) or ((FIRST == '+' or FIRST == '-') and sizeof...(REST) > 0),
                          DEC_PARSE_ERROR);
            if constexpr(isdigit(FIRST))
                return parse_dec_exponent<RT, MANTISSA, POW, true, decvalue(FIRST), REST...>();
            else if constexpr(FIRST == '+')
                return parse_dec_exponent<RT, MANTISSA, POW, true, 0, REST...>();
            else
                return parse_dec_exponent<RT, MANTISSA, POW, false, 0, REST...>();
        }
        
        
        // This function parses the characters that are after the period
        template<typename RT, unsigned long long MANTISSA, int POW>
        constexpr RT parse_dec_post_period()
        {
            return validate_dec<RT, MANTISSA, POW, true, 0>();
        }
        
        template<typename RT, unsigned long long MANTISSA, int POW, char FIRST, char... REST>
        constexpr RT parse_dec_post_period()
        {
            static_assert(isdigit(FIRST) or FIRST == 'e' or FIRST == 'E',
                          DEC_PARSE_ERROR);
            if constexpr(isdigit(FIRST))
                return parse_dec_post_period<RT, MANTISSA*10 + decvalue(FIRST), POW-1, REST...>();
            else
                return parse_dec_exponent_sign<RT, MANTISSA, POW, REST...>();
        }
        
        
        // This character parses the first characters before the period
        template<typename RT, unsigned long long MANTISSA>
        constexpr RT parse_dec_pre_period()
        {
            return validate_dec<RT, MANTISSA, 0, true, 0>();
        }
        
        template<typename RT, unsigned long long MANTISSA, char FIRST, char... REST>
        constexpr RT parse_dec_pre_period()
        {
            static_assert(isdigit(FIRST) or FIRST == '.' or FIRST == 'e' or FIRST == 'E',
                          DEC_PARSE_ERROR);
            if constexpr(isdigit(FIRST))
                return parse_dec_pre_period<RT, MANTISSA*10 + decvalue(FIRST), REST...>();
            else if constexpr(FIRST == '.')
                return parse_dec_post_period<RT, MANTISSA, 0, REST...>();
            else
                return parse_dec_exponent_sign<RT, MANTISSA, 0, REST...>();
        }
        
        
        template<typename RT, char... STR>
        constexpr RT test_dec()
        {
            return parse_dec_pre_period<RT, 0, STR...>();
        }

        // Don't pollute global namespace
        #undef HEX_PARSE_ERROR
        #undef DEC_PARSE_ERROR
        #undef VALUE_ERROR
    }
    
    namespace literal
    {
        template<char... STR>
        constexpr auto operator""_sf()
        {
            if constexpr(detail::has_hex_prefix<STR...>())
                return detail::test_hex<float, STR...>();
            else
                return detail::test_dec<float, STR...>();
        }

        template<char... STR>
        constexpr auto operator""_sd()
        {
            if constexpr(detail::has_hex_prefix<STR...>())
                return detail::test_hex<double, STR...>();
            else
                return detail::test_dec<double, STR...>();
        }

        template<char... STR>
        constexpr auto operator""_sld()
        {
            if constexpr(detail::has_hex_prefix<STR...>())
                return detail::test_hex<long double, STR...>();
            else
                return detail::test_dec<long double, STR...>();
        }
    }
}   
#endif //COMPETENCY_TEST_CPP17_HPP
