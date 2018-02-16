//
// Created by thomas on 13/02/18.
//

#ifndef BOOST_TEST_SAFE_FLOAT_HPP
#define BOOST_TEST_SAFE_FLOAT_HPP

#include <exception>
/*
 * The trivial implementation
 * constexpr float operator""_sf(long double a)
 * {
 *     static_assert(is_pow_of_0_5(a), "...");
 *     return a;
 * }
 * doesn't work because the parameter isn't constant
 */

namespace literal
{
    namespace impl
    {
        using return_type = float;

        #define bad_hex_format_message "Not a valid hex floating point number literal"
        
        template<unsigned long long N>
        struct always_false { static constexpr const bool value = false; };
        
        constexpr bool isxdigit(char c)
        {
            return ((c >= '0' and c <= '9')
                 or (c >= 'a' and c <= 'f')
                 or (c >= 'A' and c <= 'F'));
        }
        
        constexpr int hexvalue(char xdigit)
        {
            return (xdigit >= '0' and xdigit <= '9') ? xdigit - '0' 
                 : (xdigit >= 'a' and xdigit <= 'f') ? xdigit - 'a' + 10
                 : (xdigit >= 'A' and xdigit <= 'F') ? xdigit - 'A' + 10
                 : 0;
        }

        template<char FIRST, char... REST>
        constexpr return_type parse()
        {
            return 0.0;
        }

        template<unsigned long long MANTISSA, int POW>
        constexpr return_type parse_hex_mantissa_post_period()
        {
            return MANTISSA;//bad_hex_format();
        }

        template<unsigned long long MANTISSA, int POW, char FIRST, char... REST>
        constexpr return_type parse_hex_mantissa_post_period()
        {
            return MANTISSA;
        }

        template<unsigned long long MANTISSA>
        constexpr return_type parse_hex_mantissa_pre_period()
        {
            //throw std::exception();
            static_assert(always_false<MANTISSA>::value, bad_hex_format_message);
            return return_type{};
        }

        template<unsigned long long MANTISSA, char FIRST, char... REST>
        constexpr return_type parse_hex_mantissa_pre_period()
        {
            static_assert((isxdigit(FIRST) or FIRST == '.' or FIRST == 'p' or FIRST == 'P'),
                          bad_hex_format_message);
        
                return (FIRST == 'p' or FIRST == 'P') ? MANTISSA
                     : isxdigit(FIRST) ? parse_hex_mantissa_pre_period<MANTISSA * 16 + hexvalue(FIRST), REST...>()
                     : parse_hex_mantissa_post_period<MANTISSA, 0, REST...>();
            
        }

        template<char FIRST, char SECOND, char... REST>
        constexpr return_type parse_hex_prefix()
        {
            static_assert(FIRST == '0' and (SECOND == 'x' or SECOND == 'X'),
                          bad_hex_format_message);
            return parse_hex_mantissa_pre_period<0, REST...>();
        }

        template<char... STR>
        constexpr return_type parse_hex()
        {
            return parse_hex_prefix<STR...>();
        }
    }
    
    template<char... STR>
    constexpr impl::return_type operator ""_sf()
    {
        //static_assert(a == 0.0, "Not a positive power of 0.5");
        return impl::parse_hex<STR...>();
    }
}

#undef bad_hex_format_message

#endif //BOOST_TEST_SAFE_FLOAT_HPP
