#pragma once

#include <pebble.h>
#include "pebble-gbc-graphics/pebble-gbc-graphics.h"

/*
 * This is the duration of a frame, in milliseconds (more or less)
 * E.g. a FRAME_DURATION of 33ms will give ~30 fps
 * For "as fast as possible" (i.e. render as soon as the
 * graphics engine has completed its last draw cycle),
 * use a FRAME_DURATION of 2. 
 * Black and white Pebbles tend to behave differently than color
 * Pebbles, so you may have to play around with these values to 
 * find one that works best for your application
 */
#if defined(PBL_COLOR)
#define FRAME_DURATION 50
#else
#define FRAME_DURATION 20
#endif

#define MAX_PLAYER_FUEL 500 // The amount of fuel the player has
#define NUM_FUEL_BARS 5 // The number of fuel bars to render the fuel into
#define SCORE_BAR_OFFSET PBL_IF_ROUND_ELSE(5, 2) // The x offset of the score bar

#define NEW_GAME_WINDOW GRect(PBL_IF_ROUND_ELSE(3, 1), 0, 15, 4) // The frame for the New Game window
#define NEW_GAME_WINDOW_START GPoint(0, GBC_Graphics_get_screen_height(s_graphics)) // The start position of the New Game window animation
#define NEW_GAME_WINDOW_END GPoint(0, GBC_Graphics_get_screen_height(s_graphics) - 48) // The end position of the New Game window animation

#define GAME_OVER_WINDOW GRect(PBL_IF_ROUND_ELSE(3, 1), PBL_IF_ROUND_ELSE(2, 6), 15, 8) // The frame for the Game Over window
#define GAME_OVER_WINDOW_START GPoint(GBC_Graphics_get_screen_width(s_graphics), 8) // The start position of the Game Over window animation
#define GAME_OVER_WINDOW_END GPoint(0, 8) // The end position of the Game Over window animation

#define SAVE_KEY 0 // The key for the save file

/**
 * The save data structure
 */
typedef struct {
    uint16_t high_score; // The player's high score
} SaveData;

/**
 * The states of the game
 */
typedef enum {
    GS_LOAD_IN_TRANSITION,              // Load in, new game window and player fly in
    GS_NEW_GAME,                        // Waiting for player input to start
    GS_NEW_GAME_TRANSITION,             // Hiding new game window
    GS_PLAYING,                         // Main gameplay state
    GS_MOVE_PLAYER_OFF_SCREEN,          // Game ended, player moving off screen
    GS_GAME_OVER_TRANSITION,            // Game over window fly in
    GS_GAME_OVER,                       // Waiting for player input to start again
    GS_GAME_OVER_NEW_GAME_TRANSITION,   // Game over window fly out
    GS_MOVE_PLAYER_ON_SCREEN,           // Player fly in
} GameState;

/**
 * Create a new game
 * 
 * @param window The window that the graphics should render into
 */
void game_init(Window *window);

/**
 * Destroy a game session
 */
void game_deinit();