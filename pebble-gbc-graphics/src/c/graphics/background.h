#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

#define SOLID_TILE_00 0
#define SOLID_TILE_01 1
#define SOLID_TILE_10 2
#define SOLID_TILE_11 3
#define WINDOW_FRAME_CORNER 4
#define WINDOW_FRAME_HORIZONTAL 5
#define WINDOW_FRAME_VERTICAL 6

void generate_new_game_background(GBC_Graphics *graphics);
void animate_graphics(GBC_Graphics *graphics);
void start_bg_animation();
void start_window_animation();
void change_bg_scroll();
void render_background(GBC_Graphics *graphics, uint player_x, uint8_t player_y);