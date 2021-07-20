#include "player.h"
#include "../util.h"

static uint s_player_x;
static uint8_t s_player_y;
static Direction s_player_y_direction;
static uint8_t s_player_x_speed;
static int8_t s_player_y_speed;
static uint8_t s_prop_frame, s_bob_frame, s_slide_frame;
static int s_player_screen_x, s_player_screen_y_offset;
static PlayerState s_player_state;
static EngineSpeed s_engine_speed;

extern uint8_t sprites_vram_offset;

extern const uint8_t ease_in_out_percentages[];

extern const uint8_t accelerate_slow_percentages[];

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

static const uint8_t s_prop_fast_frames[] = {
    PLANE_NOSE_0,
    PLANE_NOSE_3,
};

static const uint8_t s_prop_medium_frames[] = {
    PLANE_NOSE_0,
    PLANE_NOSE_2,
    PLANE_NOSE_3,
    PLANE_NOSE_1,
};

static const uint8_t s_prop_slow_frames[] = {
    PLANE_NOSE_0,
    PLANE_NOSE_1,
    PLANE_NOSE_2,
    PLANE_NOSE_3,
    PLANE_NOSE_2,
    PLANE_NOSE_1,
};

static const uint8_t *s_engine_speed_to_prop_frames[] = {
    s_prop_fast_frames,
    s_prop_medium_frames,
    s_prop_slow_frames,
    s_prop_slow_frames,
};

static const uint8_t s_num_prop_frames_per_speed[] = {
    2, 4, 6, 6
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
    uint8_t player_screen_y = clamp_int(0, s_player_y - GBC_Graphics_bg_get_scroll_y(graphics) + SPRITE_OFFSET_Y + s_player_screen_y_offset, UINT8_MAX);
    
    player_screen_y += s_bob_offsets[s_bob_frame / (s_engine_speed + 1)];

    GBC_Graphics_oam_set_sprite(graphics, 0, clamp_int(0, s_player_screen_x, s_player_screen_x), player_screen_y, sprites_vram_offset + PLANE_RUDDER, plane_attrs);

    GBC_Graphics_oam_set_sprite(graphics, 1, clamp_int(0, s_player_screen_x + TILE_WIDTH, s_player_screen_x + TILE_WIDTH), player_screen_y, sprites_vram_offset + PLANE_TAIL, plane_attrs);

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
    GBC_Graphics_oam_set_sprite(graphics, 2, clamp_int(0, s_player_screen_x + TILE_WIDTH * 2, s_player_screen_x + TILE_WIDTH * 2), player_screen_y, sprites_vram_offset + plane_body_sprite, plane_attrs);

    uint8_t plane_engine_sprite = s_engine_speed_to_prop_frames[s_engine_speed][s_prop_frame];
    GBC_Graphics_oam_set_sprite(graphics, 3, clamp_int(0, s_player_screen_x + TILE_WIDTH * 3, s_player_screen_x + TILE_WIDTH * 3), player_screen_y, sprites_vram_offset + plane_engine_sprite, plane_attrs);
}

void player_init(GBC_Graphics *graphics) {
    s_player_x = 0;
    s_player_y = 80;
    s_player_x_speed = 4;
    s_player_y_speed = 0;
    s_player_screen_x = PLAYER_OFF_SCREEN_X;
    s_player_screen_y_offset = 0;
    s_player_state = PS_OFF_SCREEN;
    s_engine_speed = ES_FAST;
    s_player_y_direction = D_NONE;
    set_player_sprite_palette(graphics);
    draw_player_sprites(graphics);
}

void player_step(GBC_Graphics *graphics) {
    s_player_x += s_player_x_speed;

    switch (s_player_state) {
        case PS_ON_SCREEN: 
            if (s_player_y_direction == D_NONE && s_player_y_speed != 0) {
                s_player_y_speed -= 1 * abs(s_player_y_speed) / s_player_y_speed; // just a trick to normalize an integer
            } else {
                s_player_y_speed = clamp_int8_t(-MAX_PLAYER_Y_SPEED, s_player_y_speed + dir_to_point(s_player_y_direction)[1] * 2, MAX_PLAYER_Y_SPEED);
            }
            s_player_y = clamp_uint8_t(MIN_PLAYER_Y, s_player_y + s_player_y_speed, MAX_PLAYER_Y);
            break;
        case PS_MOVING_OFF_LEFT: {
            uint8_t animation_percent = ease_in_out_percentages[s_slide_frame];
            s_player_screen_x = lerp(PLAYER_ON_SCREEN_X, PLAYER_OFF_SCREEN_X, animation_percent);
            s_slide_frame ++;
            if (animation_percent == 100) {
                s_player_state = PS_OFF_SCREEN;
            }
        } break;
        case PS_MOVING_OFF_DOWN: {
            uint8_t fall_animation_percent = accelerate_slow_percentages[s_slide_frame];
            uint8_t slide_animation_percent = accelerate_slow_percentages[s_slide_frame];
            s_player_screen_y_offset = lerp(0, PLAYER_OFF_SCREEN_Y, fall_animation_percent);
            s_player_screen_x = lerp(PLAYER_ON_SCREEN_X, PLAYER_OFF_SCREEN_X, slide_animation_percent);
            s_slide_frame ++;
            if (fall_animation_percent == 100 || slide_animation_percent == 100) {
                s_player_state = PS_OFF_SCREEN;
            }
        } break;
        case PS_MOVING_ON_RIGHT: {
            uint8_t animation_percent = ease_in_out_percentages[s_slide_frame];
            s_player_screen_x = lerp(PLAYER_OFF_SCREEN_X, PLAYER_ON_SCREEN_X, animation_percent);
            s_slide_frame ++;
            if (animation_percent == 100) {
                s_player_state = PS_ON_SCREEN;
            }
        } break;
        default:
            break;
    }

    draw_player_sprites(graphics);
    if (s_engine_speed != ES_STOPPED) {
        s_prop_frame = (s_prop_frame + 1) % s_num_prop_frames_per_speed[s_engine_speed];
        s_bob_frame = (s_bob_frame + 1) % (sizeof(s_bob_offsets) * (s_engine_speed + 1));
    }
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
    player_screen_y += s_bob_offsets[s_bob_frame / (s_engine_speed + 1)];
    return player_screen_y;
}

GRect get_player_collision() {
    return GRect(s_player_x + 20, s_player_y + s_bob_offsets[s_bob_frame / (s_engine_speed + 1)] + 7, 8, 3);
}

void player_set_vertical_direction(Direction new_scroll_dir) {
  s_player_y_direction = new_scroll_dir;
}

void player_move_off_screen_left() {
    s_player_state = PS_MOVING_OFF_LEFT;
    s_slide_frame = 0;
}

void player_move_on_screen_right() {
    s_player_state = PS_MOVING_ON_RIGHT;
    s_slide_frame = 0;
}

void player_move_off_screen_down() {
    s_player_state = PS_MOVING_OFF_DOWN;
    s_slide_frame = 0;
}

bool player_moving() {
    return s_player_state == PS_MOVING_OFF_LEFT || s_player_state == PS_MOVING_ON_RIGHT || s_player_state == PS_MOVING_OFF_DOWN;
}

bool player_on_screen() {
    return s_player_state == PS_ON_SCREEN;
}

void player_set_engine_speed(GBC_Graphics *graphics, EngineSpeed new_speed) {
    s_bob_frame = s_bob_frame * (new_speed + 1) / (s_engine_speed + 1);
    s_engine_speed = new_speed;
    s_prop_frame %= s_num_prop_frames_per_speed[s_engine_speed];
}