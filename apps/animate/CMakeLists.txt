cmake_minimum_required (VERSION 2.8)

LIST(APPEND FANIMATE_SOURCES main.cpp)
#if (FLUID_VISUALIZATION)
#  LIST(APPEND FANIMATE_SOURCES fluidview.cpp)
#endif()

add_executable(animate ${FANIMATE_SOURCES})

#if (FLUID_VISUALIZATION)
#  target_link_libraries(animate glut GLU)
#endif()
