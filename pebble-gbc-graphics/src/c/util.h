#pragma once

#include <pebble.h>
#include "pebble-gbc-graphics/pebble-gbc-graphics.h"

uint8_t clamp_uint8_t(uint8_t min, uint8_t val, uint8_t max);
void load_tilesheets(GBC_Graphics *graphics);