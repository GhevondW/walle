# TODO add headers here too
add_library(walle_exec 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/current_executor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/event_loop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/manual_executor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/thread_pool.cpp
)

add_library(walle::exec ALIAS walle_exec)

target_link_libraries(walle_exec PRIVATE walle_options walle_warnings)
target_link_system_libraries(walle_exec PUBLIC walle::core function2::function2)

target_include_directories(walle_exec ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_features(walle_exec PUBLIC cxx_std_20)

set_target_properties(
  walle_exec
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(walle_exec PUBLIC walle_exec_STATIC_DEFINE)
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