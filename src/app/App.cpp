#include "App.h"

#include <fstream>
#include <map>

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"


static bool is_number(const std::string &s) {
    return !s.empty() && std::ranges::all_of(s, ::isdigit);
}

App::App(): books_db(SmartArray<Book>()) {
}

void App::show_pop_up(const std::string &message) {
    Component ok_button = Button("  OK  ", screen.ExitLoopClosure());

    auto container = Container::Vertical({ok_button});

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
                   hbox({filler(), ok_button->Render(), filler()})
               }) | center;
    });

    screen.Loop(renderer);
}

void App::save_books_to_bin() {
    std::ofstream out(binFile, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Failed to open a file.");
    }

    const size_t count = books_db.size();
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
        throw std::runtime_error("Failed to open a file.");
    }

    size_t count;
    in.read(reinterpret_cast<char *>(&count), sizeof(count));

    books_db.reserve(books_db.size() + count);

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

void App::show_menu() {
    int selectedOption = 0;
    MenuOption menuOption;
    menuOption.on_enter = screen.ExitLoopClosure();

    Component menu = Menu(&menuEntries, &selectedOption, menuOption);

    const auto root = Container::Vertical({menu});

    const auto renderer = Renderer(root, [&] {
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
    unsigned int number_of_books_after_filter = 0;
    unsigned int all_sells = 0;
    unsigned int all_price = 0;

    std::vector<std::string> authors_strings(authors.begin(), authors.end());
    authors_strings.emplace_back("None");

    //Setting up filters
    std::string filter_ISBN_str;
    std::string filter_title_str;
    int author_stop_id = static_cast<int>(authors_strings.size()) - 1;
    int filter_author_id = author_stop_id;

    Component filter_ISBN_input = Input(&filter_ISBN_str);
    Component filter_title_input = Input(&filter_title_str);
    Component filter_author_id_input = Dropdown(authors_strings, &filter_author_id);

    //Setting up range operators
    std::string range_from_str;
    std::string range_to_str;
    Component range_from_input = Input(&range_from_str);
    Component range_to_input = Input(&range_to_str);


    std::vector<Component> row_components;
    auto makeRow = [&](size_t index) {
        auto edit_button = Button("Edit", [&, index] {
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
                if (is_number(price_str)) books_db[index].price = std::stoi(price_str);
                if (is_number(copies_sold_str)) books_db[index].copies_sold = std::stoi(copies_sold_str);
                books_db[index].author_id = selected_author;
                screen.ExitLoopClosure()();
            });
            Component cancel = Button("Cancel", screen.ExitLoopClosure());

            auto form_component = Renderer(Container::Vertical({
                                               input_ISBN, input_title, input_price_str, input_copies_sold_str,
                                               input_selected_author, save, cancel
                                           }), [&] {
                                               return vbox({
                                                          text("===Edit Screen===") | bold | center,
                                                          separator(),
                                                          window(text(" Book Form "), vbox(
                                                                     hbox({
                                                                         text("ISBN         : "), input_ISBN->Render()
                                                                     }),
                                                                     hbox({
                                                                         text("Title        : "), input_title->Render()
                                                                     }),
                                                                     hbox({
                                                                         text("Price [gr]   : "),
                                                                         input_price_str->Render()
                                                                     }),
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
                                                          hbox({save->Render(), text("   "), cancel->Render()}) | center
                                                      }) | center;
                                           });
            screen.Loop(form_component);
        });

        auto delete_button = Button("Delete", [&, index] {
            bool confirmed = false;
            Component yes_button = Button("Yes", [&] {
                confirmed = true;
                screen.ExitLoopClosure()();
            }) | center;
            filler();
            Component no_button = Button("No ", screen.ExitLoopClosure());
            auto popup_component = Renderer(Container::Vertical({yes_button, no_button}), [&] {
                return vbox({
                           text("Are you sure?") | center,
                           separator(),
                           hbox({yes_button->Render(), no_button->Render()}) | center | border
                       }) | center;
            });
            screen.Loop(popup_component);
            if (confirmed && index < books_db.size()) {
                books_db.erase(index);
                row_components.erase(row_components.begin() + index);

                activeScreen = ListBooksScreen;
                screen.ExitLoopClosure()();
            }
        });

        auto row_renderer = Renderer(Container::Horizontal({edit_button, delete_button}),
                                     [&, index, edit_button, delete_button] {
                                         return hbox({
                                             hbox(
                                                 text(std::to_string(books_db[index].id)) | center |
                                                 size(WIDTH, EQUAL, 3),
                                                 text(books_db[index].ISBN) | center |
                                                 size(WIDTH, EQUAL, 10),
                                                 text(books_db[index].title) | center |
                                                 size(WIDTH, EQUAL, 40) | color(Color::YellowLight),
                                                 text(books_db[index].get_author_name()) | center |
                                                 size(WIDTH, EQUAL, 20) | color(Color::Blue),
                                                 text(books_db[index].get_price_str()) | center |
                                                 size(WIDTH, EQUAL, 10),
                                                 text(std::to_string(books_db[index].copies_sold)) | center |
                                                 size(WIDTH, EQUAL, 10),
                                                 text(books_db[index].get_total_revenue_str()) | center |
                                                 size(WIDTH, EQUAL, 10) | color(Color::GreenLight)
                                             ),
                                             filler(), edit_button->Render(), delete_button->Render()
                                         });
                                     });

        return row_renderer;
    };

    for (unsigned i = 0; i < books_db.size(); ++i)
        row_components.push_back(makeRow(i));

    auto rows_container = Container::Vertical(row_components);

    Component back_button = Button("   Exit   ", [&]() {
        activeScreen = MenuScreen;
        screen.ExitLoopClosure()();
    });

    auto root = Container::Vertical({
        filter_ISBN_input,
        filter_title_input,
        filter_author_id_input,
        range_from_input,
        range_to_input,
        back_button,
        rows_container
    });
    auto renderer = Renderer(root, [&] {
        Elements rows;

        rows.push_back(text("=== Main Menu ===") | bold | center);
        rows.push_back(separator());

        rows.push_back(hbox(
                           {
                               text("   "),
                               vbox(
                                   {
                                       text("ISBN"),
                                       filter_ISBN_input->Render() | bgcolor(Color::GrayDark),
                                       text("Title"),
                                       filter_title_input->Render() | bgcolor(Color::GrayDark)
                                   }) | flex,
                               text("   "),
                               vbox(
                                   {
                                       text("Author"),
                                       filter_author_id_input->Render() | color(Color::GrayDark)
                                   }) | flex,
                               text("   "),
                               vbox(
                                   {
                                       text("From: "), range_from_input->Render() | bgcolor(Color::GrayDark),
                                       text(" To: "), range_to_input->Render() | bgcolor(Color::GrayDark)
                                   }) | size(WIDTH, EQUAL, 8),
                               text("   "),
                               vbox(
                                   {
                                       text("All Sales: "),
                                       text(std::to_string(all_sells)) | bgcolor(Color::DarkGreen),
                                       text("Avr price: "),
                                       text([&] {
                                           std::ostringstream oss;
                                           if (number_of_books_after_filter > 0) {
                                               double avg =
                                                       static_cast<double>(all_price) / number_of_books_after_filter /
                                                       100.0;
                                               oss << std::fixed << std::setprecision(2) << avg;
                                           } else {
                                               oss << "0.00";
                                           }
                                           return oss.str();
                                       }()) | bgcolor(Color::DarkGreen),
                                   }) | size(WIDTH, EQUAL, 15),

                               text("      "),
                               dbox({filler(), back_button->Render() | center, filler()}),
                               text("      "),
                           }) | size(HEIGHT, GREATER_THAN, 5));
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

        rows_container->DetachAllChildren();
        all_sells = 0;
        all_price = 0;
        number_of_books_after_filter = 0;
        for (size_t i = 0; i < books_db.size(); i++) {
            const Book &book = books_db[i];
            if (!filter_ISBN_str.empty() || !filter_title_str.empty() || filter_author_id != author_stop_id) {
                bool is_match = false;
                if (!filter_ISBN_str.empty()) {
                    is_match = (book.ISBN.find(filter_ISBN_str) != std::string::npos) || is_match;
                }
                if (!filter_title_str.empty()) {
                    is_match = (book.title.find(filter_title_str) != std::string::npos) || is_match;
                }
                if (filter_author_id != author_stop_id) {
                    is_match = (book.author_id == filter_author_id) || is_match;
                }
                if (!is_match) continue;
            }

            if (!range_from_str.empty() && is_number(range_from_str)) {
                if (const size_t from_idx = std::stoul(range_from_str); i < from_idx) continue;
            }
            if (!range_to_str.empty() && is_number(range_to_str)) {
                if (const unsigned to_idx = std::stoul(range_to_str); i > to_idx) continue;
            }
            all_sells += books_db[i].copies_sold;
            all_price += books_db[i].price;
            number_of_books_after_filter++;
            rows_container->Add(row_components[i]);
        }

        rows.push_back(

            vscroll_indicator(frame(rows_container->Render() | flex))
        );

        rows.push_back(separator());
        rows.push_back(separator());
        return vbox(rows);
    });
    screen.Loop(renderer);
}

void App::show_add_book_screen() {
    std::string ISBN;
    std::string title;
    std::string price_str;
    std::string copies_sold_str;

    int selected_author = 0;
    const std::vector<std::string> authors_strings(authors.begin(), authors.end());

    Component ISBN_input = Input(&ISBN);
    Component title_input = Input(&title);
    Component price_input = Input(&price_str);
    Component copies_input = Input(&copies_sold_str);
    Component author_dropdown = Dropdown(&authors_strings, &selected_author);

    Component add_button = Button("Add", [&] {
        if (!ISBN.empty() && !title.empty() && is_number(price_str) && is_number(copies_sold_str)) {
            const int parsed_price = std::stoi(price_str);
            const int parsed_copies_sold = std::stoi(copies_sold_str);
            const Book book(ISBN, title, selected_author, parsed_price, parsed_copies_sold);
            books_db.pushBack(book);
            show_pop_up("Book added!");
        } else {
            show_pop_up("Invalid input!");
        }
        activeScreen = MenuScreen;
        screen.ExitLoopClosure()();
    });

    Component cancel_button = Button("Exit", [&] {
        activeScreen = MenuScreen;
        screen.ExitLoopClosure()();
    });

    const Component form = Container::Vertical({
        ISBN_input,
        title_input,
        price_input,
        copies_input,
        author_dropdown,
        add_button,
        cancel_button
    });

    const auto renderer = Renderer(form, [&] {
        return vbox({
                   text("=== Add New Book ===") | bold | center,
                   separator(),
                   window(text(" Book Form "), vbox({
                              hbox({text("ISBN          : "), ISBN_input->Render()}),
                              hbox({text("Title         : "), title_input->Render()}),
                              hbox({text("Price [gr]    : "), price_input->Render()}),
                              hbox({text("Copies Sold   : "), copies_input->Render()}),
                              hbox({text("Select Author : "), author_dropdown->Render()}),
                          })),
                   separator(),
                   hbox({filler(), add_button->Render(), text("   "), cancel_button->Render(), filler()})
               }) | center;
    });

    screen.Loop(renderer);
}

void App::show_save_book_screen() {
    save_books_to_bin();
    show_pop_up("Saved");
    activeScreen = MenuScreen;
};

void App::show_lode_book_screen() {
    load_books_from_bin();
    show_pop_up("Loaded");
    activeScreen = MenuScreen;
};

void App::show_best_sealers_screen() {
    std::vector<std::pair<size_t, unsigned int> > index_revenue;
    index_revenue.reserve(books_db.size());

    for (size_t i = 0; i < books_db.size(); ++i) {
        index_revenue.emplace_back(i, books_db[i].get_total_revenue());
    }

    std::ranges::sort(index_revenue,
                      [](const auto &a, const auto &b) {
                          return a.second > b.second;
                      });

    const size_t number_of_best_sellers = std::min<size_t>(5, index_revenue.size());

    std::vector<Component> row_comps;
    row_comps.reserve(number_of_best_sellers);

    for (size_t i = 0; i < number_of_best_sellers; ++i) {
        const Book &b = books_db[index_revenue[i].first];
        const unsigned int revenue = index_revenue[i].second;

        const std::string revenue_str = std::to_string(revenue / 100) + "." +
                                        (revenue % 100 < 10 ? "0" : "") +
                                        std::to_string(revenue % 100) + " PLN";

        const std::string author = b.get_author_name();
        const std::string position = "#" + std::to_string(i + 1);

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

        row_comps.push_back(row);
    }

    auto rows_container = Container::Vertical(row_comps);

    Component back_button = Button("   Back   ", [&]() {
        activeScreen = MenuScreen;
        screen.ExitLoopClosure()();
    });

    const auto root = Container::Vertical({rows_container, back_button});

    const auto renderer = Renderer(root, [&] {
        Elements rows;

        rows.push_back(text("=== Top 5 Best Sellers ===") | bold | center);
        rows.push_back(separator());

        rows.push_back(window(text(" Bestseller Ranking "), vbox({
                                  separator(),
                                  rows_container->Render(),
                                  separator()
                              })));

        rows.push_back(separator());
        rows.push_back(hbox({filler(), back_button->Render(), filler()}));

        return vbox(rows) | center;
    });

    screen.Loop(renderer);
}

void App::show_books_statistics_screen() {
    const size_t book_count = books_db.size();
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

    auto format_money = [](const unsigned int grosze) {
        const unsigned int zl = grosze / 100;
        const unsigned int gr = grosze % 100;
        return std::to_string(zl) + "." + (gr < 10 ? "0" : "") + std::to_string(gr) + " PLN";
    };

    const std::string avg_price_str = (book_count > 0) ? format_money(total_price / book_count) : "0.00 PLN";


    auto make_row = [](const std::string &label, const std::string &value, const Color value_color = Color::White) {
        return Renderer([=] {
            return hbox({
                text(label + ": ") | bold | color(Color::YellowLight),
                text(value) | bold | color(value_color),
                filler()
            });
        });
    };

    std::vector<Component> row_comps;
    row_comps.push_back(make_row("Number of Books", std::to_string(book_count)));
    row_comps.push_back(make_row("Number of Books Sold", std::to_string(total_copies_sold)));
    row_comps.push_back(make_row("Total Revenue", format_money(total_revenue), Color::GreenLight));
    row_comps.push_back(make_row("Average Book Price", avg_price_str, Color::GreenLight));
    row_comps.push_back(make_row("Most Published Author", most_published_author, Color::Blue));

    auto rows_container = Container::Vertical(row_comps);

    Component back_button = Button("   Back   ", [&]() {
        activeScreen = MenuScreen;
        screen.ExitLoopClosure()();
    });

    const auto root = Container::Vertical({rows_container, back_button});

    const auto renderer = Renderer(root, [&] {
        Elements rows;

        rows.push_back(text("=== Book Statistics ===") | bold | center);
        rows.push_back(separator());

        rows.push_back(
            window(text(" Statistics Summary "), vbox({
                       separator(),
                       rows_container->Render(),
                       separator()
                   }))
        );

        rows.push_back(separator());
        rows.push_back(hbox({filler(), back_button->Render(), filler()}));

        return vbox(rows) | center;
    });

    screen.Loop(renderer);
}

void App::run() {
    while (activeScreen != Exit) {
        switch (activeScreen) {
            case AddBookScreen: show_add_book_screen();
                break;
            case MenuScreen: show_menu();
                break;
            case ListBooksScreen: showListBooksScreen();
                break;
            case SaveBookScreen: show_save_book_screen();
                break;
            case LodeBookScreen: show_lode_book_screen();
                break;
            case BestSealers: show_best_sealers_screen();
                break;
            case BooksStatistics: show_books_statistics_screen();
                break;
            default: activeScreen = MenuScreen;
                break;
        }
    }
}
