cmake_minimum_required(VERSION 3.21)

# This template attempts to be "fetch_content"-able
# so that it works well with tools like CPM or other
# manual dependency management

# Only set the cxx_standard if it is not set by someone else
if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
endif()

# Set the project name and language
project(
  walle
  VERSION 0.0.1
  DESCRIPTION ""
  HOMEPAGE_URL "https://github.com/GhevondW/walle"
  LANGUAGES CXX C)

include(cmake/PreventInSourceBuilds.cmake)
include(ProjectOptions.cmake)

walle_setup_options()

include(Dependencies.cmake)
walle_setup_dependencies()

walle_local_options()

# Helper function to print options in green
function(print_option name value)
    message("WALLE LOG : ${color}${name}: ${value}${reset}")
endfunction()

# Print all options in green
print_option("walle_WARNINGS_AS_ERRORS" ${walle_WARNINGS_AS_ERRORS})
print_option("walle_ENABLE_SANITIZER_ADDRESS" ${walle_ENABLE_SANITIZER_ADDRESS})
print_option("walle_ENABLE_SANITIZER_LEAK" ${walle_ENABLE_SANITIZER_LEAK})
print_option("walle_ENABLE_SANITIZER_UNDEFINED" ${walle_ENABLE_SANITIZER_UNDEFINED})
print_option("walle_ENABLE_SANITIZER_THREAD" ${walle_ENABLE_SANITIZER_THREAD})
print_option("walle_ENABLE_SANITIZER_MEMORY" ${walle_ENABLE_SANITIZER_MEMORY})
print_option("walle_ENABLE_PCH" ${walle_ENABLE_PCH})
print_option("walle_ENABLE_CACHE" ${walle_ENABLE_CACHE})

# Check the build mode for single-configuration generators
if(CMAKE_BUILD_TYPE)
    message("Build Mode: ${CMAKE_BUILD_TYPE}")
else()
    # For multi-configuration generators, CMAKE_BUILD_TYPE is not set
    # Use the configuration specified during the build (e.g., Debug, Release)
    message("Build Mode: Multi-configuration generator. Specify configuration at build time (e.g., --config Debug).")
endif()

target_compile_features(walle_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_library(walle::walle_options ALIAS walle_options)
add_library(walle::walle_warnings ALIAS walle_warnings)

# configure files based on CMake configuration options
add_subdirectory(configured_files)

# Adding the src:
add_subdirectory(libs)

