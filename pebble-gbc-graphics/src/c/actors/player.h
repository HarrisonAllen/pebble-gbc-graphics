#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"
#include "../util.h"

#define MIN_PLAYER_Y 8
#define MAX_PLAYER_Y 208

void player_init(GBC_Graphics *graphics);
void player_step(GBC_Graphics *graphics);
uint get_player_x();
uint8_t get_player_y();
void player_set_vertical_direction(Direction new_scroll_dir);