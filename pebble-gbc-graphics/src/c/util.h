#pragma once

#include <pebble.h>
#include "pebble-gbc-graphics/pebble-gbc-graphics.h"

typedef enum {
    D_NONE,
    D_UP,
    D_LEFT,
    D_DOWN,
    D_RIGHT,
    D_END
} Direction;

int8_t *dir_to_point(Direction dir);
int clamp_int(int min, int val, int max);
uint8_t clamp_uint8_t(uint8_t min, uint8_t val, uint8_t max);
uint8_t clamp_int8_t(int8_t min, int8_t val, int8_t max);
void load_tilesheets(GBC_Graphics *graphics);