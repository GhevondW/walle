include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(walle_setup_dependencies)

  find_package(boost CONFIG REQUIRED)
  find_package(spdlog CONFIG REQUIRED)

  # target_link_libraries(CPMExampleGtest fibonacci gtest gtest_main gmock)
  CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG release-1.12.1
    VERSION 1.12.1
    OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
  )

  CPMAddPackage(
    NAME function2
    VERSION 4.2.1 # Use the appropriate version of function2 that you need
    GITHUB_REPOSITORY Naios/function2
    GIT_TAG 4.2.1 # This should match the version you want to use
  )  

endfunction()
