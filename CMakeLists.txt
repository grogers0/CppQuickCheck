cmake_minimum_required(VERSION 2.6)
project(CppQuickCheck)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory(examples)

add_library(cppqc SHARED src/Arbitrary.cpp)

install(DIRECTORY "include/" DESTINATION "include"
    PATTERN ".*" EXCLUDE)
install(TARGETS cppqc DESTINATION "lib")
