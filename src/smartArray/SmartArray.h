#pragma once
#include <iostream>

#define STARTING_SIZE 2

/**
 * @brief A dynamic array template with automatic resizing.
 *
 * SmartArray is a custom implementation of a dynamic array similar to std::vector.
 * It supports copy/move semantics, insertion, removal, front/back operations,
 * range-checked access, and memory reservation.
 *
 * @tparam T Type of the stored elements.
 */
template<typename T>
class SmartArray;

template<typename T>
std::ostream &operator<<(std::ostream &lhs, const SmartArray<T> &rhs) {
    lhs << "[ SmartArray\n";
    for (size_t i = 0; i < rhs.size(); i++) {
        lhs << "\t" << i << " : " << rhs[i] << "\n";
    }
    lhs << "]\n";
    return lhs;
}


template<typename T>
class SmartArray {
    T *v_data_block = nullptr;
    size_t v_size = 0;
    size_t v_capacity = 0;

    /**
     * @brief Allocates a new memory block with given capacity and moves/copies elements.
     * @param new_capacity New memory capacity.
     * @throws std::bad_alloc If memory allocation fails.
     * @throws Any exception thrown by the move constructor of T.
     */
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

    /**
     * @brief Allocates a new memory block with given capacity and moves/copies elements.
     * @param new_capacity New memory capacity.
     * @param default_element Default element to insert.
     * @throws std::bad_alloc If memory allocation fails.
     * @throws Any exception thrown by the move constructor of T.
     */
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

    /**
     * @brief Grows the internal capacity if needed.
     *
     * If adding one more element would exceed the current capacity,
     * this function increases the capacity by 50% using reallocation.
     *
     * @note This function does not throw directly, but it may call re_alloc,
     *       which can throw std::bad_alloc or exceptions from T's move constructor.
     */
    void try_grow() {
        const size_t grow_factor = 1 + v_capacity / 2;
        if (v_size + 1 > v_capacity) re_alloc(v_capacity + grow_factor);
    }


    /**
     * @brief Checks whether the index is within the bounds of the array.
     *
     * Used for safe element access. Throws an exception if the index is invalid.
     *
     * @param index Index to check (must be less than size).
     * @throws std::out_of_range If the index is out of bounds.
     */
    void range_check(const size_t index) const {
        if (index >= v_size)
            throw std::out_of_range(
                "Index " + std::to_string(index) + " is more then  max_index for size " + std::to_string(v_size));
    }

public:
    /** @brief Default constructor. Initializes the array with a starting capacity. */
    SmartArray() {
        re_alloc(STARTING_SIZE);
    }

    /** @brief Constructs an empty array with a given capacity.
     *  @param capacity New memory capacity.
     */
    explicit SmartArray(const size_t capacity) {
        re_alloc(capacity);
    }

    /** @brief Constructs an array with a given size and fills with provided element.
     *  @param size Number of elements to include.
     *  @param elem Default Element
     */
    explicit SmartArray(const size_t size, const T &elem) {
        re_alloc_with_default(size, elem);
    }

    /** @brief Copy constructor. */
    SmartArray(const SmartArray &other) {
        re_alloc(other.v_capacity);
        for (size_t i = 0; i < other.v_size; ++i) {
            new(&v_data_block[i]) T(other.v_data_block[i]);
        }
        v_size = other.v_size;
    }

    /** @brief Copy assignment operator. */
    SmartArray &operator=(const SmartArray &other) {
        if (this != &other) {
            clear();
            if (v_capacity < other.v_size) {
                re_alloc(other.v_capacity);
            }
            for (size_t i = 0; i < other.v_size; ++i) {
                new(&v_data_block[i]) T(other.v_data_block[i]);
            }
            v_size = other.v_size;
        }
        return *this;
    }

    /** @brief Move constructor. */
    SmartArray(SmartArray &&other) noexcept
        : v_data_block(other.v_data_block), v_size(other.v_size), v_capacity(other.v_capacity) {
        other.v_data_block = nullptr;
        other.v_size = 0;
        other.v_capacity = 0;
    }

