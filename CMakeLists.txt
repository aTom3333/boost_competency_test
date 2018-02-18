project(boost_test)
set(CMAKE_CXX_STANDARD 11)
add_executable(boost_test main.cpp Boost.SafeFloat/test-cpp11.cpp)