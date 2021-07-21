#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

#define FUEL_BAR_FULL 0    // The tile for a full fuel bar
#define FUEL_BAR_EMPTY 8   // The tile for an empty fuel bar
#define FUEL_BAR_END 9     // The tile for the end of the fuel bar
#define FUEL_BAR_PALETTE 5 // The palette for the fuel bar, shared with FUEL_TEXT_PALETTE

/**
 * Draws a fuel bar with num_fuel_bar sections, filled up to fuel_amount/max_fuel
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param fuel_amount The amount of fuel that should be drawn
 * @param max_fuel The amount of fuel at the end of the bar
 * @param num_fuel_bars The number of sections the fuel bar should have
 * @param x The x position of the fuel bar, in tiles
 * @param y The y position of the fuel bar, in tiles
 */
void draw_fuel_bar(GBC_Graphics *graphics, uint16_t fuel_amount, uint16_t max_fuel, uint8_t num_fuel_bars, uint8_t x, uint8_t y);