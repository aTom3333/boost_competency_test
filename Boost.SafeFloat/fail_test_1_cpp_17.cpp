#include "competency-test-cpp17.hpp"
int main(){
    using namespace test::literal;
    0.3_sf; //expected to fail compilation
}
