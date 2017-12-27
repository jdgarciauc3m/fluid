cmake_minimum_required (VERSION 2.8)

LIST(APPEND FANIMATE_SOURCES main.cpp)
#if (FLUID_VISUALIZATION)
#  LIST(APPEND FANIMATE_SOURCES fluidview.cpp)
#endif()

add_executable(animate_tbb ${FANIMATE_SOURCES})

target_link_libraries(animate_tbb tbb pthread)

#if (FLUID_VISUALIZATION)
#  target_link_libraries(animate glut GLU)
#endif()
