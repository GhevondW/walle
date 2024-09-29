cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()

  find_package(walle CONFIG REQUIRED) # for intro, project_options, ...

  if(NOT TARGET walle_options)
    message(FATAL_ERROR "Required config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

# ---- Dependencies ----

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

add_executable(test_core_fast_pimpl test_core_fast_pimpl.cpp common/sample_pimpl.cpp)
target_include_directories(test_core_fast_pimpl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
  test_core_fast_pimpl
  PRIVATE walle::walle_options
          walle::library
          Catch2::Catch2WithMain)

catch_discover_tests(
  test_core_fast_pimpl
  TEST_PREFIX
  "unittests."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)

add_executable(test_sync_semaphore test_sync_semaphore.cpp)
target_include_directories(test_core_fast_pimpl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
  test_sync_semaphore
  PRIVATE walle::walle_options
          walle::library
          Catch2::Catch2WithMain)

catch_discover_tests(
  test_sync_semaphore
  TEST_PREFIX
  "unittests."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)

add_executable(test_sync_tagged_semaphore test_sync_tagged_semaphore.cpp)
target_include_directories(test_core_fast_pimpl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
  test_sync_tagged_semaphore
  PRIVATE walle::walle_options
          walle::library
          Catch2::Catch2WithMain)

catch_discover_tests(
  test_sync_tagged_semaphore
  TEST_PREFIX
  "unittests."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)

add_executable(test_sync_wait_group test_sync_wait_group.cpp)
target_include_directories(test_core_fast_pimpl PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
  test_sync_wait_group
  PRIVATE walle::walle_options
          walle::library
          Catch2::Catch2WithMain)

catch_discover_tests(
  test_sync_wait_group
  TEST_PREFIX
  "unittests."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)
