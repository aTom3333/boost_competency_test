//
// Created by thomas on 13/02/18.
//

#include <iostream>
//#include "safe_float.hpp"
#include "minimal_example.hpp"


//using namespace literal;

int main()
{
    //auto c = 0x1ap1_sf;
    
    auto c = 15e87_rat;
    
    float b = 0x9.3p1;
    std::cout << c.num << '/' << c.den << std::endl;
    

    return 0;
}