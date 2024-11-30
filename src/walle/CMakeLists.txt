# TODO add headers here too
add_library(walle_library 
    ${CMAKE_CURRENT_SOURCE_DIR}/core/coroutine_handle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/coroutine_stack_allocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/event_loop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/core/manual_executor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sync/semaphore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sync/wait_group.cpp
)

add_library(walle::library ALIAS walle_library)

target_link_libraries(walle_library PRIVATE walle_options walle_warnings)
target_link_system_libraries(walle_library PUBLIC function2::function2)
target_link_system_libraries(walle_library PRIVATE Boost::context)

target_include_directories(walle_library ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)

target_compile_features(walle_library PUBLIC cxx_std_20)

set_target_properties(
  walle_library
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(walle_library PUBLIC walle_library_STATIC_DEFINE)
endif()
