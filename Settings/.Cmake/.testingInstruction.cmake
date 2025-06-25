# [Testing Flag and Settings Overflow]
## Setting the option to determine whether the test is enabled or the compile (for windows) shall be executed
## When users feel like use cmake instructions for executing non-test code, the instructions are shown as below,
## "cmake -S . -B build -DBUILD_TESTS=OFF && cmake --build build".
option(BUILD_TEST "The default testing variable is on" ON)

## Enabling test
if (BUILD_TEST)
  enable_testing()
endif()

# [Google Test Settings]
## Google test requires
if (BUILD_TEST)
  # Trying to find GTest package
  find_package(GTest QUIET)
  if (GTest_FOUND)
    message(STATUS "System-installed: GOOGLE TEST")
    set(GTEST_LIB GTest::gtest)
    set(GTEST_MAIN_LIB GTest::gtest_main)
  else()

    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/305e5a238b3c8d11266fbafd85520fb6b3184851.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
  endif()
endif()