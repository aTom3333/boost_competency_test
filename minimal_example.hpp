struct ratio {
    unsigned long long num, den;
};

template<bool NEED_TO_TEST, unsigned long long NUM, unsigned long long DEN>
constexpr ratio parse_post_div()
{
    // Arbitrary condition I want to be verified at compile-time
    static_assert(!NEED_TO_TEST || NUM < DEN, "ratio is greater than 1");
    return ratio{NUM, DEN};
}

template<bool NEED_TO_TEST, unsigned long long NUM, unsigned long long DEN, char FIRST, char... REST>
constexpr ratio parse_post_div()
{
    static_assert(!NEED_TO_TEST || (FIRST >= '0' && FIRST <= '9'),
                  "parse error");
    return parse_post_div<NEED_TO_TEST, NUM, DEN*10 + FIRST-'0', REST...>();
}

template<bool NEED_TO_TEST, unsigned long long NUM>
constexpr ratio parse_pre_div()
{
    return ratio{NUM, 1};
}

template<bool NEED_TO_TEST, unsigned long long NUM, char FIRST, char... REST>
constexpr ratio parse_pre_div()
{
    static_assert(!NEED_TO_TEST || ((FIRST >= '0' && FIRST <= '9') || FIRST == 'e'),
                  "parse error");
    return FIRST == 'e' ? parse_post_div<NEED_TO_TEST&&FIRST=='e', NUM, 0, REST...>() 
                        : parse_pre_div<NEED_TO_TEST&&FIRST!='e', NUM*10 + FIRST-'0', REST...>();
}

template<char... STR>
constexpr ratio operator""_rat()
{
    return parse_pre_div<true, 0, STR...>();
}