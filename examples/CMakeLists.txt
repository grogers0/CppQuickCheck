add_executable(sampleOutput src/sampleOutput.cpp)
target_link_libraries(sampleOutput cppqc)

add_executable(sampleShrinkOutput src/sampleShrinkOutput.cpp)
target_link_libraries(sampleShrinkOutput cppqc)

add_executable(testReverse src/TestReverse.cpp)
target_link_libraries(testReverse cppqc)

add_executable(testReverseArray src/TestReverseArray.cpp)
target_link_libraries(testReverseArray cppqc)

add_executable(testSort src/TestSort.cpp)
target_link_libraries(testSort cppqc)

add_executable(exampleElementsGen src/exampleElementsGen.cpp)
target_link_libraries(exampleElementsGen cppqc)

# requires c++1y compile flag
#add_executable(testBoostTupleSupport src/BoostTupleSupport.cpp)
#target_link_libraries(testBoostTupleSupport cppqc)


