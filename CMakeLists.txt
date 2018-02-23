project(boost_test)
cmake_minimum_required(VERSION 3.10)
include(CheckCXXCompilerFlag)

#Headers in project file
add_executable(test_headers Boost.SafeFloat)
set_target_properties(test_headers PROPERTIES
EXCLUDE_FROM_ALL TRUE
EXCLUDE_FROM_DEFAULT_BUILD TRUE
LINKER_LANGUAGE CXX)

#Executable for C++17/1z
add_executable(boost_test_17 main.cpp Boost.SafeFloat/test-cpp17.cpp)
check_cxx_compiler_flag(-std=c++17 HAVE_FLAG_STD_CXX17)
if(HAVE_FLAG_STD_CXX17)
set_target_properties(boost_test_17 PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -pedantic --std=c++17")
else()
set_target_properties(boost_test_17 PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -pedantic --std=c++1z")
endif()

#Executable for C++11
add_executable(boost_test_11 main.cpp Boost.SafeFloat/test-cpp11.cpp)
set_target_properties(boost_test_11 PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -pedantic --std=c++11")
