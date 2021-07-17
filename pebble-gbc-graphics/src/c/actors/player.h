#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"
#include "../util.h"

#define MIN_PLAYER_Y 8
#define MAX_PLAYER_Y 208
#define MAX_PLAYER_Y_SPEED 3

#define PLAYER_SPRITE_PALETTE 0
#define PLANE_RUDDER 0
#define PLANE_TAIL 2
#define PLANE_BODY 4
#define PLANE_BODY_UP 6
#define PLANE_BODY_DOWN 8
#define PLANE_NOSE_0 10
#define PLANE_NOSE_1 12
#define PLANE_NOSE_2 14
#define PLANE_NOSE_3 16

void player_init(GBC_Graphics *graphics);
void player_step(GBC_Graphics *graphics);
uint get_player_x();
uint8_t get_player_y();
void player_set_vertical_direction(Direction new_scroll_dir);