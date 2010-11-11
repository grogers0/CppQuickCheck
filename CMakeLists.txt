cmake_minimum_required(VERSION 2.6)
project(CppQuickCheck)

add_executable(sampleOutput examples/src/sampleOutput.cpp)
add_executable(sampleShrinkOutput examples/src/sampleShrinkOutput.cpp)
add_executable(testReverse examples/src/TestReverse.cpp)

include_directories("${PROJECT_SOURCE_DIR}/include")

install(DIRECTORY "include/" DESTINATION "include"
    PATTERN ".*" EXCLUDE)
