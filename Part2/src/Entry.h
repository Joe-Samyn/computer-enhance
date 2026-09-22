#pragma once

#include <stdint.h>
struct Entry {
    int32_t x0;
    int32_t y0;
    int32_t x1;
    int32_t y1;
};

struct Pairs {
    Entry* entries;
};
