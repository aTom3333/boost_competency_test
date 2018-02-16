project(boost_test)
set(CMAKE_CXX_STANDARD 17)
add_executable(boost_test main.cpp Boost.SafeFloat/competency-test-cpp17.hpp)