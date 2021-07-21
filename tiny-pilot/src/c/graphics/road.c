#include "road.h"

/**
 * Initializes the background palettes for the road tiles
 */
static void load_palettes(GBC_Graphics *graphics) {
    /**
     * For the ground palette, the colors are:
     *   1. Sky color
     *   2. Tree trunk color
     *   3. Tree/horizon edge color
     *   4. Tree/ground fill color
     * For the tree road palette, the colors are:
     *   1. Road color
     *   2. Road edge color
     *   3. Tree edge color
     *   4. Tree fill color
     * For the road palette, the colors are:
     *   1. Road color
     *   2. Road edge color
     *   3. Car/Sidewalk fill color
     *   4. Detail color
     */
#if defined(PBL_COLOR)
    GBC_Graphics_set_bg_palette(graphics, GROUND_PALETTE, GColorPictonBlueARGB8, GColorWindsorTanARGB8, GColorDarkGreenARGB8, GColorMayGreenARGB8);
    GBC_Graphics_set_bg_palette(graphics, TREE_ROAD_PALETTE, GColorBlackARGB8, GColorDarkGrayARGB8, GColorDarkGreenARGB8, GColorMayGreenARGB8);
    GBC_Graphics_set_bg_palette(graphics, ROAD_PALETTE, GColorBlackARGB8, GColorDarkGrayARGB8, GColorLightGrayARGB8, GColorYellowARGB8);
#else
    GBC_Graphics_set_bg_palette(graphics, GROUND_PALETTE, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_BLACK, GBC_COLOR_WHITE);
    GBC_Graphics_set_bg_palette(graphics, TREE_ROAD_PALETTE, GBC_COLOR_BLACK, GBC_COLOR_BLACK, GBC_COLOR_BLACK, GBC_COLOR_WHITE);
    GBC_Graphics_set_bg_palette(graphics, ROAD_PALETTE, GBC_COLOR_BLACK, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
#endif
}

void draw_road(GBC_Graphics *graphics, uint8_t road_vram_offset) {
    // Set each palette individually
    load_palettes(graphics);

    uint8_t ground_attrs = GBC_Graphics_attr_make(GROUND_PALETTE, 0, false, false, false);
    uint8_t road_attrs = GBC_Graphics_attr_make(ROAD_PALETTE, 0, false, false, false);

    // Fill in the base for the road at the bottom of the background tilemap
    for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) { 
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, GBC_TILEMAP_HEIGHT - 5, SKY_TILE, ground_attrs);
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, GBC_TILEMAP_HEIGHT - 4, road_vram_offset + HORIZON, ground_attrs);
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, GBC_TILEMAP_HEIGHT - 3, road_vram_offset + ROAD_TOP, road_attrs);
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, GBC_TILEMAP_HEIGHT - 2, road_vram_offset + ROAD_BOT, road_attrs);
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, GBC_TILEMAP_HEIGHT - 1, GROUND_TILE, ground_attrs);
    }

    // Place the upper row of trees
    uint8_t tile_odds;
    for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) {
        tile_odds = rand()%4;
        switch (tile_odds) {
            case 0:
                GBC_Graphics_bg_set_tile_and_attrs(graphics, x, GBC_TILEMAP_HEIGHT - 5, road_vram_offset + TREE_TOP, ground_attrs);
                GBC_Graphics_bg_set_tile(graphics, x, GBC_TILEMAP_HEIGHT - 4, road_vram_offset + TREE_BOT_SKY);
                x++;
                break; 
            case 1:
                GBC_Graphics_bg_set_tile(graphics, x, GBC_TILEMAP_HEIGHT - 4, road_vram_offset + TREE_SMALL_SKY);
                x++;
                break;
            default:
                break;
        }
    }

    // Place cars on the road
    for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) {
        tile_odds = rand()%4;
        switch (tile_odds) {
            case 0:
                GBC_Graphics_bg_set_tile(graphics, x, GBC_TILEMAP_HEIGHT - 3, road_vram_offset + CAR_TOP_LEFT);
                GBC_Graphics_bg_set_tile(graphics, x + 1, GBC_TILEMAP_HEIGHT - 3, road_vram_offset + CAR_TOP_RIGHT);
                GBC_Graphics_bg_set_tile(graphics, x, GBC_TILEMAP_HEIGHT - 2, road_vram_offset + CAR_BOT_LEFT);
                GBC_Graphics_bg_set_tile(graphics, x + 1, GBC_TILEMAP_HEIGHT - 2, road_vram_offset + CAR_BOT_RIGHT);
                x += 2;
                break;
            default:
                break;
        }
    }

    // Place lower trees below the road
    for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) {
        tile_odds = rand()%4;
        switch (tile_odds) {
            case 0:
                GBC_Graphics_bg_set_tile(graphics, x, GBC_TILEMAP_HEIGHT - 1, road_vram_offset + TREE_SMALL_GROUND);
                x++;
                break;
            default:
                break;
        }
    }
}