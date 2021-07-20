#include "player.h"
#include "../util.h"

static uint32_t s_player_x; // The player's x position in the world
static uint8_t s_player_y; // The player's y position in the world
static uint8_t s_player_x_speed; // The player's horizontal speed
static int8_t s_player_y_speed; // The player's vertical velocity
static int s_player_screen_x; // The player's x position on the screen, for animations
static int s_player_screen_y_offset; // The player's y position offset from the screen position, for animations
static Direction s_player_y_direction; // The player's vertical direction

static uint8_t s_prop_frame; // The animation frame for the propellor
static uint8_t s_bob_frame; // The animation frame for the up and down bobbing
static uint8_t s_slide_frame; // The animation frame for sliding in and out

static PlayerState s_player_state; // The player's state
static EngineSpeed s_engine_speed; // The speed of the engine

extern uint8_t sprites_vram_offset; // Need to grab the offset of the sprites tilesheet
extern const uint8_t ease_in_out_percentages[]; // Want to grab the ease in, ease out animation sequence
extern const uint8_t accelerate_slow_percentages[]; // Also want the slow acceleration animation sequence

/**
 * Custom animation sequence for bobbing up and down,
 * centered around zero
 */
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

// The propellor frames to use for a fast engine speed
static const uint8_t s_prop_fast_frames[] = {
    PLANE_NOSE_0_SPRITE,
    PLANE_NOSE_3_SPRITE,
};

// The propellor frames to use for a medium engine speed
static const uint8_t s_prop_medium_frames[] = {
    PLANE_NOSE_0_SPRITE,
    PLANE_NOSE_2_SPRITE,
    PLANE_NOSE_3_SPRITE,
    PLANE_NOSE_1_SPRITE,
};

// The propellor frames to use for a slow engine speed
static const uint8_t s_prop_slow_frames[] = {
    PLANE_NOSE_0_SPRITE,
    PLANE_NOSE_1_SPRITE,
    PLANE_NOSE_2_SPRITE,
    PLANE_NOSE_3_SPRITE,
    PLANE_NOSE_2_SPRITE,
    PLANE_NOSE_1_SPRITE,
};

// Convert an engine speed to a list of propellor frames
static const uint8_t *s_engine_speed_to_prop_frames[] = {
    s_prop_fast_frames,
    s_prop_medium_frames,
    s_prop_slow_frames,
    s_prop_slow_frames,
};

// How many frame are in the prop frames for that engine speed
static const uint8_t s_num_prop_frames_per_speed[] = {
    2, 4, 6, 6
};

