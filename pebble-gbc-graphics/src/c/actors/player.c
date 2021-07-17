#include "player.h"
#include "../util.h"

static uint32_t s_player_x;
static uint8_t s_player_y;
static Direction s_player_y_direction;
static uint8_t s_player_speed;

extern uint8_t base_vram_offset, cloud_vram_offset, fuel_vram_offset, road_vram_offset, sprites_vram_offset, text_vram_offset;

static const int8_t s_dir_to_point[] = {
  // x, y
  0, 0, // D_NONE
  0, -1, // D_UP
  -1, 0, // D_LEFT
  0, 1, // D_DOWN
  1, 0, // D_RIGHT
  0, 0, // D_END
};

void player_init(GBC_Graphics *graphics) {
    s_player_x = 0;
    s_player_y = 104;
    s_player_speed = 2;
}

void player_step(GBC_Graphics *graphics) {
    s_player_x += s_player_speed;
    s_player_y = clamp_uint8_t(MIN_PLAYER_Y, s_player_y + s_dir_to_point[s_player_y_direction * 2 + 1] * s_player_speed, MAX_PLAYER_Y);
}

uint32_t get_player_x() {
    return s_player_x;
}

uint8_t get_player_y() {
    return s_player_y;
}

void player_set_vertical_direction(Direction new_scroll_dir) {
  s_player_y_direction = new_scroll_dir;
}