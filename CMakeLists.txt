# CMakeList.txt : CMake project for colourful, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project ("colourful")

include(FetchContent)
include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# Add source to this project's executable.
add_executable (colourful "colourful.cpp" "colourful.h")

message("Fetching GoogleTest/GoogleMock...")
FetchContent_Declare(
  GoogleTest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)
# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# FetchContent_MakeAvailable(GoogleTest)
message("Fetch complete for GoogleTest/GoogleMock")