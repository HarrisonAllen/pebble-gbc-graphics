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
#define PLUS_FUEL_ID 3

#define ITEM_SPRITE_OFFSET 4
#define BALLOON_PALETTE 1
#define FUEL_PALETTE 2
#define BALLOON_SPRITE 18
#define PLUS_ONE_SPRITE 20
#define FUEL_SPRITE 22
#define PLUS_FUEL_SPRITE 24

void items_init(GBC_Graphics *graphics);
GRect get_item_collision(uint8_t item_id);
uint *get_item(uint8_t item_id);
void handle_collision(GBC_Graphics *graphics, uint8_t item_id);
void items_step(GBC_Graphics *graphics);