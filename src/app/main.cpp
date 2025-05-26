
// #include "ftxui/component/screen_interactive.hpp"
// #include "../smartArray/SmartArray.h"
// #include "../book/book.h"
// #include "ftxui/component/component.hpp"
//
// using namespace ftxui;
// class App2 {
//     SmartArray<Book> books_database;
//     ScreenInteractive screen;
//
//     Component main_renderer;
//     Component tab_content;
//     Component tab_selection;
//     Component exit_button;
//     Component main_loop_component;
//
//     int tab_index = 0;
//
//
//     std::string ISBN_string;
//     std::string title_string;
//     std::string price_string;
//     std::string copies_sold_string;
//
//     const std::vector<std::string> entries = {
//         "All Records",
//         "Add Book",
//         "Edit Book",
//         "Count Books",
//         "Save",
//         "Load",
//     };
//
//     void rebuild_ui() {
//         tab_selection = Menu(&entries, &tab_index, MenuOption::HorizontalAnimated());
//
//         tab_content = Container::Tab({
//                                          make_all_records(),
//                                          make_add_book(),
//                                          make_edit_book(),
//                                          make_count_book(),
//                                          make_save_book(),
//                                          make_load_book(),
//                                      }, &tab_index);
//
//         exit_button = Button("Exit", [&] {
//             screen.Exit();
//         }, ButtonOption::Animated());
//
//         auto main_container = Container::Vertical({
//             Container::Horizontal({
//                 tab_selection,
//                 exit_button,
//             }),
//             tab_content,
//         });
//
//         main_renderer = Renderer(main_container, [&] {
//             return vbox({
//                 text("FTXUI Demo") | bold | hcenter,
//                 hbox({
//                     tab_selection->Render() | flex,
//                     exit_button->Render(),
//                 }),
//                 tab_content->Render() | flex,
//             });
//         });
//     }
//
     // Component make_all_records() {
     //     std::vector<Component> book_rows;
     //     auto header = Container::Horizontal({
     //                       Renderer([] { return text("ID") | center | bold | size(WIDTH, EQUAL, 3); }),
     //                       Renderer([] { return text("ISBN") | center | bold | size(WIDTH, EQUAL, 15); }),
     //                       Renderer([] { return text("Title") | center | bold | size(WIDTH, EQUAL, 50); }),
     //                       Renderer([] { return text("Author") | center | bold | size(WIDTH, EQUAL, 20); }),
     //                       Renderer([] { return text("Price") | center | bold | size(WIDTH, EQUAL, 15); }),
     //                       Renderer([] { return text("Copies Sold") | center | bold | size(WIDTH, EQUAL, 15); }),
     //                       Renderer([] { return text("Revenue") | center | bold | size(WIDTH, EQUAL, 15); }),
     //                       Renderer([] { return text("Interaction") | center | bold | size(WIDTH, EQUAL, 20); }),
     //                   }) | border;
     //     book_rows.push_back(header);
//
//         if (books_database.size() == 0) {
//             book_rows.push_back(Renderer([&] {
//                 return text("Sorry, you have no books.") | flex | center;
//             }));
//         }
//
//         for (size_t i = 0; i < books_database.size(); ++i) {
//             int index = static_cast<int>(i); // kopia indeksu
//             book_rows.push_back(
//                 Container::Horizontal({
//                     Renderer([&, index] {
//                         return text(std::to_string(books_database[index].id)) | center | size(WIDTH, EQUAL, 3);
//                     }),
//                     Renderer([&, index] {
//                         return text(books_database[index].ISBN) | center | size(WIDTH, EQUAL, 15);
//                     }),
//                     Renderer([&, index] {
//                         return text(books_database[index].title) | center | size(WIDTH, EQUAL, 50);
//                     }),
//                     Renderer([&, index] {
//                         return text(books_database[index].get_author_name()) | center | size(WIDTH, EQUAL, 20);
//                     }),
//                     Renderer([&, index] {
//                         return text(books_database[index].get_price_str()) | center | size(WIDTH, EQUAL, 15);
//                     }),
//                     Renderer([&, index] {
//                         return text(std::to_string(books_database[index].copies_sold)) | center |
//                                size(WIDTH, EQUAL, 15);
//                     }),
//                     Renderer([&, index] {
//                         return text(books_database[index].get_total_revenue_str()) | center | size(WIDTH, EQUAL, 15);
//                     }),
//                     Button("Edit", [this, index] {
//                         //auto editor = make_edit_form(index);
//                         //screen.Loop(editor); // Uruchamia oddzielny formularz edycji
//                     }) | size(WIDTH, EQUAL, 10),
//                     Button("Delete", [this, index] {
//                         if (index < books_database.size()) {
//                             books_database.erase(index);
//                             screen.PostEvent(Event::Custom); // sygnalizacja zmiany danych
//                         }
//                     }) | size(WIDTH, EQUAL, 10),
//                 }) | border
//             );
//         }
//
//         return Container::Vertical({
//             Container::Vertical(book_rows) | vscroll_indicator | yframe,
//         });
//     }
//
//     Component make_add_book() {
//         // Komponenty wejściowe
//         Component input_ISBN_string = Input(&ISBN_string, "000-000-000-000");
//         Component input_title_string = Input(&title_string, "Book Title");
//         Component input_price_string = Input(&price_string, "Price in Grosze");
//         Component input_copies_sold_string = Input(&copies_sold_string, "Copies Sold");
//
//         // Kontener obsługujący zdarzenia i fokus
//         auto container = Container::Horizontal({
//             input_ISBN_string,
//             input_title_string,
//             input_price_string,
//             input_copies_sold_string,
//             Button("Add", [&] {
//                 books_database.pushBack(Book(ISBN_string, title_string, 1,100,100));
//                 //screen.PostEvent(Event::Custom);
//             })
//         });
//
//         // Renderujemy cały kontener — nie pojedyncze komponenty!
//         return Renderer(container, [=] {
//             return container->Render() | border | flex;
//         });
//     }
//
//     Component make_edit_book() {
//         Component hello_component = Renderer([] {
//             return text("Edit BOOK") | border;
//         });
//         return hello_component;
//     }
//
//     Component make_count_book() {
//         Component hello_component = Renderer([] {
//             return text("Count BOOK") | border;
//         });
//         return hello_component;
//     }
//
//     Component make_save_book() {
//         Component hello_component = Renderer([] {
//             return text("Save BOOK") | border;
//         });
//         return hello_component;
//     }
//
//     Component make_load_book() {
//         Component hello_component = Renderer([] {
//             return text("Load BOOK") | border;
//         });
//         return hello_component;
//     }
//
// public:
//     App2()
//         : books_database(2),
//           screen(ScreenInteractive::Fullscreen()) {
//         books_database.pushBack(Book("345456sdf4", "Ale Fajna Książka", 1, 50 * 100, 33));
//         books_database.pushBack(Book("3454564", "Ale Fajna Książkddddddddddddda", 1, 50 * 100, 33));
//         books_database.pushBack(Book("344", "Ale Fajna Książka", 1, 50 * 100, 33));
//
//         ISBN_string = "000-000-000-000";
//         title_string = "Book Title";
//         price_string = "0";
//         copies_sold_string = "0";
//     }
//
//     void run() {
//         rebuild_ui();
//
//         main_loop_component = CatchEvent(main_renderer, [&](const Event &event) {
//             if (event == Event::Custom) {
//                 rebuild_ui();
//                 return true;
//             }
//             return false;
//         });
//
//         screen.Loop(main_loop_component);
//     }
// };


#include "App.h"

int main() {
    App app;
    app.run();
}
