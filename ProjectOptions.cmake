include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
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
  option(walle_ENABLE_HARDENING "Enable hardening" ON)
  option(walle_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    walle_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    walle_ENABLE_HARDENING
    OFF)

  walle_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR walle_PACKAGING_MAINTAINER_MODE)
    option(walle_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(walle_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(walle_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(walle_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(walle_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(walle_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(walle_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(walle_ENABLE_PCH "Enable precompiled headers" OFF)
    option(walle_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(walle_ENABLE_IPO "Enable IPO/LTO" ON)
    option(walle_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(walle_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(walle_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(walle_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(walle_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(walle_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(walle_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(walle_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(walle_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(walle_ENABLE_PCH "Enable precompiled headers" OFF)
    option(walle_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      walle_ENABLE_IPO
      walle_WARNINGS_AS_ERRORS
      walle_ENABLE_USER_LINKER
      walle_ENABLE_SANITIZER_ADDRESS
      walle_ENABLE_SANITIZER_LEAK
      walle_ENABLE_SANITIZER_UNDEFINED
      walle_ENABLE_SANITIZER_THREAD
      walle_ENABLE_SANITIZER_MEMORY
      walle_ENABLE_UNITY_BUILD
      walle_ENABLE_CLANG_TIDY
      walle_ENABLE_CPPCHECK
      walle_ENABLE_COVERAGE
      walle_ENABLE_PCH
      walle_ENABLE_CACHE)
  endif()

  walle_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED AND (walle_ENABLE_SANITIZER_ADDRESS OR walle_ENABLE_SANITIZER_THREAD OR walle_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(walle_BUILD_FUZZ_TESTS "Enable fuzz testing executable" ${DEFAULT_FUZZER})

endmacro()

macro(walle_global_options)
  if(walle_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    walle_enable_ipo()
  endif()

  walle_supports_sanitizers()

  if(walle_ENABLE_HARDENING AND walle_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR walle_ENABLE_SANITIZER_UNDEFINED
       OR walle_ENABLE_SANITIZER_ADDRESS
       OR walle_ENABLE_SANITIZER_THREAD
       OR walle_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${walle_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${walle_ENABLE_SANITIZER_UNDEFINED}")
    walle_enable_hardening(walle_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
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

  if(walle_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    walle_configure_linker(walle_options)
  endif()

  include(cmake/Sanitizers.cmake)
  walle_enable_sanitizers(
    walle_options
    ${walle_ENABLE_SANITIZER_ADDRESS}
    ${walle_ENABLE_SANITIZER_LEAK}
    ${walle_ENABLE_SANITIZER_UNDEFINED}
    ${walle_ENABLE_SANITIZER_THREAD}
    ${walle_ENABLE_SANITIZER_MEMORY})

  set_target_properties(walle_options PROPERTIES UNITY_BUILD ${walle_ENABLE_UNITY_BUILD})

  if(walle_ENABLE_PCH)
    target_precompile_headers(
      walle_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(walle_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    walle_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(walle_ENABLE_CLANG_TIDY)
    walle_enable_clang_tidy(walle_options ${walle_WARNINGS_AS_ERRORS})
  endif()

  if(walle_ENABLE_CPPCHECK)
    walle_enable_cppcheck(${walle_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(walle_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    walle_enable_coverage(walle_options)
  endif()

  if(walle_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(walle_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(walle_ENABLE_HARDENING AND NOT walle_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR walle_ENABLE_SANITIZER_UNDEFINED
       OR walle_ENABLE_SANITIZER_ADDRESS
       OR walle_ENABLE_SANITIZER_THREAD
       OR walle_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    walle_enable_hardening(walle_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
