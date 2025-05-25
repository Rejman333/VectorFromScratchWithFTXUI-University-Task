#include "dragon.h"

std::string toString(const COLOR color) {
    switch (color) {
        case COLOR::RED: return "RED";
        case COLOR::GREEN: return "GREEN";
        case COLOR::BLUE: return "BLUE";
        default: return "UNKNOWN";
    }
}

std::ostream &operator<<(std::ostream &os, const COLOR color) {
    return os << toString(color);
}

std::ostream &operator<<(std::ostream &os, const Dragon &obj) {
    os << "Dragon(" << obj.id << ", " << obj.name << ", " << obj.age << ", "
            << obj.color << ", " << obj.is_flaying << ")\n";
    return os;
}
