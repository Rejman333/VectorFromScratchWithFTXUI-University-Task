#pragma once

#include <string>
#include <vector>
#include "../book/book.h"
#include "../smartArray/SmartArray.h"

#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;

enum ActiveScreen {
    EditScreen = -2,
    MenuScreen = -1,
    AddBookScreen = 0,
    ListBooksScreen,
    BestSealers,
    BooksStatistics,
    SaveBookScreen,
    LodeBookScreen,
    Exit
};

class App {
    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    std::vector<std::string> menuEntries = {
        "Add Book",
        "Book List",
        "Best Sellers",
        "Statistics",
        "Save to .bin",
        "Lode from .bin",
        "Exit"
    };

    ActiveScreen activeScreen = MenuScreen;

    SmartArray<Book> books_db;

    const std::string binFile = "../books.bin";


    void showMenu();

    void showListBooksScreen();

    void showAddBookScreen();

    void showSaveBookScreen();

    void showLodeBookScreen();

    void showBestSealersScreen();

    void showBooksStatisticsScreen();

    void showPopUp(const std::string &message);

    void showEditScreen();

    void save_books_to_bin();

    void load_books_from_bin();

public:
    App();

    void run();
};
