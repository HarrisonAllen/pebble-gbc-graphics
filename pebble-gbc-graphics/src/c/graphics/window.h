#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

#define WINDOW_FRAME_CORNER 4
#define WINDOW_FRAME_HORIZONTAL 5
#define WINDOW_FRAME_VERTICAL 6

#define WINDOW_PALETTE 7

void clear_window_layer(GBC_Graphics *graphics);
void window_init(GBC_Graphics *graphics);
void set_window_layer_frame(GBC_Graphics *graphics, GRect frame, GPoint origin);
void start_window_animation(GPoint start, GPoint end);
void step_window_animation(GBC_Graphics *graphics);
bool is_window_animating();
void set_window_has_priority(GBC_Graphics *graphics, bool has_priority);