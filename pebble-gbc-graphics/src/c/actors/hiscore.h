#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"
#include "../util.h"

/**
 * This object shows up as "NEW" when a new high score
 * is achieved for the player
 */


#define HI_SCORE_OAM_OFFSET 30 // The OAM offset for the sprites
#define HI_SCORE_SPRITE_1 26 // The tilesheet offset for the first sprite
#define HI_SCORE_SPRITE_2 28 // The tilesheet offset for the second sprite
#define HI_SCORE_SPRITE_3 30 // The tilesheet offset for the third sprite
#define HI_SCORE_PALETTE 3 // The sprite palette number for the high score sprites

#define HI_SCORE_Y_POS PBL_IF_ROUND_ELSE(62, 93) // The x position relative to the window layer
#define HI_SCORE_X_POS PBL_IF_ROUND_ELSE(144, 128) // The y position relative to the window layer
#define HI_SCORE_Y_OFFSET_MAX 3 // The farthest down the object can travel from its origin

/**
 * Initializes the high score object
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void hiscore_init(GBC_Graphics *graphics);

/**
 * Steps the animation for the high score object
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void hiscore_step(GBC_Graphics *graphics);