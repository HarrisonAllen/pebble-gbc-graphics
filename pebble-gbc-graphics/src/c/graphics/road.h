#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

// palette indices
#define GROUND_PALETTE 1
#define TREE_ROAD_PALETTE 2
#define ROAD_PALETTE 3

// tiles from the base tilesheet
#define GROUND_TILE 3
#define SKY_TILE 0

// tiles from the road tilesheet
#define TREE_TOP 0
#define TREE_BOT_SKY 1
#define HORIZON 2
#define TREE_SMALL_GROUND 3
#define TREE_BOT_GROUND 4
#define TREE_SMALL_SKY 5
#define ROAD_TOP 6
#define ROAD_BOT 7
#define CAR_TOP_LEFT 8
#define CAR_TOP_RIGHT 9
#define CAR_BOT_LEFT 10
#define CAR_BOT_RIGHT 11


void draw_road(GBC_Graphics *graphics, uint8_t road_vram_offset);