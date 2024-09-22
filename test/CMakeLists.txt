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

add_executable(tests tests.cpp)
target_link_libraries(
  tests
  PRIVATE walle::walle_warnings
          walle::walle_options
          walle::library
          Catch2::Catch2WithMain)

if(WIN32 AND BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET tests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:tests> $<TARGET_FILE_DIR:tests>
    COMMAND_EXPAND_LISTS)
endif()

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  tests
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
target_link_libraries(
  test_sync_semaphore
  PRIVATE walle::walle_warnings
          walle::walle_options
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
target_link_libraries(
  test_sync_tagged_semaphore
  PRIVATE walle::walle_warnings
          walle::walle_options
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
target_link_libraries(
  test_sync_wait_group
  PRIVATE walle::walle_warnings
          walle::walle_options
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
