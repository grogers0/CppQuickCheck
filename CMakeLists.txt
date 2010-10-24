cmake_minimum_required(VERSION 2.6)
project(CppQuickCheck)

add_executable(sampleOutput examples/sampleOutput.cpp)
add_executable(sampleShrinkOutput examples/sampleShrinkOutput.cpp)
add_executable(testReverse examples/TestReverse.cpp)

include_directories("${PROJECT_BINARY_DIR}")

install(DIRECTORY cppqc DESTINATION "include"
    PATTERN ".*" EXCLUDE)
