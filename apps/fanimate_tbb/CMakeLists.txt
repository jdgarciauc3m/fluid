cmake_minimum_required (VERSION 2.8)

add_definitions( -D_GNU_SOURCE -D_XOPEN_SOURCE=600)

LIST(APPEND FANIMATE_SOURCES tbb.cpp cellpool.cpp)
if (FLUID_VISUALIZATION)
  LIST(APPEND FANIMATE_SOURCES fluidview.cpp)
endif()

add_executable(fanimate_tbb ${FANIMATE_SOURCES})
target_link_libraries(fanimate_tbb tbb tbbmalloc)
if (FLUID_VISUALIZATION)
  target_link_libraries(fanimate glut GLU)
endif()
