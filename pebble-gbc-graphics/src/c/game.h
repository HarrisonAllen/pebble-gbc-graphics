#pragma once

#include <pebble.h>
#include "pebble-gbc-graphics/pebble-gbc-graphics.h"

// This is the duration of a frame, in milliseconds
// E.g. a FRAME_DURATION of 33ms will give ~30 fps
// For "as fast as possible" (i.e. render as soon as the
// graphics engine has completed its last draw cycle),
// use a FRAME_DURATION of 2. Keep in mind that black
// and white pebbles will render much faster than the
// color pebbles. I manually tested different values for
// the non-color frame duration to find one that matched the
// color fps, giving a consistent experience across all pebbles
#if defined(PBL_COLOR)
#define FRAME_DURATION 50
#else
#define FRAME_DURATION 20
#endif

#define MAX_PLAYER_FUEL 500
#define NUM_FUEL_BARS 5
#define SCORE_BAR_OFFSET PBL_IF_ROUND_ELSE(5, 2)

#define NEW_GAME_WINDOW GRect(PBL_IF_ROUND_ELSE(3, 1), 0, 15, 4)
#define NEW_GAME_WINDOW_END GPoint(0, GBC_Graphics_get_screen_height(s_graphics) - 48)
#define NEW_GAME_WINDOW_START GPoint(0, GBC_Graphics_get_screen_height(s_graphics))

#if defined(PBL_ROUND)
    #define GAME_OVER_WINDOW GRect(3, 2, 15, 8)
#else
    #define GAME_OVER_WINDOW GRect(1, 6, 15, 8)
#endif
#define GAME_OVER_WINDOW_START GPoint(GBC_Graphics_get_screen_width(s_graphics), 8)
#define GAME_OVER_WINDOW_END GPoint(0, 8)

#define SAVE_KEY 0

typedef struct {
    uint16_t high_score;
} SaveData;

typedef enum {
    GS_LOAD_IN_TRANSITION,
    GS_NEW_GAME,
    GS_NEW_GAME_TRANSITION,
    GS_PLAYING,
    GS_MOVE_PLAYER_OFF_SCREEN,
    GS_GAME_OVER_TRANSITION,
    GS_GAME_OVER,
    GS_GAME_OVER_NEW_GAME_TRANSITION,
    GS_MOVE_PLAYER_ON_SCREEN,
} GameState;

void game_init(Window *window);
void game_deinit();

void select_click_handler(ClickRecognizerRef recognizer, void *context);
void back_click_handler(ClickRecognizerRef recognizer, void *context);
void up_press_handler(ClickRecognizerRef recognizer, void *context);
void up_release_handler(ClickRecognizerRef recognizer, void *context);
void down_press_handler(ClickRecognizerRef recognizer, void *context);
void down_release_handler(ClickRecognizerRef recognizer, void *context);