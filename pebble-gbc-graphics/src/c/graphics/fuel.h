#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

#define FUEL_BAR_PALETTE 5 // Shared with FUEL_TEXT_PALETTE

#define FUEL_BAR_FULL 0
#define FUEL_BAR_EMPTY 8
#define FUEL_BAR_END 9

void draw_fuel_bar(GBC_Graphics *graphics, uint16_t fuel_amount, uint16_t max_fuel, uint8_t num_fuel_bars, uint8_t x, uint8_t y);