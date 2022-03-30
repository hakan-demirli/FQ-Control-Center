#include "utility.h"
#include "memory_map.h"


void entry_point () __attribute__((section(".entry_point")));
int sf (int b);

void entry_point () {
    GPIO_1 = 0b010000;
    passed();
    GPIO_1 = 0b010000;
}

int sf (int b) {
    return b+77;
}