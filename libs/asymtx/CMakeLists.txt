# TODO add headers here too
add_library(walle_asymtx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/all.cpp
)

add_library(walle::asymtx ALIAS walle_asymtx)

target_link_libraries(walle_asymtx PRIVATE walle_options walle_warnings)
target_link_system_libraries(walle_asymtx PUBLIC walle::core walle::exec)
target_include_directories(walle_asymtx ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(walle_asymtx ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)

target_compile_features(walle_asymtx PUBLIC cxx_std_20)

set_target_properties(
    walle_asymtx
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(walle_asymtx PUBLIC walle_asymtx_STATIC_DEFINE)
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
