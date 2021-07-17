#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"

#define CLOUD_PALETTE 0
#define CLOUD_SKY_TILE 0

void draw_clouds(GBC_Graphics *graphics, uint8_t cloud_vram_start);