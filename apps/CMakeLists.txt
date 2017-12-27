cmake_minimum_required (VERSION 2.8)
project (fluidanimate)

add_subdirectory(fanimate)
add_subdirectory(animate)
add_subdirectory(animate_tbb)
add_subdirectory(fanimate_tbb)

add_test(fanimate_5K
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fanimate"
  1 100
  "${CMAKE_SOURCE_DIR}/in/in_5K.fluid"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fout_5K.fluid"
)

add_test(animate_5K
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/animate"
  1 100
  "${CMAKE_SOURCE_DIR}/in/in_5K.fluid"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/out_5K.fluid"
)
set_tests_properties(animate_5K PROPERTIES DEPENDS fanimate_5K)

add_test(cmpseq_5K
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fcmp"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/out_5K.fluid"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fout_5K.fluid"
  --ptol 0
  --vtol 0
  --bbox 0
  --verbose
)
set_tests_properties(cmpseq_5K PROPERTIES DEPENDS animate_5K)
set_tests_properties(cmpseq_5K PROPERTIES DEPENDS fanimate_5K)

add_test(animatetbb_5K
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/animate_tbb"
  4 100
  "${CMAKE_SOURCE_DIR}/in/in_5K.fluid"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/outtbb_5K.fluid"
)
set_tests_properties(animatetbb_5K PROPERTIES DEPENDS fanimate_5K)

add_test(cmptbb_5K
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fcmp"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/outtbb_5K.fluid"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fout_5K.fluid"
  --ptol 0.01
  --bbox 0.001
  --verbose
)
set_tests_properties(cmptbb_5K PROPERTIES DEPENDS animatetbb_5K)
set_tests_properties(cmptbb_5K PROPERTIES DEPENDS fanimate_5K)

add_test(fanimatetbb_5K
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fanimate_tbb"
  4 100
  "${CMAKE_SOURCE_DIR}/in/in_5K.fluid"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fouttbb_5K.fluid"
)
set_tests_properties(animatetbb_5K PROPERTIES DEPENDS fanimate_5K)

add_test(cmpftbb_5K
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fcmp"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fouttbb_5K.fluid"
  "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fout_5K.fluid"
  --ptol 0.01
  --bbox 0.001
  --verbose
)
set_tests_properties(cmpftbb_5K PROPERTIES DEPENDS fanimatetbb_5K)
set_tests_properties(cmpftbb_5K PROPERTIES DEPENDS fanimate_5K)



