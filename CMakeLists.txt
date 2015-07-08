cmake_minimum_required(VERSION 2.6)
project(CppQuickCheck)
set(CMAKE_CXX_FLAGS "-O3 -g -Wall -std=c++11")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory(examples)

add_library(cppqc SHARED src/Arbitrary.cpp)

install(DIRECTORY "include/" DESTINATION "include"
    PATTERN ".*" EXCLUDE)
install(TARGETS cppqc DESTINATION "lib")
