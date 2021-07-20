#pragma once

#include <pebble.h>
#include "pebble-gbc-graphics/pebble-gbc-graphics.h"

/**
 * Various utility functions that are non-class specific
 */

/**
 * Various directions used for movement
 */
typedef enum {
    D_NONE,     // No direction
    D_UP,       // Upwards direction
    D_LEFT,     // Leftwards direction
    D_DOWN,     // Downwards direction
    D_RIGHT,    // Rightwards direction
    D_END       // Can use for looping through directions, e.g. (direction + 1) % D_END
} Direction;

/**
 * Clamps an int between two values
 * 
 * @param min The lower bound of the clamp
 * @param val The value to clamp
 * @param max The upper bound of the clamp
 * @return A clamped value
 */
int clamp_int(int min, int val, int max);

/**
 * Clamps an uint8_t between two values
 * 
 * @param min The lower bound of the clamp
 * @param val The value to clamp
 * @param max The upper bound of the clamp
 * @return A clamped value
 */
uint8_t clamp_uint8_t(uint8_t min, uint8_t val, uint8_t max);

/**
 * Clamps an int8_t between two values
 * 
 * @param min The lower bound of the clamp
 * @param val The value to clamp
 * @param max The upper bound of the clamp
 * @return A clamped value
 */
uint8_t clamp_int8_t(int8_t min, int8_t val, int8_t max);


/**
 * Converts a direction to a normalized point
 * 
 * @param min The lower bound of the clamp
 * @param val The value to clamp
 * @param max The upper bound of the clamp
 * @return A pointer to an array containing the vector:
 *  [0] The normalized x vector
 *  [1] The normalized y vector
 */
int8_t *dir_to_point(Direction dir);

/**
 * Checks if two GRects overlap
 * @note Touching edges counts as overlap
 * 
 * @param rect1 The first rectangle to compare
 * @param rect2 The second rectangle to compare
 * @return Whether or not rect1 and rect2 overlap
 */
bool grects_overlap(GRect rect1, GRect rect2);

/** 
 * Loads the tilesheets for this project into VRAM
 * @note This will log to info how many tiles were loaded
 * 
 * @param graphics A pointer to the GBC_Graphics object to load into
 */
void load_tilesheets(GBC_Graphics *graphics);

/** 
 * Performs a linear interpolation from start to end based on percent
 * 
 * @param start The value to start the interpolation from
 * @param end The value the interpolation ends on
 * @param percent The percentage of completion on the lerp
 * @note For percent, a value of 50 corresponds to 50%, and so on
 */
int lerp(int start, int end, int percent);