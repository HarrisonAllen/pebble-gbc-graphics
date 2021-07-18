#include "player.h"

static uint s_player_x;
static uint8_t s_player_y;
static Direction s_player_y_direction;
static uint8_t s_player_x_speed;
static int8_t s_player_y_speed;
static uint8_t s_prop_frame, s_bob_frame;
static uint8_t s_player_screen_x;

extern uint8_t sprites_vram_offset;

static const int8_t s_bob_offsets[] = {
    0,
    -1,
    -1,
    -2,
    -2,
    -2,
    -2,
    -1,
    -1,
    0,
    1,
    1,
    2,
    2,
    2,
    2,
    1,
    1,
};

static const uint8_t s_prop_frames[] = {
    PLANE_NOSE_0,
    PLANE_NOSE_1,
    PLANE_NOSE_2,
    PLANE_NOSE_3,
    PLANE_NOSE_2,
    PLANE_NOSE_1,
};

static void set_player_sprite_palette(GBC_Graphics *graphics) {
    // The first color in a sprite palette is treated as transparency, and won't be visible
#if defined(PBL_COLOR)
    GBC_Graphics_set_sprite_palette(graphics, PLAYER_SPRITE_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorRedARGB8, GColorWhiteARGB8);
#else
    GBC_Graphics_set_sprite_palette(graphics, PLAYER_SPRITE_PALETTE, GBC_WHITE, GBC_BLACK, GBC_GRAY, GBC_WHITE);
#endif
}

static void draw_player_sprites(GBC_Graphics *graphics) {
    uint8_t plane_attrs = GBC_Graphics_attr_make(PLAYER_SPRITE_PALETTE, 0, false, false, false);
    uint8_t player_screen_y = s_player_y - GBC_Graphics_bg_get_scroll_y(graphics) + SPRITE_OFFSET_Y;
    
    player_screen_y += s_bob_offsets[s_bob_frame / 3];

    GBC_Graphics_oam_set_sprite(graphics, 0, s_player_screen_x, player_screen_y, sprites_vram_offset + PLANE_RUDDER, plane_attrs);
    GBC_Graphics_oam_set_sprite(graphics, 1, s_player_screen_x + TILE_WIDTH, player_screen_y, sprites_vram_offset + PLANE_TAIL, plane_attrs);
    uint8_t plane_body_sprite;
    switch (s_player_y_direction) {
        case D_UP:
            plane_body_sprite = PLANE_BODY_UP;
            break;
        case D_DOWN:
            plane_body_sprite = PLANE_BODY_DOWN;
            break;
        default:
            plane_body_sprite = PLANE_BODY;
            break;
    }
    GBC_Graphics_oam_set_sprite(graphics, 2, s_player_screen_x + TILE_WIDTH * 2, player_screen_y, sprites_vram_offset + plane_body_sprite, plane_attrs);
    GBC_Graphics_oam_set_sprite(graphics, 3, s_player_screen_x + TILE_WIDTH * 3, player_screen_y, sprites_vram_offset + s_prop_frames[s_prop_frame], plane_attrs);
    
    GBC_Graphics_render(graphics);
}

void player_init(GBC_Graphics *graphics) {
    s_player_x = 0;
    s_player_y = 80;
    s_player_x_speed = 4;
    s_player_y_speed = 0;
    s_player_screen_x = 16;
    set_player_sprite_palette(graphics);
    draw_player_sprites(graphics);
}

void player_step(GBC_Graphics *graphics) {
    s_player_x += s_player_x_speed;
    if (s_player_y_direction == D_NONE && s_player_y_speed != 0) {
        s_player_y_speed -= 1 * abs(s_player_y_speed) / s_player_y_speed; // just a trick to normalize an integer
    } else {
        s_player_y_speed = clamp_int8_t(-MAX_PLAYER_Y_SPEED, s_player_y_speed + dir_to_point(s_player_y_direction)[1] * 2, MAX_PLAYER_Y_SPEED);
    }
    s_player_y = clamp_uint8_t(MIN_PLAYER_Y, s_player_y + s_player_y_speed, MAX_PLAYER_Y);

    draw_player_sprites(graphics);
    s_prop_frame = (s_prop_frame + 1) % (sizeof(s_prop_frames));
    s_bob_frame = (s_bob_frame + 1) % (sizeof(s_bob_offsets) * 3);
}

uint get_player_x() {
    return s_player_x;
}

uint8_t get_player_y() {
    return s_player_y;
}

uint8_t get_player_screen_x() {
    return s_player_screen_x;
}

uint8_t get_player_screen_y(GBC_Graphics *graphics) {
    uint8_t player_screen_y = s_player_y - GBC_Graphics_bg_get_scroll_y(graphics) + SPRITE_OFFSET_Y;
    player_screen_y += s_bob_offsets[s_bob_frame / 3];
    return player_screen_y;
}

GRect get_player_collision() {
    return GRect(s_player_x + 20, s_player_y + s_bob_offsets[s_bob_frame / 3] + 7, 8, 3);
}

void player_set_vertical_direction(Direction new_scroll_dir) {
  s_player_y_direction = new_scroll_dir;
}