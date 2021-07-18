#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"
#include "../util.h"

#define NUMBER_OF_ITEMS 6
#define BALLOON_ODDS 5
#define FUEL_ODDS 2
#define BALLOON_ID 0
#define PLUS_ONE_ID 1
#define FUEL_ID 2

#define ITEM_SPRITE_OFFSET 4
#define BALLOON_PALETTE 1
#define FUEL_PALETTE 2
#define BALLOON_SPRITE 18
#define PLUS_ONE_SPRITE 20
#define FUEL_SPRITE 22

void items_init(GBC_Graphics *graphics);
void items_step(GBC_Graphics *graphics);