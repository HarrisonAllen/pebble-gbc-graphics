#pragma once

#include <pebble.h>
#include "pebble-gbc-graphics/pebble-gbc-graphics.h"

// I'm going for 20 fps here, thus 50ms, but you can definitely go faster
// a 2ms frame duration is "as fast as possible"
// I tend to get weird results when using a 1ms frame duration
#define FRAME_DURATION 50

void game_init(Window *window);
void game_deinit();

void select_click_handler(ClickRecognizerRef recognizer, void *context);
void up_press_handler(ClickRecognizerRef recognizer, void *context);
void up_release_handler(ClickRecognizerRef recognizer, void *context);
void down_press_handler(ClickRecognizerRef recognizer, void *context);
void down_release_handler(ClickRecognizerRef recognizer, void *context);