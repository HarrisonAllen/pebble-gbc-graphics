#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"
#include "../util.h"

/**
 * This class handles the items in the game
 */

#define NUMBER_OF_ITEMS 6 // The number of items to render at once
#define NUMBER_OF_BALLOONS 5 // On average, how many of the items should be balloons? The rest will be fuel 

#define ITEM_OAM_OFFSET 4 // The OAM offset for the sprites
#define BALLOON_SPRITE 18 // The tilesheet offset for the balloon sprite
#define PLUS_ONE_SPRITE 20 // The tilesheet offset for the +1 sprite
#define FUEL_SPRITE 22 // The tilesheet offset for the fuel sprite
#define PLUS_FUEL_SPRITE 24 // The tilesheet offset for the +F sprite
#define BALLOON_PALETTE 1 // The sprite palette number for the balloon sprites
#define FUEL_PALETTE 2 // The sprite palette number for the fuel sprites

/**
 * The different types of items
 */
typedef enum {
    IT_NONE,        // An empty item, no collision, invisible
    IT_BALLOON,     // The balloon item, player collects for points, has collision
    IT_PLUS_ONE,    // The +1 item, appears when balloon is collected, no collision
    IT_FUEL,        // The fuel item, player collects to replenish fuel, has collision
    IT_PLUS_FUEL,   // The +F item, appears when fuel is collected, no collision
} ItemTypes;

/**
 * Sets up the items
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void items_init(GBC_Graphics *graphics);

/**
 * Gets the bounding rectangle that defines collision for an item
 * 
 * @param item_id The item to get the collision of
 * @return The GRect that defines the collision for this item
 */
GRect get_item_collision(uint8_t item_id);

/**
 * Gets the data structure for the item
 * 
 * @param item_id The item to get the data of
 * @return A pointer to a list containing:
 *  [0] (ItemType) The type of the item
 *  [1] (uint32_t) The item's x position
 *  [2] (uint32_t) The item's y position
 */
uint32_t *get_item(uint8_t item_id);

/**
 * Handles collision for the given item
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param item_id The item to handle collision for
 */
void handle_collision(GBC_Graphics *graphics, uint8_t item_id);

/**
 * Steps the items, moving, hiding, generating new
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void items_step(GBC_Graphics *graphics);

/**
 * Clears out all of the items
 */
void items_clear();