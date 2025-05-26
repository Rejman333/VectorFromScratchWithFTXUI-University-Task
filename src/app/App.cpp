#include "App.h"

#include <fstream>
#include <map>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"


static bool isNumber(const std::string &s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

App::App(): books_db(SmartArray<Book>()) {
}

void App::showPopUp(const std::string &message) {
    Component okBtn = Button("  OK  ", screen.ExitLoopClosure());

    auto container = Container::Vertical({okBtn});

    auto renderer = Renderer(container, [&] {
        return vbox({
                   text("=== Notification ===") | bold | center,
                   separator(),
                   window(text(" Info "), vbox({
                              filler(),
                              text(message) | center | bold,
                              filler()
                          })),
                   separator(),
                   hbox({filler(), okBtn->Render(), filler()})
               }) | center;
    });

    screen.Loop(renderer);
}

void App::save_books_to_bin() {
    std::ofstream out(binFile, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Nie można otworzyć pliku do zapisu.");
    }

    size_t count = books_db.size();
    out.write(reinterpret_cast<const char *>(&count), sizeof(count));

    for (size_t i = 0; i < books_db.size(); i++) {
        const Book &b = books_db[i];
        size_t isbn_len = b.ISBN.size();
        out.write(reinterpret_cast<const char *>(&isbn_len), sizeof(isbn_len));
        out.write(b.ISBN.data(), isbn_len);

        size_t title_len = b.title.size();
        out.write(reinterpret_cast<const char *>(&title_len), sizeof(title_len));
        out.write(b.title.data(), title_len);

        out.write(reinterpret_cast<const char *>(&b.author_id), sizeof(b.author_id));
        out.write(reinterpret_cast<const char *>(&b.price), sizeof(b.price));
        out.write(reinterpret_cast<const char *>(&b.copies_sold), sizeof(b.copies_sold));
    }
}

void App::load_books_from_bin() {
    std::ifstream in(binFile, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Nie można otworzyć pliku do odczytu.");
    }

    size_t count;
    in.read(reinterpret_cast<char *>(&count), sizeof(count));

    //ToDo debug this
    //books_db.reserve(books_db.size() + count);

    for (size_t i = 0; i < count; ++i) {
        size_t isbn_len;
        in.read(reinterpret_cast<char *>(&isbn_len), sizeof(isbn_len));
        std::string isbn(isbn_len, '\0');
        in.read(&isbn[0], isbn_len);

        size_t title_len;
        in.read(reinterpret_cast<char *>(&title_len), sizeof(title_len));
        std::string title(title_len, '\0');
        in.read(&title[0], title_len);

        size_t author_id;
        unsigned int price;
        unsigned int copies_sold;

        in.read(reinterpret_cast<char *>(&author_id), sizeof(author_id));
        in.read(reinterpret_cast<char *>(&price), sizeof(price));
        in.read(reinterpret_cast<char *>(&copies_sold), sizeof(copies_sold));

        books_db.pushBack(Book(isbn, title, author_id, price, copies_sold));
    }
}

void App::showMenu() {
    int selectedOption = 0;
    MenuOption menuOption;
    menuOption.on_enter = screen.ExitLoopClosure();

    Component menu = Menu(&menuEntries, &selectedOption, menuOption);

    auto root = Container::Vertical({menu});

    auto renderer = Renderer(root, [&] {
        return vbox({
                   text("=== Main Menu ===") | bold | center,
                   separator(),
                   menu->Render() | center,
               }) | center;
    });

    screen.Loop(renderer);

    activeScreen = static_cast<ActiveScreen>(selectedOption);
}

void App::showListBooksScreen() {
    // --- text filter ---
    std::string filterStr;
    Component filterInput = Input(&filterStr);

    // --- range from / to ---
    std::string rangeFromStr;
    std::string rangeToStr;
    Component rangeFromInput = Input(&rangeFromStr);
    Component rangeToInput = Input(&rangeToStr);

    // --- vector of persistent rows ---
    std::vector<Component> rowComps;

    // helper to build a row
    const std::vector authors_strings(authors.begin(), authors.end());
    auto makeRow = [&](unsigned index) {
        auto editBtn = Button("Edytuj", [&, index] {
            std::string ISBN = books_db[index].ISBN;
            std::string title = books_db[index].title;
            std::string price_str = std::to_string(books_db[index].price);
            std::string copies_sold_str = std::to_string(books_db[index].copies_sold);
            int selected_author = static_cast<int>(books_db[index].author_id);

            Component input_ISBN = Input(&ISBN);
            Component input_title = Input(&title);
            Component input_price_str = Input(&price_str);
            Component input_copies_sold_str = Input(&copies_sold_str);
            Component input_selected_author = Dropdown(&authors_strings, &selected_author);

            Component save = Button("Save", [&, index] {
                if (!ISBN.empty()) books_db[index].ISBN = ISBN;
                if (!title.empty()) books_db[index].title = title;
                if (isNumber(price_str)) books_db[index].price = std::stoi(price_str);
                if (isNumber(copies_sold_str)) books_db[index].price = std::stoi(copies_sold_str);
                screen.ExitLoopClosure()();
            });
            Component cancel = Button("Cancel", screen.ExitLoopClosure());

            auto dialog = Renderer(Container::Vertical({
                                       input_ISBN, input_title, input_price_str, input_copies_sold_str,
                                       input_selected_author, save, cancel
                                   }), [&] {
                                       return vbox({
                                                  text("===Edit Screen===") | bold | center,
                                                  separator(),
                                                  window(text(" Book Form "), vbox(
                                                             hbox({text("ISBN         : "), input_ISBN->Render()}),
                                                             hbox({text("Title        : "), input_title->Render()}),
                                                             hbox({text("Price [gr]   : "), input_price_str->Render()}),
                                                             hbox({
                                                                 text("Copies Sold  : "),
                                                                 input_copies_sold_str->Render()
                                                             }),
                                                             hbox({
                                                                 text("Author       : "),
                                                                 input_selected_author->Render()
                                                             })

                                                         )),
                                                  separator(),
                                                  hbox({save->Render(), text("   "), cancel->Render()})| center
                                              }) | center;
                                   });
            screen.Loop(dialog);
        });

        auto delBtn = Button("Delete", [&, index] {
            bool confirmed = false;
            Component yesBtn = Button("Yes", [&] {
                confirmed = true;
                screen.ExitLoopClosure()();
            }) | center;
            filler();
            Component noBtn = Button("No ", screen.ExitLoopClosure());
            auto confirmView = Renderer(Container::Vertical({yesBtn, noBtn}), [&] {
                return vbox({
                           text("Are you sure?") | center,
                           separator(),
                           hbox({yesBtn->Render(), noBtn->Render()}) | center | border
                       }) | center;
            });
            screen.Loop(confirmView);
            if (confirmed && index < books_db.size()) {
                // 1) remove record
                books_db.erase(index);
                rowComps.erase(rowComps.begin() + index);


                // simply refresh the entire list screen
                activeScreen = ListBooksScreen;
                screen.ExitLoopClosure()();
            }
        });

        // row renderer
        auto rowRenderer = Renderer(Container::Horizontal({editBtn, delBtn}), [&, index, editBtn, delBtn] {
            return hbox({
                hbox(
                    text(std::to_string(books_db[index].id)) | center | size(WIDTH, EQUAL, 3),
                    text(books_db[index].ISBN) | center | size(WIDTH, EQUAL, 10),
                    text(books_db[index].title) | center | size(WIDTH, EQUAL, 40) | color(Color::YellowLight),
                    text(books_db[index].get_author_name()) | center | size(WIDTH, EQUAL, 20) | color(Color::Blue),
                    text(books_db[index].get_price_str()) | center | size(WIDTH, EQUAL, 10),
                    text(std::to_string(books_db[index].copies_sold)) | center | size(WIDTH, EQUAL, 10),
                    text(books_db[index].get_total_revenue_str()) | center | size(WIDTH, EQUAL, 10) | color(
                        Color::GreenLight)
                ),
                filler(), editBtn->Render(), delBtn->Render()
            });
        });

        return rowRenderer;
    };

    // initial rows
    for (unsigned i = 0; i < books_db.size(); ++i)
        rowComps.push_back(makeRow(i));

    auto rowsContainer = Container::Vertical(rowComps);

    // add 3 spaces on the left and right to visually and practically widen the hit-box
    Component backButton = Button("   Exit   ", [&]() {
        activeScreen = MenuScreen;
        screen.ExitLoopClosure()();
    });

    auto root = Container::Vertical({filterInput, rangeFromInput, rangeToInput, rowsContainer, backButton});
    // --- main renderer ---
    auto renderer = Renderer(root, [&] {
        Elements rows;

        rows.push_back(text("=== Main Menu ===") | bold | center);
        rows.push_back(separator());
        // ---- filters header ----

        rows.push_back(hbox({text("Filter (ISBN / Title / Author): "), filterInput->Render(), filler()}));
        rows.push_back(hbox({
            text("From: "), rangeFromInput->Render(),
            text(" To: "), rangeToInput->Render()
        }));
        rows.push_back(separator());
        rows.push_back(hbox(
            text("ID") | center | bold | size(WIDTH, EQUAL, 3),
            text("ISBN") | center | bold | size(WIDTH, EQUAL, 10),
            text("Title") | center | bold | size(WIDTH, EQUAL, 40),
            text("Author") | center | bold | size(WIDTH, EQUAL, 20),
            text("Price") | center | bold | size(WIDTH, EQUAL, 10),
            text("Sold") | center | bold | size(WIDTH, EQUAL, 10),
            text("Revenue") | center | bold | size(WIDTH, EQUAL, 10)
        ));
        rows.push_back(separator());

        // Rebuild visible rowsContainer based on current filter/range
        rowsContainer->DetachAllChildren();
        for (unsigned i = 0; i < books_db.size(); i++) {
            const Book &b = books_db[i];
            // text filter: numeric input matches exact row number, otherwise substring match on name/index
            if (!filterStr.empty()) {
                bool match = false;
                bool isNum = std::all_of(filterStr.begin(), filterStr.end(), ::isdigit);
                if (isNum) {
                    unsigned f = std::stoul(filterStr);
                    match = (i == f);
                } else {
                    match = (b.ISBN.find(filterStr) != std::string::npos)
                            || (b.title.find(filterStr) != std::string::npos)
                            || (b.get_author_name().find(filterStr) != std::string::npos);
                }
                if (!match) continue;
            }
            // range filter
            if (!rangeFromStr.empty() && isNumber(rangeFromStr)) {
                unsigned fromIdx = std::stoul(rangeFromStr);
                if (i < fromIdx) continue;
            }
            if (!rangeToStr.empty() && isNumber(rangeToStr)) {
                unsigned toIdx = std::stoul(rangeToStr);
                if (i > toIdx) continue;
            }
            rowsContainer->Add(rowComps[i]);
        }

        rows.push_back(
            flex_grow(
                flex_shrink(
                    vscroll_indicator(frame(rowsContainer->Render()))
                )
            )
        );

        rows.push_back(separator());
        rows.push_back(
            hbox({filler(), backButton->Render(), filler()})
        );
        rows.push_back(separator());
        return vbox(rows);
    });

    screen.Loop(renderer);
}

void App::showAddBookScreen() {
    std::string ISBN;
    std::string title;
    std::string price_str;
    std::string copies_sold_str;

    int selected_author = 0;
    std::vector<std::string> authors_strings(authors.begin(), authors.end());

    Component ISBNInput = Input(&ISBN);
    Component titleInput = Input(&title);
    Component priceInput = Input(&price_str);
    Component copiesInput = Input(&copies_sold_str);
    Component authorSelect = Dropdown(&authors_strings, &selected_author);

    Component addBtn = Button("Add", [&] {
        if (!ISBN.empty() && !title.empty() && isNumber(price_str) && isNumber(copies_sold_str)) {
            int parsed_price = std::stoi(price_str);
            int parsed_copies_sold = std::stoi(copies_sold_str);
            Book book(ISBN, title, selected_author, parsed_price, parsed_copies_sold);
            books_db.pushBack(book);
            showPopUp("Book added!");
        } else {
            showPopUp("Invalid input!");
        }
        activeScreen = MenuScreen;
        screen.ExitLoopClosure()();
    });

    Component cancelBtn = Button("Exit", [&] {
        activeScreen = MenuScreen;
        screen.ExitLoopClosure()();
    });

    Component form = Container::Vertical({
        ISBNInput,
        titleInput,
        priceInput,
        copiesInput,
        authorSelect,
        addBtn,
        cancelBtn
    });

    auto renderer = Renderer(form, [&] {
        return vbox({
                   text("=== Add New Book ===") | bold | center,
                   separator(),
                   window(text(" Book Form "), vbox({
                              hbox({text("ISBN          : "), ISBNInput->Render()}),
                              hbox({text("Title         : "), titleInput->Render()}),
                              hbox({text("Price [gr]    : "), priceInput->Render()}),
                              hbox({text("Copies Sold   : "), copiesInput->Render()}),
                              hbox({text("Select Author : "), authorSelect->Render()}),
                          })),
                   separator(),
                   hbox({filler(), addBtn->Render(), text("   "), cancelBtn->Render(), filler()})
               }) | center;
    });

    screen.Loop(renderer);
}

void App::showSaveBookScreen() {
    save_books_to_bin();
    showPopUp("Saved");
    activeScreen = MenuScreen;
};

void App::showLodeBookScreen() {
    load_books_from_bin();
    showPopUp("Loaded");
    activeScreen = MenuScreen;
};

void App::showBestSealersScreen() {
    std::vector<std::pair<size_t, unsigned int> > index_revenue;
    index_revenue.reserve(books_db.size());

    for (size_t i = 0; i < books_db.size(); ++i) {
        index_revenue.emplace_back(i, books_db[i].get_total_revenue());
    }

    std::sort(index_revenue.begin(), index_revenue.end(),
              [](const auto &a, const auto &b) {
                  return a.second > b.second;
              });

    size_t number_of_best_sellers = std::min<size_t>(5, index_revenue.size());

    std::vector<Component> rowComps;
    rowComps.reserve(number_of_best_sellers);

    for (size_t i = 0; i < number_of_best_sellers; ++i) {
        const Book &b = books_db[index_revenue[i].first];
        unsigned int revenue = index_revenue[i].second;

        std::string revenue_str = std::to_string(revenue / 100) + "." +
                                  (revenue % 100 < 10 ? "0" : "") +
                                  std::to_string(revenue % 100) + " PLN";

        std::string author = b.get_author_name();
        std::string position = "#" + std::to_string(i + 1);

        auto row = Renderer([=] {
            return hbox({
                text(position + " ") | bold,
                text(b.title) | bold | color(Color::YellowLight),
                text("  by ") | dim,
                text(author) | color(Color::Blue),
                filler(),
                text(revenue_str) | bold | color(Color::GreenLight)
            });
        });

        rowComps.push_back(row);
    }

    auto rowsContainer = Container::Vertical(rowComps);

    Component backButton = Button("   Back   ", [&]() {
        activeScreen = MenuScreen;
        screen.ExitLoopClosure()();
    });

    auto root = Container::Vertical({rowsContainer, backButton});

    auto renderer = Renderer(root, [&] {
        Elements rows;

        rows.push_back(text("=== Top 5 Best Sellers ===") | bold | center);
        rows.push_back(separator());

        rows.push_back(window(text(" Bestseller Ranking "), vbox({
                                  separator(),
                                  rowsContainer->Render(),
                                  separator()
                              })));

        rows.push_back(separator());
        rows.push_back(hbox({filler(), backButton->Render(), filler()}));

        return vbox(rows) | center;
    });

    screen.Loop(renderer);
}

void App::showBooksStatisticsScreen() {
    size_t book_count = books_db.size();
    unsigned int total_copies_sold = 0;
    unsigned int total_revenue = 0;
    unsigned int total_price = 0;

    std::map<size_t, size_t> author_book_count;

    for (size_t i = 0; i < books_db.size(); ++i) {
        const Book &b = books_db[i];
        total_copies_sold += b.copies_sold;
        total_revenue += b.get_total_revenue();
        total_price += b.price;
        author_book_count[b.author_id]++;
    }

    std::string most_published_author = "N/A";
    size_t max_books = 0;
    for (const auto &[author_id, count]: author_book_count) {
        if (count > max_books) {
            max_books = count;
            most_published_author = authors[author_id];
        }
    }

    auto format_money = [](unsigned int grosze) {
        unsigned int zl = grosze / 100;
        unsigned int gr = grosze % 100;
        return std::to_string(zl) + "." + (gr < 10 ? "0" : "") + std::to_string(gr) + " PLN";
    };

    std::string avg_price_str = (book_count > 0) ? format_money(total_price / book_count) : "0.00 PLN";

    std::vector<Component> rowComps;

    auto makeRow = [](const std::string &label, const std::string &value, Color value_color = Color::White) {
        return Renderer([=] {
            return hbox({
                text(label + ": ") | bold | color(Color::YellowLight),
                text(value) | bold | color(value_color),
                filler()
            });
        });
    };

    rowComps.push_back(makeRow("Number of Books", std::to_string(book_count)));
    rowComps.push_back(makeRow("Number of Books Sold", std::to_string(total_copies_sold)));
    rowComps.push_back(makeRow("Total Revenue", format_money(total_revenue), Color::GreenLight));
    rowComps.push_back(makeRow("Average Book Price", avg_price_str, Color::GreenLight));
    rowComps.push_back(makeRow("Most Published Author", most_published_author, Color::Blue));

    auto rowsContainer = Container::Vertical(rowComps);

    Component backButton = Button("   Back   ", [&]() {
        activeScreen = MenuScreen;
        screen.ExitLoopClosure()();
    });

    auto root = Container::Vertical({rowsContainer, backButton});

    auto renderer = Renderer(root, [&] {
        Elements rows;

        rows.push_back(text("=== Book Statistics ===") | bold | center);
        rows.push_back(separator());

        rows.push_back(
            window(text(" Statistics Summary "), vbox({
                       separator(),
                       rowsContainer->Render(),
                       separator()
                   }))
        );

        rows.push_back(separator());
        rows.push_back(hbox({filler(), backButton->Render(), filler()}));

        return vbox(rows) | center;
    });

    screen.Loop(renderer);
}

void App::run() {
    while (activeScreen != Exit) {
        switch (activeScreen) {
            case AddBookScreen: showAddBookScreen();
                break;
            case MenuScreen: showMenu();
                break;
            case ListBooksScreen: showListBooksScreen();
                break;
            case SaveBookScreen: showSaveBookScreen();
                break;
            case LodeBookScreen: showLodeBookScreen();
                break;
            case BestSealers: showBestSealersScreen();
                break;
            case BooksStatistics: showBooksStatisticsScreen();
                break;
            default: activeScreen = MenuScreen;
                break;
        }
    }
}
