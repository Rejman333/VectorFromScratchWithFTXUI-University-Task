#pragma once
#include <iostream>

#define STARTING_SIZE 2


template<typename T>
class SmartArray;

template<typename T>
std::ostream &operator<<(std::ostream &lhs, const SmartArray<T> &rhs) {
    lhs<<"[ SmartArray\n";
    for (size_t i = 0; i < rhs.size(); i++) {
        lhs <<"\t"<< i << " : " << rhs[i] << "\n";
    }
    lhs<<"]\n";
    return lhs;
}


template<typename T>
class SmartArray {
    T *v_data_block = nullptr;
    size_t v_size = 0;
    size_t v_capacity = 0;

    void re_alloc(const size_t new_capacity) {
        if (new_capacity == v_capacity) return;

        const size_t number_of_elem_to_copy = (new_capacity > v_capacity) ? v_size : new_capacity;
        T *new_data_block = static_cast<T *>(::operator new(new_capacity * sizeof(T)));

        for (size_t i = 0; i < number_of_elem_to_copy; i++) {
            new(&new_data_block[i]) T(std::move(v_data_block[i]));
        }

        clear();
        ::operator delete(v_data_block, v_capacity * sizeof(T));
        v_data_block = new_data_block;
        v_capacity = new_capacity;
        v_size = number_of_elem_to_copy;
    }

    void re_alloc_with_default(const size_t new_capacity, const T &default_element) {
        if (new_capacity == v_capacity) return;

        size_t number_of_elem_to_copy = (new_capacity > v_capacity) ? v_size : new_capacity;
        T *new_data_block = static_cast<T *>(::operator new(new_capacity * sizeof(T)));

        for (size_t i = 0; i < number_of_elem_to_copy; i++) {
            new(&new_data_block[i]) T(std::move(v_data_block[i]));
        }

        for (size_t i = number_of_elem_to_copy; i < new_capacity; i++) {
            new(&new_data_block[i]) T(default_element); //This is copying
        }

        clear();
        ::operator delete(v_data_block, v_capacity * sizeof(T));
        v_data_block = new_data_block;
        v_capacity = new_capacity;
        v_size = new_capacity;
    }

    void try_grow() {
        const size_t grow_factor = 1 + v_capacity / 2;
        if (v_size + 1 > v_capacity) re_alloc(v_capacity + grow_factor);
    }

    void range_check(const size_t index) const {
        if (index >= v_size)
            throw std::out_of_range(
                "Index " + std::to_string(index) + " is more then  max_index for size " + std::to_string(v_size));
    }

public:
    SmartArray() {
        re_alloc(STARTING_SIZE);
    }

    explicit SmartArray(const size_t capacity) {
        re_alloc(capacity);
    }

    explicit SmartArray(const size_t size, const T& elem) {
        re_alloc_with_default(size, elem);
    }

    ~SmartArray() {
        clear();
        ::operator delete(v_data_block, v_capacity * sizeof(T));
    }

    const T &operator[](size_t index) const {
        return v_data_block[index];
    }

    T &operator[](size_t index) {
        return v_data_block[index];
    }

    bool operator==(const SmartArray &other) const {
        if (v_size != other.v_size) return false;

        for (size_t i = 0; i < v_size; ++i) {
            if (!(v_data_block[i] == other.v_data_block[i])) {
                return false;
            }
        }
        return true;
    }

    T &at(const size_t index) {
        range_check(index);
        return v_data_block[index];
    }

    const T &at(const size_t index) const {
        range_check(index);
        return v_data_block[index];
    }

    void reserve(const size_t size) {
        if (size < v_size) return;
        re_alloc(size);
    }

    void resize(const size_t size) {
        re_alloc(size);
    }

    void resize(const size_t size, const T &elem) {
        re_alloc_with_default(size, elem);
    }

    void shrinkToFit() {
        re_alloc(v_size);
    }

    //Żuca błedem bo nie ma gwarancji że istnieje
    void pushFront(const T &elem) {
        insert(0, elem);
    }

    void popFront() {
        if(v_size>0) erase(0);
    }

    void clear() {
        for (size_t i = 0; i < v_size; i++) {
            v_data_block[i].~T();
        }
        v_size = 0;
    }

    //This copes elem
   void insert(size_t index, const T& elem) {
        range_check(index);
        try_grow();

        //This is necessary to init last element
        new(&v_data_block[v_size]) T(std::move(v_size - 1));

        for (size_t i = v_size - 1; i > index; i--) {
            v_data_block[i] = std::move(v_data_block[i - 1]);
        }

        v_data_block[index] = elem;
        v_size++;
    }

    void erase(const size_t index) {
        range_check(index);

        v_data_block[index].~T();

        // Shift only if not erasing the last element
        if (index < v_size - 1) {
            for (size_t i = index; i < v_size - 1; ++i) {
                new(&v_data_block[i]) T(std::move(v_data_block[i + 1]));
                v_data_block[i + 1].~T(); // destroy the moved-from
            }
        }

        v_size--;
    }

    void erase(const size_t index, const size_t amount) {
        const size_t final_index = index + amount;
        if (final_index > v_size) {
            throw std::out_of_range("Erase range out of bounds");
        }

        const size_t num_elements_to_move = v_size - final_index;

        // Move trailing elements forward
        for (size_t i = 0; i < num_elements_to_move; ++i) {
            new(&v_data_block[index + i]) T(std::move(v_data_block[final_index + i]));
            v_data_block[final_index + i].~T(); // destroy moved-from
        }

        // Destroy leftover elements at the end
        for (size_t i = v_size - amount; i < v_size; ++i) {
            v_data_block[i].~T();
        }

        v_size -= amount;
    }

    /*This will use copy constructor if the caller passes lvalue.
     *It will use move constructor if the caller passes rvalue.
     */
    void pushBack(const T elem) {
        try_grow();
        new(&v_data_block[v_size]) T(std::move(elem));
        v_size++;
    }

    void popBack() {
        if (v_size > 0) {
            v_data_block[v_size - 1].~T();
            v_size--;
        }
    }

    //[[nodiscard]] makes it that return value cant be ignored
    [[nodiscard]] size_t size() const {
        return v_size;
    }

    [[nodiscard]] size_t capacity() const {
        return v_capacity;
    }

    [[nodiscard]] bool isEmpty() const {
        return size() == 0;
    }

    static bool testGTest() {
        return false;
    }

    //TODO Zapytać o prostszą implementację
    friend std::ostream &operator<< <>(std::ostream &lhs, const SmartArray<T> &rhs);
};