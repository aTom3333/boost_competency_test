#include "test-cpp17.hpp"
#include "competency-test-cpp17.hpp"
#include <iostream>

// Print vector
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
    os << '{';
    for(auto& e : v)
        os << e << ", ";
    if(!v.empty())
        os << "\b\b";
    return os << '}';
}

// Print tuple
template<class Tuple, std::size_t N>
struct TuplePrinter {
    static void print(std::ostream& os, const Tuple& t)
    {
        TuplePrinter<Tuple, N-1>::print(os, t);
        os << ", " << std::get<N-1>(t);
    }
};

template<class Tuple>
struct TuplePrinter<Tuple, 1> {
    static void print(std::ostream& os, const Tuple& t)
    {
        os << std::get<0>(t);
    }
};

template<class... Args>
std::ostream& operator<<(std::ostream& os, const std::tuple<Args...>& t)
{
    os << '[';
    TuplePrinter<decltype(t), sizeof...(Args)>::print(os, t);
    return os << ']';
}

// Basic struct for test
struct Point
{
    double x, y;
    friend std::ostream& operator<<(std::ostream& os, Point p)
    { return os << '(' << p.x << ',' << p.y << ')'; }
};


void test_floating_literal()
{
    using namespace test::literal;
    
    // Should compile
    static_assert(0.5_sf == 0.5f, "Problem");
    static_assert(0.25_sf == 0.25f, "Problem");
    static_assert(.03125_sd == .03125, "Problem");
    static_assert(62.5e-3_sld == 62.5e-3l, "Problem");
    static_assert(9.5367431640625e-07_sd == 9.5367431640625e-07, "Problem");
    static_assert(125.e-3_sf == 125.e-3f, "Problem");
    static_assert(0.005e2_sld == 0.005e2l, "Problem");
    static_assert(0x1p-1_sf == 0x1p-1f, "Problem");
    static_assert(0x8.0p-98_sd == 0x8.0p-98, "Problem");
    static_assert(0x.00004p+3_sld == 0x.00004p+3l, "Problem");
    
    // Shouldn't compile
    //3_sf;
    //0.26_sd;
    //9.5367431640626e-07_sd;
    //1.0_sld;
    //0x3.0p-7_sf;
    //0_sf;
    //0x0p0_sd;
    
    // This one should be a parse error
    //0x4_sf;
}


void test_vectorize()
{
    // Manual declaration of type to know what it is made of
    std::tuple<int, const char*, double, Point> t(48, "foo", 3.14, {15.2, 48.6});

    auto tv = test::vectorize(4, t);
    static_assert(std::is_same_v<decltype(tv),
                      std::tuple<
                          std::vector<int>,
                          std::vector<const char*>,
                          std::vector<double>,
                          std::vector<Point>
                      >>,
                  "There is a problem");
    
    std::cout << tv << std::endl;
}


void test_get_vector()
{
    std::tuple<int, const char*, double, Point> t(48, "foo", 3.14, {15.2, 48.6});
    auto tv = test::vectorize(2, t);

    std::cout << "Before : " << tv << std::endl;
    test::get_vector<int>(tv).push_back(87);
    test::get_vector<const char*>(tv).clear();
    test::get_vector<Point>(tv).begin()->y = -0.47;
    std::cout << "After : " << tv << std::endl;
}