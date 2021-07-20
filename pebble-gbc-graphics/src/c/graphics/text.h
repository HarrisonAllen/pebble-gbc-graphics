#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

#define BLANK_TILE 1 // The tile for a blank space
#define BALLOON_TILE 18 // The tile for the balloon icon
#define FUEL_TILE 23 // The tile for the fuel icon
#define BALLOON_TEXT_PALETTE 4 // The palette for the balloon icon 
#define FUEL_TEXT_PALETTE 5 // The palette for the fuel icon, shared with FUEL_BAR_PALETTE

#define NUMBER_OFFSET 26 // The offset on the text tilesheet where the numbers start
#define COLON_OFFSET 36 // The offset on the text tilesheet for the colon
#define TIMES_OFFSET 37 // The offset on the text tilesheet for the times symbol

/**
 * Clears the top row of the background tilemap to prep for the score bar
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void clear_top_row(GBC_Graphics *graphics);

/**
 * Initializes the text palettes and clears the top row
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void text_init(GBC_Graphics *graphics);

/**
 * Draws text at a location with the given palette
 * @note Supported characters include:
 *  'A'->'Z' (capital letters)
 *  '0'->'9' (numbers)
 *  'x'      (times symbol)
 *  ':'      (colon)
 *  'b'      (balloon icon)
 *  'f'      (fuel icon)
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param text A pointer to the character array for drawing text
 * @param x The x position in tiles to draw the text
 * @param y The y position in tiles to draw the text
 * @param palette_num The palette number to use for the tiles
 * @param background Should the text be drawn on the background? Draws on window when false
 */
void draw_text_at_location(GBC_Graphics *graphics, char *text, uint8_t x, uint8_t y, uint8_t palette_num, bool background);