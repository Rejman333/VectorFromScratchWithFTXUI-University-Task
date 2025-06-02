cmake_minimum_required(VERSION 3.20)
project(JiPP_project)

set(CMAKE_CXX_STANDARD 20)

# Import FTXUI

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG main
)
FetchContent_MakeAvailable(ftxui)



add_library(JiPP_project_smartArray
        src/smartArray/SmartArray.cpp
        src/smartArray/SmartArray.h)

add_executable(JiPP_project_app
        src/app/main.cpp
        src/book/book.cpp
        src/book/book.h
        src/app/App.cpp
        src/app/App.h

)

target_link_libraries(JiPP_project_app JiPP_project_smartArray)

# Add FTXUI to JiPP_project_cpp

target_link_libraries(JiPP_project_app ftxui::screen ftxui::dom ftxui::component)


add_subdirectory(tests)
