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

# Function to add a Google Test executable and register it
function(walle_add_gtest test_name test_source)
  add_executable(${test_name} ${test_source})
  target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(
    ${test_name}
    PRIVATE walle::walle_options
            walle::library
            gtest
            gtest_main)
  add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

# walle_add_gtest(test_core_fast_pimpl "test_core_fast_pimpl.cpp;common/sample_pimpl.cpp")
walle_add_gtest(test_std_resumable test_std_resumable.cpp)
walle_add_gtest(test_core_coroutine_handle test_core_coroutine_handle.cpp)
walle_add_gtest(test_sync_semaphore test_sync_semaphore.cpp)
walle_add_gtest(test_sync_tagged_semaphore test_sync_tagged_semaphore.cpp)
walle_add_gtest(test_sync_wait_group test_sync_wait_group.cpp)
walle_add_gtest(test_unbounded_blocking_queue test_unbounded_blocking_queue.cpp)
walle_add_gtest(test_event_loop test_event_loop.cpp)
walle_add_gtest(test_manual_executor test_manual_executor.cpp)
walle_add_gtest(test_thread_pool test_thread_pool.cpp)

