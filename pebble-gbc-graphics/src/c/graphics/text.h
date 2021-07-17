#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

#define BLANK_TILE 0
#define NUMBER_OFFSET 26
#define COLON_OFFSET 36

void clear_top_row(GBC_Graphics *graphics);
void draw_text_at_location(GBC_Graphics *graphics, char *text, uint8_t x, uint8_t y, uint8_t palette_num);