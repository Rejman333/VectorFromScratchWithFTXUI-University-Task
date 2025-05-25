    #pragma once
    #include <string>
    #include <utility>
    #include <iostream>

    enum class COLOR {
        RED,
        GREEN,
        BLUE
    };

    std::string toString(COLOR color);

    std::ostream &operator<<(std::ostream &os, COLOR color);

    class Dragon {
    private:
        inline static size_t next_id = 0;
        size_t id;

    public:
        std::string name;
        int age;
        COLOR color;
        bool is_flaying;

        explicit Dragon(std::string name, const int age, const COLOR color, const bool is_flaying)
            : id(next_id++), name(std::move(name)), age(age),
              color(color), is_flaying(is_flaying) {
        }

        Dragon(const Dragon &other) {
            this->id = other.id;
            this->name = other.name;
            this->age = other.age;
            this->color = other.color;
            this->is_flaying = other.is_flaying;
        }

        Dragon &operator=(const Dragon &other) {
            if (this != &other) {
                this->id = other.id;
                this->name = other.name;
                this->age = other.age;
                this->color = other.color;
                this->is_flaying = other.is_flaying;
            }

            return *this;;
        }

        Dragon(Dragon &&other) noexcept {
            this->id = other.id;
            this->name = other.name;
            this->age = other.age;
            this->color = other.color;
            this->is_flaying = other.is_flaying;
        }

        Dragon &operator=(Dragon &&other) noexcept {
            this->id = other.id;
            this->name = other.name;
            this->age = other.age;
            this->color = other.color;
            this->is_flaying = other.is_flaying;
            return *this;
        }

        bool operator==(const Dragon &other) const {
            if (this->id != other.id) return false;
            if (this->name != other.name) return false;
            if (this->color != other.color) return false;
            if (this->is_flaying != other.is_flaying) return false;
            if (this->age != other.age) return false;
            return true;
        }

        friend std::ostream &operator<<(std::ostream &os, const Dragon &obj);
    };

    std::ostream &operator<<(std::ostream &os, const Dragon &obj);
