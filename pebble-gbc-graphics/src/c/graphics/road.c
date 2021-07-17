#include "road.h"

static void load_palettes(GBC_Graphics *graphics) {
    // Here, I explicitly set the values of the palette array
#if defined(PBL_COLOR)
    GBC_Graphics_set_bg_palette(graphics, GROUND_PALETTE, GColorPictonBlueARGB8, GColorWindsorTanARGB8, GColorDarkGreenARGB8, GColorMayGreenARGB8);
    GBC_Graphics_set_bg_palette(graphics, TREE_ROAD_PALETTE, GColorBlackARGB8, GColorDarkGrayARGB8, GColorDarkGreenARGB8, GColorMayGreenARGB8);
    GBC_Graphics_set_bg_palette(graphics, ROAD_PALETTE, GColorBlackARGB8, GColorDarkGrayARGB8, GColorLightGrayARGB8, GColorYellowARGB8);
#else
    GBC_Graphics_set_bg_palette(graphics, GROUND_PALETTE, GBC_WHITE, GBC_BLACK, GBC_BLACK, GBC_GRAY);
    GBC_Graphics_set_bg_palette(graphics, TREE_ROAD_PALETTE, GBC_BLACK, GBC_BLACK, GBC_BLACK, GBC_GRAY);
    GBC_Graphics_set_bg_palette(graphics, ROAD_PALETTE, GBC_BLACK, GBC_BLACK, GBC_GRAY, GBC_WHITE);
#endif
}

void draw_road(GBC_Graphics *graphics, uint8_t road_vram_offset) {
    load_palettes(graphics);

    uint8_t ground_attrs = GBC_Graphics_attr_make(GROUND_PALETTE, 0, false, false, false);
    uint8_t road_attrs = GBC_Graphics_attr_make(ROAD_PALETTE, 0, false, false, false);

    // Fill in the base for the road
    for (uint8_t x = 0; x < LAYER_WIDTH; x++) { 
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, LAYER_HEIGHT - 4, road_vram_offset + HORIZON, ground_attrs);
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, LAYER_HEIGHT - 3, road_vram_offset + ROAD_TOP, road_attrs);
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, LAYER_HEIGHT - 2, road_vram_offset + ROAD_BOT, road_attrs);
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, LAYER_HEIGHT - 1, GROUND_TILE, ground_attrs);
    }

    // Place trees

    // Place cars
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "road generated");
}