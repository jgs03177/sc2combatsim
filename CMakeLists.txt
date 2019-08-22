cmake_minimum_required (VERSION 3.15)
#cmake_policy(SET CMP0091 NEW)
project (SC2CombatSim CXX)

set(SC2CombatSim_VERSION_MAJOR 1)
set(SC2CombatSim_VERSION_MINOR 0)

set(PATH_MAPS "${PROJECT_SOURCE_DIR}/maps/")
set(PATH_CONFIG "${PROJECT_SOURCE_DIR}/config/")

configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/ProjectConfig.h.in"
  "${PROJECT_BINARY_DIR}/generated/ProjectConfig.h"
)

if (MSVC)
message(STATUS
    "MSVC -> forcing use of dynamically-linked runtime."
)
    #config yourself.
    #set(CMAKE_CXX_FLAGS_DEBUG "/MTd ${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING "" FORCE)
    #set(CMAKE_CXX_FLAGS_MINSIZEREL "/MT ${CMAKE_CXX_FLAGS_MINSIZEREL}" CACHE STRING "" FORCE)
    #set(CMAKE_CXX_FLAGS_RELEASE "/MT ${CMAKE_CXX_FLAGS_RELEASE}" CACHE STRING "" FORCE)
    #set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}" CACHE STRING "" FORCE)
else()
    set(CMAKE_CXX_FLAGS "-fpermissive")  
endif()

# Build with c++14 support.
set(CMAKE_CXX_STANDARD 14)

include_directories("${PROJECT_BINARY_DIR}/generated")

add_subdirectory(src)
