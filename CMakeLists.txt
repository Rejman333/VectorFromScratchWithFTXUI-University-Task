cmake_minimum_required(VERSION 3.20)
project(JiPP_project)

set(CMAKE_CXX_STANDARD 20)

# Import FTXUI

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG main # Important: Specify a version or a commit hash here.
)
FetchContent_MakeAvailable(ftxui)



add_library(JiPP_project_smartArray
        src/smartArray/SmartArray.cpp
        src/smartArray/SmartArray.h
        src/book/book.cpp
        src/book/book.h)

add_executable(JiPP_project_app
        src/app/main.cpp
        src/book/book.cpp
        src/book/book.h
)

target_link_libraries(JiPP_project_app JiPP_project_smartArray)

# Add FTXUI to JiPP_project_cpp

target_link_libraries(JiPP_project_app ftxui::screen ftxui::dom ftxui::component)

add_subdirectory(tests)
