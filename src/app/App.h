#pragma once

#include <string>
#include <vector>
#include "../book/book.h"
#include "../smartArray/SmartArray.h"

#include "ftxui/component/screen_interactive.hpp"

using namespace ftxui;

enum ActiveScreen {
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


    void show_menu();

    void showListBooksScreen();

    void show_add_book_screen();

    void show_save_book_screen();

    void show_lode_book_screen();

    void show_best_sealers_screen();

    void show_books_statistics_screen();

    void show_pop_up(const std::string &message);

    void save_books_to_bin();

    void load_books_from_bin();

public:
    App();

    void run();
};
