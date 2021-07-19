#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"
#include "../util.h"

#define MIN_PLAYER_Y 8
#define MAX_PLAYER_Y 208
#define MAX_PLAYER_Y_SPEED 6
#define PLAYER_ON_SCREEN_X PBL_IF_ROUND_ELSE(32, 16)
#define PLAYER_OFF_SCREEN_X -32
#define PLAYER_OFF_SCREEN_Y GBC_Graphics_get_screen_height(graphics)

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

typedef enum {
    PS_ON_SCREEN,
    PS_OFF_SCREEN,
    PS_MOVING_OFF_LEFT,
    PS_MOVING_ON_RIGHT,
    PS_MOVING_OFF_DOWN
} PlayerState;

void player_init(GBC_Graphics *graphics);
void player_step(GBC_Graphics *graphics);
uint get_player_x();
uint8_t get_player_y();
uint8_t get_player_screen_x();
uint8_t get_player_screen_y(GBC_Graphics *graphics);
GRect get_player_collision();
void player_set_vertical_direction(Direction new_scroll_dir);
void player_move_off_screen_left();
void player_move_on_screen_right();
void player_move_off_screen_down();
bool player_moving();
bool player_on_screen();