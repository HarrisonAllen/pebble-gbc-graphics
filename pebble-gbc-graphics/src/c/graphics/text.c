#include "text.h"
#include "../util.h"

extern uint8_t text_vram_offset;

void clear_top_row(GBC_Graphics *graphics) {
    for (uint8_t x = 0; x < TILEMAP_WIDTH; x++) {
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x, 0, BLANK_TILE, GBC_Graphics_attr_make(0, 0, false, false, false));
    }
}

void draw_text_at_location(GBC_Graphics *graphics, char *text, uint8_t x, uint8_t y, uint8_t palette_num) {
    uint8_t start_x = x;
    // size_t text_length = strlen(text);
    uint char_index = 0;
    char cur_char = text[char_index];
    while (cur_char != '\0') {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Char %c", cur_char);
        if (cur_char >= 'A' && cur_char <= 'Z') {
            GBC_Graphics_bg_set_tile(graphics, x, y, text_vram_offset + (cur_char - 'A'));
        } else if (cur_char >= '0' && cur_char <= '9') {
            GBC_Graphics_bg_set_tile(graphics, x, y, text_vram_offset + NUMBER_OFFSET + (cur_char - '0'));
        } else if (cur_char == ':') {
            GBC_Graphics_bg_set_tile(graphics, x, y, text_vram_offset + COLON_OFFSET);
        } else if (cur_char == '\n') {
            x = start_x;
            y++;
        } else {
            GBC_Graphics_bg_set_tile(graphics, x, y, BLANK_TILE);
        }
        GBC_Graphics_bg_set_tile_palette(graphics, x, y, palette_num);
        x++;
        char_index++;
        cur_char = text[char_index];
    }
}