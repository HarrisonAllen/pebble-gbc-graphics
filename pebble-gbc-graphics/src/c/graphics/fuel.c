#include "fuel.h"
#include "../util.h"

extern uint8_t fuel_vram_offset;

void draw_fuel_bar(GBC_Graphics *graphics, uint16_t fuel_amount, uint16_t max_fuel, uint8_t num_fuel_bars, uint8_t x, uint8_t y) {
#if defined(PBL_COLOR)
    float fuel_percentage = (float)fuel_amount / max_fuel;
    uint8_t color;
    if (fuel_percentage <= 0.1) {
        color = GColorRedARGB8;
    } else if (fuel_percentage <= 0.3) {
        color = GColorChromeYellowARGB8;
    } else {
        color = GColorIslamicGreenARGB8;
    }
    GBC_Graphics_set_bg_palette_color(graphics, FUEL_BAR_PALETTE, 2, color);
    GBC_Graphics_set_bg_palette_color(graphics, FUEL_BAR_PALETTE, 3, color);
#else
    float fuel_percentage = (float)fuel_amount / max_fuel;
    uint8_t color;
    if (fuel_percentage <= 0.3) {
        color = GBC_COLOR_GRAY;
    } else {
        color = GBC_COLOR_BLACK;
    }
    GBC_Graphics_set_bg_palette_color(graphics, FUEL_BAR_PALETTE, 2, color);
#endif

    uint8_t num_pixels = (num_fuel_bars * 8) * fuel_amount / max_fuel;

    uint8_t num_full_bars = num_pixels / 8;
    uint8_t last_bar_len = num_pixels % 8;
    uint8_t attrs = GBC_Graphics_attr_make(FUEL_BAR_PALETTE, 0, false, false, true);
    uint8_t bar_pos;
    for (bar_pos = 0; bar_pos < num_full_bars; bar_pos++) {
        GBC_Graphics_bg_set_tile_and_attrs(graphics, x+bar_pos, y, fuel_vram_offset + FUEL_BAR_FULL, attrs);
    }
    if (bar_pos < num_fuel_bars) {
        if (num_full_bars == 0 && last_bar_len == 0 && fuel_amount != 0) {
            GBC_Graphics_bg_set_tile_and_attrs(graphics, x+bar_pos, y, fuel_vram_offset + FUEL_BAR_EMPTY - 1, attrs);
        } else {
            GBC_Graphics_bg_set_tile_and_attrs(graphics, x+bar_pos, y, fuel_vram_offset + FUEL_BAR_EMPTY - last_bar_len, attrs);
        }
    }
    if (bar_pos < num_fuel_bars) {
        for (bar_pos++; bar_pos < num_fuel_bars; bar_pos++) {
            GBC_Graphics_bg_set_tile_and_attrs(graphics, x+bar_pos, y, fuel_vram_offset + FUEL_BAR_EMPTY, attrs);
        }
    }
    GBC_Graphics_bg_set_tile_and_attrs(graphics, x+bar_pos, y, fuel_vram_offset + FUEL_BAR_END, attrs);
}