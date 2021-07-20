#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

#define SOLID_TILE_00 0
#define SOLID_TILE_01 1
#define SOLID_TILE_10 2
#define SOLID_TILE_11 3

void generate_new_game_background(GBC_Graphics *graphics);
void start_bg_animation();
void change_bg_scroll();
void render_background(GBC_Graphics *graphics, uint32_t player_x, uint8_t player_y);
uint8_t get_bg_scroll_x();
uint8_t get_bg_scroll_y();