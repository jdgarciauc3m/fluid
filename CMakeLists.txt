cmake_minimum_required (VERSION 2.8)
project (fluidanimate)

#Global definitions
option(FLUID_CFL_CHECK "Activate CFL check. Do not use for performance measures")
if (FLUID_CFL_CHECK)
  add_definitions(-DENABLE_CFL_CHECK)
endif()

option(FLUID_STATISTICS "Activate statistics. Do n ot use for performance measures")
if (FLUID_STATISTICS)
  add_definitions(-DENABLE_STATISTICS)
endif()

option(FLUID_VISUALIZATION "Compile with visualization")
if(FLUID_VISUALIZATION)
  add_definitions(-DENABLE_VISUALIZATION)
endif()

option(FLUID_TIMING "Activate timing")
if (FLUID_TIMING)
  add_definitions(-DXUL_TIME_METER_ENABLED)
else()
  add_definitions(-DXUL_TIME_METER_DISABLED)
endif()

option(FLUID_IMPENERATABLE_WALLS "Impeneratable Walls" ON)
if (FLUID_IMPENERATABLE_WALLS)
  add_definitions(-DUSE_ImpeneratableWall)
endif()

option(FLUID_ENABLE_DOUBLE_PRECISION "Enable double precision")
IF (FLUID_ENABLE_DOUBLE_PRECISION)
  add_definitions(-DENABLE_DOUBLE_PRECISION)
endif()

enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
include_directories("${CMAKE_SOURCE_DIR}/include")

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-deprecated -Werror -pedantic-errors")

add_subdirectory(apps)
