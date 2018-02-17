//
// Created by thomas on 13/02/18.
//

#include <iostream>
#include <tuple>
#include <type_traits>
#include <iomanip>
//#include "safe_float.hpp"
#include "Boost.SafeFloat/competency-test-cpp17.hpp"


//using namespace literal;

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

using namespace test::literal;

int main()
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
    
    //auto c = 0x1ap1_sf;
    /*
    auto c = 15e87_rat;
    
    float b = 0x9.3p1;
    std::cout << c.num << '/' << c.den << std::endl;
    */
    static_assert(test::detail::is_pow_of_0_5(.25), "nul");
    
    auto f = .001953125_sf;
    std::cout << std::setprecision(40) << f/2 << std::endl << .4589e+3f << std::endl;

    return 0;
}