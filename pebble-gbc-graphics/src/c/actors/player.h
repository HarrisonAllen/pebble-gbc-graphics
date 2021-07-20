#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"
#include "../util.h"

#define MIN_PLAYER_Y 8 // The highest position that the player can go
#define MAX_PLAYER_Y 208 // The lowest position that the player can go
#define MAX_PLAYER_Y_SPEED 6 // The fastest that the player can move vertically
#define PLAYER_START_Y 80 // The starting y position for the player
#define PLAYER_X_SPEED 4 // The player's horizontal speed

#define PLAYER_ON_SCREEN_X PBL_IF_ROUND_ELSE(32, 16) // The x position of the player when on scren
#define PLAYER_OFF_SCREEN_X -32 // The x position of the player when off screen left
#define PLAYER_OFF_SCREEN_Y GBC_Graphics_get_screen_height(graphics) // The y position of the player when off screen down

#define PLANE_RUDDER_SPRITE 0 // The tilesheet offset for the rudder (1st sprite)
#define PLANE_TAIL_SPRITE 2 // The tilesheet offset for the tail (2nd sprite)
#define PLANE_BODY_SPRITE 4 // The tilesheet offset for the body when going straight (3rd sprite)
#define PLANE_BODY_UP_SPRITE 6 // The tilesheet offset for the body when going up (3rd sprite)
#define PLANE_BODY_DOWN_SPRITE 8 // The tilesheet offset for the body when going down (3rd sprite)
#define PLANE_NOSE_0_SPRITE 10 // The tilesheet offset for nose, propellor vertical (4th sprite)
#define PLANE_NOSE_1_SPRITE 12 // The tilesheet offset for nose, propellor mostly vertical (4th sprite)
#define PLANE_NOSE_2_SPRITE 14 // The tilesheet offset for nose, propellor mostly horizontal (4th sprite)
#define PLANE_NOSE_3_SPRITE 16 // The tilesheet offset for nose, propellor horizontal (4th sprite)
#define PLAYER_SPRITE_PALETTE 0 // The sprite palette number for the plane sprites

/**
 * The state of the player
 */
typedef enum {
    PS_ON_SCREEN,               // The player is on-screen
    PS_OFF_SCREEN,              // The player is off-screen
    PS_MOVING_OFF_LEFT,         // The player is moving off of the screen to the left
    PS_MOVING_ON_LEFT,          // The player is moving on to the screen from the left
    PS_MOVING_OFF_DOWN_AND_LEFT // The player is moving off of the screen downwards and to the left
} PlayerState;

/**
 * The speed of the engine
 */
typedef enum {
    ES_FAST,    // Fast propellor spin, fast bobbing
    ES_MEDIUM,  // Medium propellor spin, medium bobbing
    ES_SLOW,    // Slow propellor spin, slow bobbing
    ES_STOPPED, // No propellor spin, no bobbing
} EngineSpeed;

/**
 * Sets up the player
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void player_init(GBC_Graphics *graphics);

/**
 * Steps through the player's game logic
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void player_step(GBC_Graphics *graphics);

/**
 * Get the player's x position in the world
 * 
 * @return The player's x position in the world
 */
uint32_t get_player_x();

/**
 * Get the player's y position in the world
 * 
 * @return The player's y position in the world
 */
uint8_t get_player_y();

/**
 * Get the player's x position on the screen
 * 
 * @return The player's x position on the screen
 */
uint8_t get_player_screen_x();

/**
 * Get the player's y position on the screen
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @return The player's y position on the screen
 */
uint8_t get_player_screen_y(GBC_Graphics *graphics);

/**
 * Get the bounding rectangle that defines collision for the player
 * 
 * @return The GRect that defines the collision for the player
 */
GRect get_player_collision();

/**
 * Set the vertical direction of the player
 * 
 * @param new_dir The new vertical direction
 */
void player_set_vertical_direction(Direction new_dir);

/**
 * Start moving the player off of the screen to the left
 */
void player_move_off_screen_left();

/**
 * Start moving the player on to the screen from the left
 */
void player_move_on_screen_left();

/**
 * Start moving the player off of the screen downwards
 */
void player_move_off_screen_down();

/**
 * Check if the player is currently moving on or off the screen
 *
 * @return Whether or not the player is moving on or off the screen
 */
bool is_player_moving();

/**
 * Check if the player is currently on the screen and not moving
 *
 * @return Whether or not the player is on the screen and not moving
 */
bool is_player_on_screen();

/**
 * Sets the engine speed
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param new_speed The new engine speed
 */
void player_set_engine_speed(GBC_Graphics *graphics, EngineSpeed new_speed);