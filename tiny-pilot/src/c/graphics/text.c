#include "text.h"
#include "../util.h"

extern uint8_t text_vram_offset, sprites_vram_offset; // Need to grab the offset of the text and sprites tilesheets

/**
 * Initializes the background palettes for the balloon and fuel text icons
 */
static void setup_text_palettes(GBC_Graphics *graphics) {
    /**
     * The colors are:
     *   1. Sky color
     *   2. Border color
     *   3. Primary color
     *   4. Detail color
     */
#if defined(PBL_COLOR)
    GBC_Graphics_set_bg_palette(graphics, BALLOON_TEXT_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorRedARGB8, GColorWhiteARGB8);
    GBC_Graphics_set_bg_palette(graphics, FUEL_TEXT_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorIslamicGreenARGB8, GColorIslamicGreenARGB8);
#else
    GBC_Graphics_set_bg_palette(graphics, BALLOON_TEXT_PALETTE, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
    GBC_Graphics_set_bg_palette(graphics, FUEL_TEXT_PALETTE, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_WHITE);
#endif
}

void clear_top_row(GBC_Graphics *graphics) {
    for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) {
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, 0, BLANK_TILE, GBC_Graphics_attr_make(0, 0, false, false, true));
    }
}

void text_init(GBC_Graphics *graphics) {
    setup_text_palettes(graphics);
    clear_top_row(graphics);
}

void draw_text_at_location(GBC_Graphics *graphics, char *text, uint8_t x, uint8_t y, uint8_t palette_num, bool background) {
    uint8_t start_x = x;
    uint char_index = 0;
    char cur_char = text[char_index];
    uint8_t tile = 0;
    uint8_t palette;

    while (cur_char != '\0') { // Go until we reach the end of the char array
        palette = palette_num;
        if (cur_char >= 'A' && cur_char <= 'Z') {
            tile = text_vram_offset + (cur_char - 'A');
        } else if (cur_char >= '0' && cur_char <= '9') {
            tile = text_vram_offset + NUMBER_OFFSET + (cur_char - '0');
        } else if (cur_char == ':') {
            tile = text_vram_offset + COLON_OFFSET;
        } else if (cur_char == 'x') {
            tile = text_vram_offset + TIMES_OFFSET;
        }else if (cur_char == 'b') { // For balloon, use diferent palette
            tile = sprites_vram_offset + BALLOON_TILE;
            palette = BALLOON_TEXT_PALETTE;
        }else if (cur_char == 'f') { // For fuel, use different palette
            tile = sprites_vram_offset + FUEL_TILE;
            palette = FUEL_TEXT_PALETTE;
        } else {
            tile = BLANK_TILE;
        }

        if (cur_char == '\n') { // newline, don't render, just move to next line
            x = start_x;
            y++;
        } else {
            if (background) {
                GBC_Graphics_bg_set_tile_palette(graphics, x, y, palette);
                GBC_Graphics_bg_set_tile(graphics, x, y, tile);
            } else {
                GBC_Graphics_window_set_tile_palette(graphics, x, y, palette);
                GBC_Graphics_window_set_tile(graphics, x, y, tile);
            }
            x++;
        }
        char_index++;
        cur_char = text[char_index];
    }
}