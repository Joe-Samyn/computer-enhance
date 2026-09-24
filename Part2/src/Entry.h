#pragma once

#include <stdint.h>
struct Entry {
    float x0;
    float y0;
    float x1;
    float y1;
};

struct Pairs {
    Entry* entries;
};
