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
            walle::asymtx
            gtest
            gtest_main)
  add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

walle_async_add_gtest(test_test_async_scope test_async_scope.cpp)
walle_async_add_gtest(test_asymtx_sync_spawn test_sync_spawn.cpp)
walle_async_add_gtest(test_asymtx_sync_task test_sync_task.cpp)
walle_async_add_gtest(test_asymtx_task test_task.cpp)
