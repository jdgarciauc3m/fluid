cmake_minimum_required (VERSION 2.8)

add_definitions( -D_GNU_SOURCE -D_XOPEN_SOURCE=600)

LIST(APPEND FANIMATE_SOURCES serial.cpp cellpool.cpp)
if (FLUID_VISUALIZATION)
  LIST(APPEND FANIMATE_SOURCES fluidview.cpp)
endif()
add_executable(fanimate ${FANIMATE_SOURCES})
if (FLUID_VISUALIZATION)
  target_link_libraries(fanimate glut GLU)
endif()

add_executable(fcmp fluidcmp.cpp)
