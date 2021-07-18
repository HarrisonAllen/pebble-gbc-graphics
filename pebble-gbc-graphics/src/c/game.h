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

typedef enum {
    GS_NEW_GAME,
    GS_PLAYING,
    GS_GAME_OVER
} GameState;

void game_init(Window *window);
void game_deinit();

void select_click_handler(ClickRecognizerRef recognizer, void *context);
void up_press_handler(ClickRecognizerRef recognizer, void *context);
void up_release_handler(ClickRecognizerRef recognizer, void *context);
void down_press_handler(ClickRecognizerRef recognizer, void *context);
void down_release_handler(ClickRecognizerRef recognizer, void *context);