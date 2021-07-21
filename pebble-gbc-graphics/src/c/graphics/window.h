#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

/**
 * @file window.h
 * 
 * Handles the drawing and movement of the window layer
 */

#define WINDOW_FRAME_CORNER 4     // The tile for a corner of the window frame
#define WINDOW_FRAME_HORIZONTAL 5 // The tile for the horizontal bar of the window frame
#define WINDOW_FRAME_VERTICAL 6   // The tile for the vertical bar of the window frame
#define WINDOW_PALETTE 7          // The background palette number for the window frame

/**
 * Clears the tiles on the window layer
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void clear_window_layer(GBC_Graphics *graphics);

/**
 * Sets up the window window layer
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void window_init(GBC_Graphics *graphics);

/**
 * Draws the frame on the window layer, and moves the window to the origin
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param frame The rectangle to draw the window frame in
 * @param origin The offset of the window layer
 */
void set_window_layer_frame(GBC_Graphics *graphics, GRect frame, GPoint origin);

/**
 * Starts an animation that will move the window layer from start to end
 * 
 * @param start The start position of the window layer
 * @param end The end position of the window layer
 */
void start_window_animation(GPoint start, GPoint end);

/**
 * Animates the movement of the window layer
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void step_window_animation(GBC_Graphics *graphics);

/**
 * Checks if the window is currently animating
 * 
 * @return Whether or not the window is currently animating
 */
bool is_window_animating();

/**
 * Sets the background rendering priority of the tiles on the window layer
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param has_priority Whether or not the window tiles should have priority over the sprite layer
 */
void set_window_has_priority(GBC_Graphics *graphics, bool has_priority);