#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

#define BLANK_TILE 1
#define NUMBER_OFFSET 26
#define COLON_OFFSET 36
#define TIMES_OFFSET 37

#define BALLOON_TILE 18
#define FUEL_TILE 23

#define BALLOON_TEXT_PALETTE 4
#define FUEL_TEXT_PALETTE 5

void text_init(GBC_Graphics *graphics);
void draw_text_at_location(GBC_Graphics *graphics, char *text, uint8_t x, uint8_t y, uint8_t palette_num);