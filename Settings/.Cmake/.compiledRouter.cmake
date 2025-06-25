# [Debug & Testing/Release Settings]
## When the testing function is set as "ON"
if (BUILD_TEST)
  add_subdirectory(Tests)
else() # The necessary directory when compiling on Windows/Linux
  add_subdirectory(Apps)
endif()