static void set_player_sprite_palette(GBC_Graphics *graphics) {
    /**
     * The colors are:
     *   1. Transparent (doesn't matter)
     *   2. Border color
     *   3. Primary color
     *   4. Detail color
     */
#if defined(PBL_COLOR)
    GBC_Graphics_set_sprite_palette(graphics, PLAYER_SPRITE_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorRedARGB8, GColorWhiteARGB8);
#else
    GBC_Graphics_set_sprite_palette(graphics, PLAYER_SPRITE_PALETTE, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
#endif
}

/**
 * Renders the player's sprites
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
static void draw_player_sprites(GBC_Graphics *graphics) {
    uint8_t plane_attrs = GBC_Graphics_attr_make(PLAYER_SPRITE_PALETTE, 0, false, false, false);

    // Set the player y (relative to the background) and offset by the bobbing animation
    uint8_t player_screen_y = clamp_int(0, s_player_y - GBC_Graphics_bg_get_scroll_y(graphics) + GBC_SPRITE_OFFSET_Y + s_player_screen_y_offset, UINT8_MAX);
    player_screen_y += s_bob_offsets[s_bob_frame / (s_engine_speed + 1)];

    // Draw the rudder
    GBC_Graphics_oam_set_sprite(graphics, 0, clamp_int(0, s_player_screen_x, s_player_screen_x), player_screen_y, sprites_vram_offset + PLANE_RUDDER_SPRITE, plane_attrs);

    // Draw the tail
    GBC_Graphics_oam_set_sprite(graphics, 1, clamp_int(0, s_player_screen_x + GBC_TILE_WIDTH, s_player_screen_x + GBC_TILE_WIDTH), player_screen_y, sprites_vram_offset + PLANE_TAIL_SPRITE, plane_attrs);

    // Based on player direction, draw the body
    uint8_t plane_body_sprite;
    switch (s_player_y_direction) {
        case D_UP:
            plane_body_sprite = PLANE_BODY_UP_SPRITE;
            break;
        case D_DOWN:
            plane_body_sprite = PLANE_BODY_DOWN_SPRITE;
            break;
        default:
            plane_body_sprite = PLANE_BODY_SPRITE;
            break;
    }
    GBC_Graphics_oam_set_sprite(graphics, 2, clamp_int(0, s_player_screen_x + GBC_TILE_WIDTH * 2, s_player_screen_x + GBC_TILE_WIDTH * 2), player_screen_y, sprites_vram_offset + plane_body_sprite, plane_attrs);

    // Based on engine speed and the propellor frame, draw the nose
    uint8_t plane_engine_sprite = s_engine_speed_to_prop_frames[s_engine_speed][s_prop_frame];
    GBC_Graphics_oam_set_sprite(graphics, 3, clamp_int(0, s_player_screen_x + GBC_TILE_WIDTH * 3, s_player_screen_x + GBC_TILE_WIDTH * 3), player_screen_y, sprites_vram_offset + plane_engine_sprite, plane_attrs);
}

void player_init(GBC_Graphics *graphics) {
    s_player_x = 0;
    s_player_y = PLAYER_START_Y;
    s_player_x_speed = PLAYER_X_SPEED;
    s_player_y_speed = 0;
    s_player_screen_x = PLAYER_OFF_SCREEN_X;
    s_player_screen_y_offset = 0;
    s_player_y_direction = D_NONE;
    s_player_state = PS_OFF_SCREEN;
    s_engine_speed = ES_FAST;
    set_player_sprite_palette(graphics);
    draw_player_sprites(graphics);
}

void player_step(GBC_Graphics *graphics) {
    // Move the player to the right through the world
    s_player_x += s_player_x_speed;

    switch (s_player_state) {
        // Playing the game, so move up and down based on user inputs
        case PS_ON_SCREEN: 
            if (s_player_y_direction == D_NONE && s_player_y_speed != 0) {
                s_player_y_speed -= 1 * abs(s_player_y_speed) / s_player_y_speed; // just a trick to normalize an integer
            } else {
                s_player_y_speed = clamp_int8_t(-MAX_PLAYER_Y_SPEED, s_player_y_speed + dir_to_point(s_player_y_direction)[1] * 2, MAX_PLAYER_Y_SPEED);
            }
            s_player_y = clamp_uint8_t(MIN_PLAYER_Y, s_player_y + s_player_y_speed, MAX_PLAYER_Y);
            break;

        // Ease the player off the screen to the left
        case PS_MOVING_OFF_LEFT: {
            uint8_t animation_percent = ease_in_out_percentages[s_slide_frame];
            s_player_screen_x = lerp(PLAYER_ON_SCREEN_X, PLAYER_OFF_SCREEN_X, animation_percent);
            s_slide_frame ++;
            if (animation_percent == 100) {
                s_player_state = PS_OFF_SCREEN;
            }
        } break;

        // Accelerate the player off the screen downwards (falling) and ease off to the left (slowing)
        case PS_MOVING_OFF_DOWN_AND_LEFT: {
            uint8_t fall_animation_percent = accelerate_slow_percentages[s_slide_frame];
            s_player_screen_y_offset = lerp(0, PLAYER_OFF_SCREEN_Y, fall_animation_percent);
            
            uint8_t slide_animation_percent = accelerate_slow_percentages[s_slide_frame];
            s_player_screen_x = lerp(PLAYER_ON_SCREEN_X, PLAYER_OFF_SCREEN_X, slide_animation_percent);
            s_slide_frame ++;

            if (fall_animation_percent == 100 || slide_animation_percent == 100) {
                s_player_state = PS_OFF_SCREEN;
            }
        } break;

        // Ease the player on the screen from the left
        case PS_MOVING_ON_LEFT: {
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

    // Don't forget to render
    draw_player_sprites(graphics);

    // Keep the propellor and bobbing animations going
    if (s_engine_speed != ES_STOPPED) {
        s_prop_frame = (s_prop_frame + 1) % s_num_prop_frames_per_speed[s_engine_speed];
        s_bob_frame = (s_bob_frame + 1) % (sizeof(s_bob_offsets) * (s_engine_speed + 1));
    }
}

uint32_t get_player_x() {
    return s_player_x;
}

uint8_t get_player_y() {
    return s_player_y;
}

uint8_t get_player_screen_x() {
    return s_player_screen_x;
}

uint8_t get_player_screen_y(GBC_Graphics *graphics) {
    uint8_t player_screen_y = s_player_y - GBC_Graphics_bg_get_scroll_y(graphics) + GBC_SPRITE_OFFSET_Y;
    player_screen_y += s_bob_offsets[s_bob_frame / (s_engine_speed + 1)];
    return player_screen_y;
}

GRect get_player_collision() {
    return GRect(s_player_x + 20, s_player_y + s_bob_offsets[s_bob_frame / (s_engine_speed + 1)] + 7, 8, 3); // Small rectangle around the body/nose
}

void player_set_vertical_direction(Direction new_dir) {
  s_player_y_direction = new_dir;
}

void player_move_off_screen_left() {
    s_player_state = PS_MOVING_OFF_LEFT;
    s_slide_frame = 0;
}

void player_move_on_screen_left() {
    s_player_state = PS_MOVING_ON_LEFT;
    s_slide_frame = 0;
}

void player_move_off_screen_down() {
    s_player_state = PS_MOVING_OFF_DOWN_AND_LEFT;
    s_slide_frame = 0;
}

bool is_player_moving() {
    return s_player_state == PS_MOVING_OFF_LEFT || s_player_state == PS_MOVING_ON_LEFT || s_player_state == PS_MOVING_OFF_DOWN_AND_LEFT;
}

bool is_player_on_screen() {
    return s_player_state == PS_ON_SCREEN;
}

void player_set_engine_speed(GBC_Graphics *graphics, EngineSpeed new_speed) {
    s_bob_frame = s_bob_frame * (new_speed + 1) / (s_engine_speed + 1); // Make sure the new bob frame lines up with the old one
    s_engine_speed = new_speed;
    s_prop_frame %= s_num_prop_frames_per_speed[s_engine_speed]; // Constrain the prop frame to the current number of frames
}