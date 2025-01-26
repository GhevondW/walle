include(cmake/SystemLink.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)


macro(walle_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND NOT WIN32)
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(walle_setup_options)
  walle_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR walle_PACKAGING_MAINTAINER_MODE)
    option(walle_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(walle_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(walle_ENABLE_PCH "Enable precompiled headers" OFF)
    option(walle_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(walle_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(walle_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    # option(walle_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    # option(walle_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(walle_ENABLE_PCH "Enable precompiled headers" OFF)
    option(walle_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      walle_WARNINGS_AS_ERRORS
      walle_ENABLE_SANITIZER_ADDRESS
      walle_ENABLE_SANITIZER_LEAK
      walle_ENABLE_SANITIZER_UNDEFINED
      walle_ENABLE_SANITIZER_THREAD
      walle_ENABLE_SANITIZER_MEMORY
      walle_ENABLE_PCH
      walle_ENABLE_CACHE)
  endif()

endmacro()

macro(walle_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(walle_warnings INTERFACE)
  add_library(walle_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  walle_set_project_warnings(
    walle_warnings
    ${walle_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  include(cmake/Sanitizers.cmake)
  walle_enable_sanitizers(
    walle_options
    ${walle_ENABLE_SANITIZER_ADDRESS}
    ${walle_ENABLE_SANITIZER_LEAK}
    ${walle_ENABLE_SANITIZER_UNDEFINED}
    ${walle_ENABLE_SANITIZER_THREAD}
    ${walle_ENABLE_SANITIZER_MEMORY})

  if(walle_ENABLE_PCH)
    target_precompile_headers(
      walle_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(walle_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(walle_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()
endmacro()
