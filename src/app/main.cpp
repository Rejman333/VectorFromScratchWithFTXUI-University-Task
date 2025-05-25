#include "../dragon/dragon.h"
#include "../smartArray/SmartArray.h"
#include "ftxui/dom/elements.hpp"


int main() {
    SmartArray<Dragon> dragons_array = SmartArray(2, Dragon("Elo", 333, COLOR::RED, true));
    std::cout<<dragons_array;
    return 0;
}