    /** @brief Move assignment operator. */
    SmartArray &operator=(SmartArray &&other) noexcept {
        if (this != &other) {
            clear();
            ::operator delete(v_data_block, v_capacity * sizeof(T));

            v_data_block = other.v_data_block;
            v_size = other.v_size;
            v_capacity = other.v_capacity;

            other.v_data_block = nullptr;
            other.v_size = 0;
            other.v_capacity = 0;
        }
        return *this;
    }

    /** @brief Destructor. */
    ~SmartArray() {
        clear();
        ::operator delete(v_data_block, v_capacity * sizeof(T));
    }

    /** @brief Access element without bounds checking. */
    T &operator[](size_t index) {
        return v_data_block[index];
    }

    /** @brief Const access without bounds checking. */
    const T &operator[](size_t index) const {
        return v_data_block[index];
    }

    /** @brief Range-checked access. Throws if out of range. */
    T &at(const size_t index) {
        range_check(index);
        return v_data_block[index];
    }

    /** @brief Const version of range-checked access. */
    const T &at(const size_t index) const {
        range_check(index);
        return v_data_block[index];
    }

    /** @brief Returns the number of stored elements. */
    [[nodiscard]] size_t size() const {
        return v_size;
    }

    /** @brief Returns the capacity of the array. */
    [[nodiscard]] size_t capacity() const {
        return v_capacity;
    }

    /** @brief Checks whether the array is empty. */
    [[nodiscard]] bool isEmpty() const {
        return size() == 0;
    }

    /** @brief Checks if two arrays are equal in size and content. */
    bool operator==(const SmartArray &other) const {
        if (v_size != other.v_size) return false;

        for (size_t i = 0; i < v_size; ++i) {
            if (!(v_data_block[i] == other.v_data_block[i])) {
                return false;
            }
        }
        return true;
    }

    /** @brief Removes all elements but keeps allocated memory. */
    void clear() {
        for (size_t i = 0; i < v_size; i++) {
            v_data_block[i].~T();
        }
        v_size = 0;
    }

    /** @brief Adds an element to the end (copied or moved). */
    void pushBack(const T elem) {
        try_grow();
        new(&v_data_block[v_size]) T(std::move(elem));
        v_size++;
    }

    /** @brief Removes the last element if array is not empty. */
    void popBack() {
        if (v_size > 0) {
            v_data_block[v_size - 1].~T();
            v_size--;
        }
    }

    /** @brief Adds an element at the front of the array. This shifts all the elements */
    void pushFront(const T &elem) {
        insert(0, elem);
    }

    /** @brief Removes the first element of the array. This shifts all the elements*/
    void popFront() {
        if (v_size > 0) erase(0);
    }

    /** @brief Inserts an element at the specified index.
     *  @param index Index at which to insert.
     *  @param elem Element to insert.
     *  @throws std::out_of_range
     */
    void insert(size_t index, const T &elem) {
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

    /** @brief Removes the element at the specified index.
     *  @throws std::out_of_range
     */
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

    /**
     * @brief Removes multiple elements starting from a given index.
     * @param index Erase start index (inclusive).
     * @param amount Number of elements to remove.
     * @throws std::out_of_range If the erase range exceeds the array size.
     */
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

    /** @brief Reserves capacity for at least the specified number of elements. */
    void reserve(const size_t size) {
        if (size < v_size) return;
        re_alloc(size);
    }

    /** @brief Resizes the array. */
    void resize(const size_t size) {
        re_alloc(size);
    }

    /** @brief Resizes the array and fills new elements with the given value. */
    void resize(const size_t size, const T &elem) {
        re_alloc_with_default(size, elem);
    }

    /** @brief Shrinks the allocated memory to fit the current number of elements. */
    void shrinkToFit() {
        re_alloc(v_size);
    }

    /**
     * @brief Sanity check function for testing purposes.
     *
     * This function is intended for unit tests or compilation checks.
     * @return Always returns false.
     */
    static bool testGTest() {
        return false;
    }

    /**
     * @brief Outputs the array contents to the given stream.
     * @param lhs Output stream.
     * @param rhs SmartArray to print.
     * @return Modified output stream.
     */
    friend std::ostream &operator<<<>(std::ostream &lhs, const SmartArray<T> &rhs);
};
