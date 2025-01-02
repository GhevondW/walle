# TODO add headers here too
add_library(walle_cortex 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/coroutine.cpp
)

add_library(walle::cortex ALIAS walle_cortex)

target_link_libraries(walle_cortex PRIVATE walle_options walle_warnings)
target_link_system_libraries(walle_cortex PUBLIC function2::function2)
target_link_system_libraries(walle_cortex PRIVATE Boost::context)

target_include_directories(walle_cortex ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_features(walle_cortex PUBLIC cxx_std_20)

set_target_properties(
  walle_cortex
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(walle_cortex PUBLIC walle_cortex_STATIC_DEFINE)
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
