cmake_minimum_required(VERSION 3.2)
project(cer2nutlib)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# uncomment to enable profiling
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pg")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

file(GLOB_RECURSE LIB_SOURCE_FILES lib/*.cpp)

include_directories(lib)

file(GLOB_RECURSE TEST_SOURCE_FILES test/*.cpp)
enable_testing()
add_executable(unittests ${LIB_SOURCE_FILES} ${TEST_SOURCE_FILES})
target_link_libraries(unittests GTest::gmock_main)
include(GoogleTest)
gtest_discover_tests(unittests)
