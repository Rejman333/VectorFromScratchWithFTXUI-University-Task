cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(CTest)
enable_testing()

add_executable(smartAraay_tests tests.cpp
        ../src/book/book.cpp
        ../src/book/book.h
        ../src/app/App.cpp
        ../src/app/App.h)

target_link_libraries(smartAraay_tests JiPP_project_smartArray)

target_link_libraries(smartAraay_tests GTest::gtest_main)


include(GoogleTest)
gtest_discover_tests(smartAraay_tests)