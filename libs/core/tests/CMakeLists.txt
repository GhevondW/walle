cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()
endif()

# ---- Dependencies ----

# Function to add a Google Test executable and register it
function(walle_core_add_gtest test_name test_source)
  add_executable(${test_name} ${test_source})
  target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(
    ${test_name}
    PRIVATE walle::walle_options
            walle::core
            gtest
            gtest_main)
  add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

walle_core_add_gtest(test_core_semaphore test_core_semaphore.cpp)
walle_core_add_gtest(test_core_wait_group test_core_wait_group.cpp)
walle_core_add_gtest(test_core_tagged_semaphore test_core_tagged_semaphore.cpp)
walle_core_add_gtest(test_core_unbounded_blocking_queue test_core_unbounded_blocking_queue.cpp)
