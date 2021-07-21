#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

#define SKY_TILE 0    // The tile to use for the sky
#define GROUND_TILE 3 // The tile to use for the ground

#define TREE_TOP 0          // The tile for the top of tall trees
#define TREE_BOT_SKY 1      // The tile for the bottom of tall trees on the horizon
#define HORIZON 2           // The tile for the horizon
#define TREE_SMALL_GROUND 3 // The tile for the small trees on the bottom
#define TREE_BOT_GROUND 4   // The tile for the bottom of trees on the bottom
#define TREE_SMALL_SKY 5    // The tile for small trees on the horizon
#define ROAD_TOP 6          // The tile for the top of the road
#define ROAD_BOT 7          // The tile for the bottom of the road
#define CAR_TOP_LEFT 8      // The tile for the top left of a car
#define CAR_TOP_RIGHT 9     // The tile for the top right of a car
#define CAR_BOT_LEFT 10     // The tile for the bottom left of a car
#define CAR_BOT_RIGHT 11    // The tile for the bottom right of a car

#define GROUND_PALETTE 1    // The palette to use for ground tiles
#define TREE_ROAD_PALETTE 2 // The palette to use for trees overlapping the road
#define ROAD_PALETTE 3      // The palette to use for the road

/**
 * Draws the road
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param cloud_vram_start The VRAM offset for the cloud tiles
 */
void draw_road(GBC_Graphics *graphics, uint8_t road_vram_offset);