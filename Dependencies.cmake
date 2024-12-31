include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(walle_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.3.2")
  endif()

  # target_link_libraries(CPMExampleGtest fibonacci gtest gtest_main gmock)
  CPMAddPackage(
    NAME googletest
    GITHUB_REPOSITORY google/googletest
    GIT_TAG release-1.12.1
    VERSION 1.12.1
    OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt"
  )

  # TODO : change it with my asan option
  add_compile_definitions(BOOST_USE_ASAN)
  set(BOOST_CONTEXT_IMPLEMENTATION "ucontext")
  # boost is a huge project and directly downloading the 'alternate release'
  # from github is much faster than recursively cloning the repo.
  CPMAddPackage(
    NAME Boost
    VERSION 1.84.0
    URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
    URL_HASH SHA256=2e64e5d79a738d0fa6fb546c6e5c2bd28f88d268a2a080546f74e5ff98f29d0e
    OPTIONS "BOOST_ENABLE_CMAKE ON"
  )

  CPMAddPackage(
    NAME function2
    VERSION 4.2.1 # Use the appropriate version of function2 that you need
    GITHUB_REPOSITORY Naios/function2
    GIT_TAG 4.2.1 # This should match the version you want to use
  )  

endfunction()
