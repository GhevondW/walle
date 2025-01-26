cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()
endif()

# ---- Dependencies ----

# Function to add a Google Test executable and register it
function(walle_async_add_gtest test_name test_source)
  add_executable(${test_name} ${test_source})
  target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(
    ${test_name}
    PRIVATE walle::walle_options
            walle::async
            walle::core
            gtest
            gtest_main)
  add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

walle_async_add_gtest(test_task test_task.cpp)
