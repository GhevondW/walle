# TODO add headers here too
add_library(walle_core 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/semaphore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/wait_group.cpp
)

add_library(walle::core ALIAS walle_core)

target_link_libraries(walle_core PRIVATE walle_options walle_warnings)

target_include_directories(walle_core ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_features(walle_core PUBLIC cxx_std_20)

set_target_properties(
  walle_core
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(walle_core PUBLIC walle_core_STATIC_DEFINE)
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