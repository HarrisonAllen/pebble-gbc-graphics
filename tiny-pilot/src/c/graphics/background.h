#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

/**
 * @file backgound.h
 * 
 * Handles the rendering and motion of the background layer
 */

#define SOLID_TILE_00 0 // A solid tile using palette color 0
#define SOLID_TILE_01 1 // A solid tile using palette color 1
#define SOLID_TILE_10 2 // A solid tile using palette color 2
#define SOLID_TILE_11 3 // A solid tile using palette color 3

/**
 * Generates a background with new clouds, trees, cars
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void generate_new_game_background(GBC_Graphics *graphics);

/**
 * Renders the background based on the player's position
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param player_x The player's x position in the world
 * @param player_y The player's y position in the world
 */
void render_background(GBC_Graphics *graphics, uint32_t player_x, uint8_t player_y);

/**
 * Get the background's current x scroll position
 * 
 * @return The background's x scroll position
 */
uint8_t get_bg_scroll_x();

/**
 * Get the background's current y scroll position
 * 
 * @return The background's y scroll position
 */
uint8_t get_bg_scroll_y();