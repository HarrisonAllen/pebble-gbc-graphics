#include <pebble.h>
#include "pebble-gbc-graphics-advanced/pebble-gbc-graphics-advanced.h"

#define NUMBER_OF_VRAM_BANKS_TO_GENERATE 1
#define NUMBER_OF_TILES 16
#define NUMBER_OF_PALETTES 8

static Window *s_window;
static GBC_Graphics *s_gbc_graphics;

/**
 * Loads a tilesheet from the resources into a VRAM bank
 */
static void load_tilesheet() {
    // Calculate how many tiles are on the tilesheet
    ResHandle handle = resource_get_handle(RESOURCE_ID_DATA_SAMPLE_TILESHEET);
    size_t res_size = resource_size(handle);
    uint16_t tiles_to_load = res_size / GBC_TILE_NUM_BYTES;

    uint8_t tilesheet_start_offset = 0; // This is the tile on the tilesheet we want to start loading from
    uint8_t vram_start_offset = 0; // This is the tile in the VRAM we want to start loading into
    uint8_t vram_bank = 0; // The VRAM bank we want to store the tiles into
    GBC_Graphics_load_from_tilesheet_into_vram(s_gbc_graphics, RESOURCE_ID_DATA_SAMPLE_TILESHEET, tilesheet_start_offset, 
                                                tiles_to_load, vram_start_offset, vram_bank);
}

/**
 * Sets palettes for the background in various gradients. You may find this link helpful: https://developer.rebble.io/developer.pebble.com/guides/tools-and-resources/color-picker/index.html
 */
static void create_palettes() {
#if defined(PBL_COLOR) // Pebbles with color screens use the ARGB8 Pebble color definitions for palettes (which are just bytes)
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 0, 16, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 1, 16, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 2, 16, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 3, 16, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 4, 16, GColorBlackARGB8, GColorDarkGrayARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorBulgarianRoseARGB8, GColorDarkCandyAppleRedARGB8, GColorRedARGB8, GColorArmyGreenARGB8, GColorLimerickARGB8, GColorYellowARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8, GColorGreenARGB8, GColorMidnightGreenARGB8, GColorTiffanyBlueARGB8, GColorCyanARGB8);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 5, 16, GColorBlackARGB8, GColorOxfordBlueARGB8, GColorDukeBlueARGB8, GColorBlueARGB8, GColorImperialPurpleARGB8, GColorPurpleARGB8, GColorMagentaARGB8, GColorBulgarianRoseARGB8, GColorDarkCandyAppleRedARGB8, GColorRedARGB8, GColorArmyGreenARGB8, GColorLimerickARGB8, GColorYellowARGB8, GColorDarkGreenARGB8, GColorIslamicGreenARGB8, GColorGreenARGB8);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 6, 16, GColorBlackARGB8, GColorDarkGrayARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorLightGrayARGB8, GColorWhiteARGB8, GColorBlackARGB8, GColorDarkGrayARGB8, GColorLightGrayARGB8, GColorWhiteARGB8);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 7, 16, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8, GColorWhiteARGB8, GColorLightGrayARGB8, GColorDarkGrayARGB8, GColorBlackARGB8);
#else // Pebbles with black and white screens use the GBC_COLOR definitions for palettes
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 0, 16, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 1, 16, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 2, 16, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 3, 16, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 4, 16, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 5, 16, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 6, 16, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 7, 16, GBC_COLOR_WHITE, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_BLACK, GBC_COLOR_WHITE, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_BLACK, GBC_COLOR_WHITE, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_BLACK, GBC_COLOR_WHITE, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_BLACK);
#endif
}

/**
 * Sets the background tiles in a test pattern to demonstrate palettes
 */
static void generate_background() {
    for (uint8_t y = 0; y < GBC_TILEMAP_HEIGHT; y++) {
        uint8_t tile_palette = y % NUMBER_OF_PALETTES;
        for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) {
            uint8_t tile_number = x % NUMBER_OF_TILES;
            GBC_Graphics_bg_set_tile(s_gbc_graphics, x, y, tile_number);
            GBC_Graphics_bg_set_tile_palette(s_gbc_graphics, x, y, tile_palette);
        }
    }
}

/**
 * Hides the window layer from view by pushing it off of the screen
 */
static void hide_window_layer() {
    GBC_Graphics_window_set_offset_y(s_gbc_graphics, GBC_Graphics_get_screen_height(s_gbc_graphics));
}

/**
 * Execute all of the graphics functions
 */
static void window_load(Window *window) {
    // Create the GBC_Graphics object
    s_gbc_graphics = GBC_Graphics_ctor(s_window, NUMBER_OF_VRAM_BANKS_TO_GENERATE);

    load_tilesheet();
    create_palettes();
    generate_background();
    hide_window_layer();

    // Display the graphics
    GBC_Graphics_render(s_gbc_graphics);
}

static void window_unload(Window *window) {
    // Destroy the GBC_Graphics object
    GBC_Graphics_destroy(s_gbc_graphics);
}

static void init(void) {
    s_window = window_create();

    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    window_stack_push(s_window, animated);
}

static void deinit(void) {
    window_destroy(s_window);
}

int main(void) {
    init();

    app_event_loop();
    deinit();
}
