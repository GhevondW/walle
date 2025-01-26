# TODO add headers here too
add_library(walle_async
    ${CMAKE_CURRENT_SOURCE_DIR}/src/go.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/task_context.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/task_handle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/this_task.cpp
)

add_library(walle::async ALIAS walle_async)

target_link_libraries(walle_async PRIVATE walle_options walle_warnings)
target_link_system_libraries(walle_async PUBLIC walle::cortex walle::exec function2::function2 Boost::smart_ptr)
target_include_directories(walle_async ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(walle_async ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_compile_features(walle_async PUBLIC cxx_std_20)

set_target_properties(
  walle_async
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(walle_async PUBLIC walle_async_STATIC_DEFINE)
endif()

# Don't even look at tests if we're not top level
if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# Adding the tests:
include(CTest)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
