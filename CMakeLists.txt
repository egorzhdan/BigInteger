cmake_minimum_required(VERSION 3.9)

project(BigInteger)

include_directories(${BIGINT_SOURCE_DIR})

add_executable(
        big_integer_testing
        gtest/gtest-all.cc
        gtest/gtest.h
        gtest/gtest_main.cc
        big_integer_testing.cpp
        big_integer.h
        big_integer.cpp
        digit_vector.cpp digit_vector.h)

#if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
#set(CMAKE_CXX_COMPILER "g++-6")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
#endif()

target_link_libraries(big_integer_testing -lpthread)
