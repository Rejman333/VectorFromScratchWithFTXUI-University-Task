#include "book.h"
#include <iomanip>

const std::array<std::string, 5> authors = {
    "Brandon Sanderson",
    "Brent Weeks",
    "Joe Abercrombie",
    "J.R.R. Tolkien",
    "Andrzej Sapkowski"
};

Book::Book(std::string ISBN_,
           std::string title_,
           size_t author_id_,
           const unsigned int price_,
           const unsigned int copies_sold_)
    : id(next_id++),
      ISBN(std::move(ISBN_)),
      title(std::move(title_)),
      author_id(author_id_),
      price(price_),
      copies_sold(copies_sold_) {}

Book::Book(const Book &other)
    : id(other.id),
      ISBN(other.ISBN),
      title(other.title),
      author_id(other.author_id),
      price(other.price),
      copies_sold(other.copies_sold) {}

Book::Book(Book &&other) noexcept
    : id(other.id),
      ISBN(std::move(other.ISBN)),
      title(std::move(other.title)),
      author_id(other.author_id),
      price(other.price),
      copies_sold(other.copies_sold) {}

Book &Book::operator=(const Book &other) {
    if (this != &other) {
        id = other.id;
        ISBN = other.ISBN;
        title = other.title;
        author_id = other.author_id;
        price = other.price;
        copies_sold = other.copies_sold;
    }
    return *this;
}

Book &Book::operator=(Book &&other) noexcept {
    if (this != &other) {
        id = other.id;
        ISBN = std::move(other.ISBN);
        title = std::move(other.title);
        author_id = other.author_id;
        price = other.price;
        copies_sold = other.copies_sold;
    }
    return *this;
}

Book::~Book() = default;

bool Book::operator==(const Book &other) const {
    return id == other.id &&
           ISBN == other.ISBN &&
           title == other.title &&
           author_id == other.author_id &&
           price == other.price &&
           copies_sold == other.copies_sold;
}

bool Book::operator!=(const Book &other) const {
    return !(*this == other);
}

std::string Book::get_author_name() const {
    if (author_id < authors.size())
        return authors[author_id];
    return "Unknown Author";
}

unsigned int Book::get_total_revenue() const {
    return price * copies_sold;
}

std::string Book::get_price_str() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2)
        << static_cast<double>(price) / 100.0;
    return oss.str();
}

std::string Book::get_total_revenue_str() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2)
        << static_cast<double>(get_total_revenue()) / 100.0;
    return oss.str();
}

std::string Book::toString() const {
    std::ostringstream oss;
    oss << "ID: " << id << "\n"
        << "Title: " << title << "\n"
        << "Author: " << get_author_name() << "\n"
        << "ISBN: " << ISBN << "\n"
        << "Price: " << get_price_str()<<"\n"
        << "Copies sold: " << copies_sold << "\n"
        << "Total revenue: " << get_total_revenue_str()<<"\n";
    return oss.str();
}

std::ostream& operator<<(std::ostream& os, const Book& book) {
    return os << book.toString();
}
