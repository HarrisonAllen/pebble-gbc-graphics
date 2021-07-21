#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

#define CLOUD_SKY_TILE 0 // The tile of the blank sky
#define CLOUD_PALETTE 0  // The palette of the clouds

/**
 * Draws clouds on the background
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param cloud_vram_start The VRAM offset for the cloud tiles
 */
void draw_clouds(GBC_Graphics *graphics, uint8_t cloud_vram_start);