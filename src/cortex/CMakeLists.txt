# TODO add headers here too
add_library(cortex_library 
    ${CMAKE_CURRENT_SOURCE_DIR}/coroutine.cpp
)

add_library(cortex::library ALIAS cortex_library)

target_link_libraries(cortex_library PRIVATE walle_options walle_warnings)
target_link_system_libraries(cortex_library PUBLIC function2::function2)
target_link_system_libraries(cortex_library PUBLIC Boost::context)

# TODO fix this issue : this is not good to be able to include from walle
target_include_directories(cortex_library ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

target_compile_features(cortex_library PUBLIC cxx_std_20)

set_target_properties(
    cortex_library
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(cortex_library PUBLIC cortex_library_STATIC_DEFINE)
endif()
