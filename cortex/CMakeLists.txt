add_library(cortex_library 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cortex.cpp
)

add_library(cortex::library ALIAS cortex_library)

target_link_libraries(cortex_library PRIVATE walle_options walle_warnings)
target_link_system_libraries(cortex_library PUBLIC function2::function2)
target_link_system_libraries(cortex_library PUBLIC Boost::context)

target_include_directories(cortex_library ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_features(cortex_library PUBLIC cxx_std_20)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(cortex_library PUBLIC cortex_library_STATIC_DEFINE)
endif()

# -------------------------- Tests --------------------------
# Don't even look at tests if we're not top level
if(NOT PROJECT_IS_TOP_LEVEL)
  return()
endif()

# Adding the tests:
include(CTest)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()


