#include "text.h"
#include "../util.h"

extern uint8_t text_vram_offset, sprites_vram_offset;

static void setup_text_palettes(GBC_Graphics *graphics) {
#if defined(PBL_COLOR)
    GBC_Graphics_set_bg_palette(graphics, BALLOON_TEXT_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorRedARGB8, GColorWhiteARGB8);
    GBC_Graphics_set_bg_palette(graphics, FUEL_TEXT_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorIslamicGreenARGB8, GColorWhiteARGB8);
#else
    GBC_Graphics_set_bg_palette(graphics, BALLOON_TEXT_PALETTE, GBC_WHITE, GBC_BLACK, GBC_GRAY, GBC_WHITE);
    GBC_Graphics_set_bg_palette(graphics, FUEL_TEXT_PALETTE, GBC_WHITE, GBC_BLACK, GBC_WHITE, GBC_WHITE);
#endif
}

static void clear_top_row(GBC_Graphics *graphics) {
    for (uint8_t x = 0; x < TILEMAP_WIDTH; x++) {
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, 0, BLANK_TILE, GBC_Graphics_attr_make(0, 0, false, false, true));
    }
}

void text_init(GBC_Graphics *graphics) {
    setup_text_palettes(graphics);
    clear_top_row(graphics);
}

void draw_text_at_location(GBC_Graphics *graphics, char *text, uint8_t x, uint8_t y, uint8_t palette_num) {
    uint8_t start_x = x;
    // size_t text_length = strlen(text);
    uint char_index = 0;
    char cur_char = text[char_index];
    while (cur_char != '\0') {
        GBC_Graphics_bg_set_tile_palette(graphics, x, y, palette_num);
        if (cur_char >= 'A' && cur_char <= 'Z') {
            GBC_Graphics_bg_set_tile(graphics, x, y, text_vram_offset + (cur_char - 'A'));
        } else if (cur_char >= '0' && cur_char <= '9') {
            GBC_Graphics_bg_set_tile(graphics, x, y, text_vram_offset + NUMBER_OFFSET + (cur_char - '0'));
        } else if (cur_char == ':') {
            GBC_Graphics_bg_set_tile(graphics, x, y, text_vram_offset + COLON_OFFSET);
        } else if (cur_char == '\n') {
            x = start_x;
            y++;
        } else if (cur_char == 'x') {
            GBC_Graphics_bg_set_tile(graphics, x, y, text_vram_offset + TIMES_OFFSET);
        }else if (cur_char == 'b') {
            GBC_Graphics_bg_set_tile(graphics, x, y, sprites_vram_offset + BALLOON_TILE);
            GBC_Graphics_bg_set_tile_palette(graphics, x, y, BALLOON_TEXT_PALETTE);
        }else if (cur_char == 'f') {
            GBC_Graphics_bg_set_tile(graphics, x, y, sprites_vram_offset + FUEL_TILE);
            GBC_Graphics_bg_set_tile_palette(graphics, x, y, FUEL_TEXT_PALETTE);
        } else {
            GBC_Graphics_bg_set_tile(graphics, x, y, BLANK_TILE);
        }
        x++;
        char_index++;
        cur_char = text[char_index];
    }
}