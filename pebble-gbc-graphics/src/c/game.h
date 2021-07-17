#pragma once

#include <pebble.h>
#include "pebble-gbc-graphics/pebble-gbc-graphics.h"

#define FRAME_DURATION 50

void game_init(Window *window);
void game_deinit();

void select_click_handler(ClickRecognizerRef recognizer, void *context);
void up_press_handler(ClickRecognizerRef recognizer, void *context);
void up_release_handler(ClickRecognizerRef recognizer, void *context);
void down_press_handler(ClickRecognizerRef recognizer, void *context);
void down_release_handler(ClickRecognizerRef recognizer, void *context);