#pragma once
#include <pebble.h>

// This file contains information for tiles and blocks

#define BLOCK_WIDTH 2          // Width of block in tiles, which is 2 in this example
#define BLOCK_HEIGHT 2         // Height of block in tiles, which is 2 in this example
#define BLOCK_SIZE (BLOCK_WIDTH * BLOCK_HEIGHT) // Number of tiles per block, which is 2x2 tiles for this example
#define BLOCK_BLANK 0          // First block in the tilesheet, blank square
#define BLOCK_CIRCLE 1         // green circle
#define BLOCK_TRIANGLE 2       // red triangle
#define BLOCK_SQUARE 3         // blue square
#define BLOCK_PATTERN 4        // gray patterned block

uint8_t TILESHEET_OFFSETS[] = {
    BLOCK_BLANK * BLOCK_SIZE,
    BLOCK_CIRCLE * BLOCK_SIZE,
    BLOCK_TRIANGLE * BLOCK_SIZE,
    BLOCK_SQUARE * BLOCK_SIZE,
    BLOCK_PATTERN * BLOCK_SIZE,
};

uint8_t TILESHEET_PALETTE[16] = {
#if defined(PBL_COLOR)
    0b11000000, 0b11000000, 0b11000011, 0b11001000, 0b11001011, 0b11001100, 0b11010101, 0b11101010, 0b11100000, 0b11110000, 0b11110110, 0b11111111, 0b11000000, 0b11000000, 0b11000000, 0b11000000,
#else
    0b00, 0b00, 0b10, 0b10, 0b11, 0b10, 0b10, 0b11, 0b10, 0b10, 0b11, 0b11, 0b00, 0b00, 0b00, 0b00,
#endif
};

// This was manually generated, for easier visibility I used the block indices rather than their full block names
#define MAP_BLOCK_WIDTH 9 // 9 blocks wide
#define MAP_BLOCK_HEIGHT 11 // 11 blocks tall (technically 10.5 to fit full height of watch, but can't have an uneven number of tiles)
uint8_t EXAMPLE_MAP[MAP_BLOCK_WIDTH * MAP_BLOCK_HEIGHT] = {
    3, 3, 3, 3, 4, 3, 3, 3, 3, 
    3, 1, 1, 1, 4, 1, 1, 1, 3, 
    3, 1, 1, 1, 4, 1, 1, 1, 3, 
    3, 1, 1, 1, 4, 1, 1, 1, 3, 
    3, 1, 1, 1, 4, 1, 1, 1, 3, 
    4, 4, 4, 4, 4, 4, 4, 4, 4, 
    3, 2, 2, 2, 4, 0, 0, 0, 3, 
    3, 2, 2, 2, 4, 0, 0, 0, 3, 
    3, 2, 2, 2, 4, 0, 0, 0, 3, 
    3, 2, 2, 2, 4, 0, 0, 0, 3, 
    3, 3, 3, 3, 4, 3, 3, 3, 3, 
};

// The blocks to cycle through for the animation
#define NUM_ANIMATION_FRAMES 3
uint8_t ANIMATION_SEQUENCE[NUM_ANIMATION_FRAMES] = {
    BLOCK_CIRCLE,
    BLOCK_TRIANGLE,
    BLOCK_BLANK
};

// The blocks to change as part of the animation, x/y positions
// It's a 2d array, i just organized it to better match the block arrangement
#define NUM_ANIMATED_BLOCKS 10
uint8_t BLOCKS_TO_ANIMATE[NUM_ANIMATED_BLOCKS][2] = {
    {1, 1}, {2, 1}, {3, 1},
    {1, 2},         {3, 2},
    {1, 3},         {3, 3},
    {1, 4}, {2, 4}, {3, 4},
};