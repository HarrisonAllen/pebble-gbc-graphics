#include "items.h"
#include "player.h"
#include "../graphics/background.h"

static uint32_t s_items_data[NUMBER_OF_ITEMS][3]; // Each index contains an item's type, x position, and y position

extern uint8_t sprites_vram_offset; // Need to grab the offset of the sprites tilesheet

/**
 * The sprites for the item types
 */
static uint8_t s_item_sprites[] = {
    0,                  // IT_NONE: Doesn't render, no sprite
    BALLOON_SPRITE,     // IT_BALLOON: A balloon with a string
    PLUS_ONE_SPRITE,    // IT_PLUS_ONE: A '+1'
    FUEL_SPRITE,        // IT_FUEL: A fuel canister on a parachute
    PLUS_FUEL_SPRITE    // IT_PLUS_FUEL: A '+F'
};

/**
 * The sprite palettes for the item types
 */
static uint8_t s_item_palettes[] = {
    0,                  // IT_NONE: Doesn't render, no palette
    BALLOON_PALETTE,    // IT_BALLOON: Primary color is Red
    BALLOON_PALETTE,    // IT_PLUS_ONE: Primary color is Red (unused)
    FUEL_PALETTE,       // IT_FUEL: Primary color is Islamic Green
    FUEL_PALETTE        // IT_PLUS_FUEL: Primary color is Islamic Green (unused)
};

/**
 * Sets the item type to a either IT_BALLOON or IT_FUEL, and moves it off-screen
 * to the right, will later move on-screen once the player reaches it
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param item_index The item to randomize
 */
static void set_item(GBC_Graphics *graphics, uint8_t item_index) {
    uint8_t item_odds = (rand() % NUMBER_OF_ITEMS);
    uint32_t* item = s_items_data[item_index];
    item[0] = (item_odds < NUMBER_OF_BALLOONS) ? IT_BALLOON : IT_FUEL;
    item[1] = get_player_x() + GBC_Graphics_get_screen_width(graphics) + rand() % (GBC_Graphics_get_screen_width(graphics) * 3);
    item[2] = MIN_PLAYER_Y + rand() % (MAX_PLAYER_Y - MIN_PLAYER_Y);

    uint8_t item_attrs = GBC_Graphics_attr_make(s_item_palettes[item[0]], 0, false, false, false);
    GBC_Graphics_oam_set_sprite(graphics, ITEM_OAM_OFFSET + item_index, 0, 0, sprites_vram_offset + s_item_sprites[item[0]], item_attrs);
}

/**
 * Initializes the sprite palettes for the items
 */
static void setup_item_palettes(GBC_Graphics *graphics) {
    /**
     * The colors are:
     *   1. Transparent (doesn't matter)
     *   2. Border color
     *   3. Primary color
     *   4. Detail color
     */
#if defined(PBL_COLOR)
    GBC_Graphics_set_sprite_palette(graphics, BALLOON_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorRedARGB8, GColorWhiteARGB8);
    GBC_Graphics_set_sprite_palette(graphics, FUEL_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorIslamicGreenARGB8, GColorIslamicGreenARGB8);
#else
    GBC_Graphics_set_sprite_palette(graphics, BALLOON_PALETTE, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
    GBC_Graphics_set_sprite_palette(graphics, FUEL_PALETTE, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
#endif
}

void items_init(GBC_Graphics *graphics) {
    setup_item_palettes(graphics);
    for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
        set_item(graphics, i);
    }
}

/**
 * Draws the item sprites
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
static void draw_item_sprites(GBC_Graphics *graphics) {
    for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
        if (s_items_data[i][0] == IT_NONE) { // Don't render the none type items
            GBC_Graphics_oam_hide_sprite(graphics, ITEM_OAM_OFFSET + i);
            continue;
        }

        int item_screen_y = s_items_data[i][2] - get_bg_scroll_y(graphics) + GBC_SPRITE_OFFSET_Y;
        // Check if the item is on screen vertically
        if (item_screen_y <= 0 || item_screen_y > (GBC_Graphics_get_screen_height(graphics) + GBC_TILE_HEIGHT * 2)) {
            GBC_Graphics_oam_hide_sprite(graphics, ITEM_OAM_OFFSET + i);
        } else {
            int32_t item_screen_x = s_items_data[i][1] - get_player_x() + PLAYER_ON_SCREEN_X;
            bool item_overlaps_window = (item_screen_x > GBC_Graphics_window_get_offset_x(graphics) + GBC_SPRITE_OFFSET_X
                && item_screen_y > GBC_Graphics_window_get_offset_y(graphics) + GBC_SPRITE_OFFSET_Y);

            // Then, check if the item is on screen horizontally and isn't obscured by the window
            if (item_screen_x <= 0 || item_screen_x > (GBC_Graphics_get_screen_width(graphics) + GBC_TILE_WIDTH) || item_overlaps_window) {
                GBC_Graphics_oam_hide_sprite(graphics, ITEM_OAM_OFFSET + i);
            } else {
                GBC_Graphics_oam_set_sprite_pos(graphics, ITEM_OAM_OFFSET + i, item_screen_x, item_screen_y);
            }
        }
    }
}

GRect get_item_collision(uint8_t item_id) {
    uint32_t *item = s_items_data[item_id];
    return GRect(item[1], item[2], GBC_TILE_WIDTH, GBC_TILE_HEIGHT * 2); // All items have a 1x2 tile collision
}

uint32_t *get_item(uint8_t item_id) {
    return s_items_data[item_id];
}

void handle_collision(GBC_Graphics *graphics, uint8_t item_id) {
    uint32_t *item = s_items_data[item_id];
    switch (item[0]) {
        case IT_BALLOON: // For balloon, replace with plus one
            item[0] = IT_PLUS_ONE;
            GBC_Graphics_oam_set_sprite_tile(graphics, ITEM_OAM_OFFSET + item_id, sprites_vram_offset + s_item_sprites[item[0]]);
            break;
        case IT_FUEL: // For fuel, replace with fuel
            item[0] = IT_PLUS_FUEL;
            GBC_Graphics_oam_set_sprite_tile(graphics, ITEM_OAM_OFFSET + item_id, sprites_vram_offset + s_item_sprites[item[0]]);
            break;
        default:
            break;
    }
}

void items_step(GBC_Graphics *graphics) {
    for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
        uint32_t *item = s_items_data[i];
        // Do nothing for none type items
        if (item[0] == IT_NONE) {
            continue;
        }

        // Move item to the left
        item[1] -= 1;

        // For the +1 and +F items, move upwards
        if (item[0] == IT_PLUS_ONE || item[0] == IT_PLUS_FUEL) {
            item[2] -= 1;
        }

        // If the item goes off-screen, re-roll it into a new item
        int item_screen_x = item[1] - get_player_x() + get_player_screen_x();
        if (item_screen_x <= 0 && is_player_on_screen()) {
            set_item(graphics, i);
        }
    }

    // And don't forget to render!
    draw_item_sprites(graphics);
}

void items_clear() {
    for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
        s_items_data[i][0] = IT_NONE;
    }
}