#pragma once
#include <string>
#include <array>

extern const std::array<std::string, 5> authors;

class Book {
private:
    inline static size_t next_id = 0;

public:
    size_t id;
    std::string ISBN;
    std::string title;
    size_t author_id;
    unsigned int price; // grosze
    unsigned int copies_sold;

    Book(std::string ISBN_,
         std::string title_,
         size_t author_id_,
         const unsigned int price_,
         const unsigned int copies_sold_ = 0);

    Book(const Book &other);

    Book(Book &&other) noexcept;

    Book &operator=(const Book &other);

    Book &operator=(Book &&other) noexcept;

    bool operator==(const Book &other) const;

    bool operator!=(const Book &other) const;

    ~Book();

    [[nodiscard]] std::string get_author_name() const;

    [[nodiscard]] unsigned int get_total_revenue() const;

    [[nodiscard]] std::string get_price_str() const;

    [[nodiscard]] std::string get_total_revenue_str() const;

    [[nodiscard]] std::string toString() const;

    friend std::ostream& operator<<(std::ostream& os, const Book& book);
};
