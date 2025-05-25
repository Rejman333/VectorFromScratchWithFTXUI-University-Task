#pragma once

#include <iostream>


class Test {
public:
    int x = 0;
    int *data = nullptr;

    Test() = delete;

    explicit Test(int x_value) : x(x_value), data(new int(0)) {
        std::cout << "Konstruktor z x\n";
    }

    explicit Test(int x_value, int data_value) : x(x_value), data(new int(data_value)) {
        std::cout << "Konstruktor z x\n";
    }


    // Destruktor
    ~Test() {
        delete data;
    }

    // Konstruktor kopiujący
    Test(const Test &other) {
        std::cout << "Copy\n";
        if (other.data) data = new int(*other.data);
        x = other.x;
    }

    // Operator kopiujący
    Test &operator=(const Test &other) {
        std::cout << "Copy\n";
        if (this != &other) {
            delete data;
            data = other.data ? new int(*other.data) : nullptr;
            x = other.x;
        }
        return *this;
    }

    // Konstruktor przenoszący
    Test(Test &&other) noexcept {
        std::cout << "Move\n";
        data = other.data;
        x = other.x;
        other.data = nullptr;
    }

    // Operator przenoszący
    Test &operator=(Test &&other) noexcept {
        std::cout << "Move\n";
        if (this != &other) {
            delete data;
            data = other.data;
            x = other.x;
            other.data = nullptr;
        }
        return *this;
    }

    bool operator==(const Test &other) const {
        return x == other.x;
    }
};

inline std::ostream &operator<<(std::ostream &os, const Test &obj) {
    os << "Test(x=" << obj.x << ", data=";
    if (obj.data) {
        os << *obj.data;
    } else {
        os << "nullptr";
    }
    os << ")";
    return os;
}
