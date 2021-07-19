#include "hiscore.h"
#include "../graphics/window.h"

static uint8_t s_screen_offset_y;
static uint8_t s_color_frame, s_bob_frame;
static Direction s_bob_direction;

const uint8_t rainbow_palette_colors[] = {
  GColorRedARGB8,
  GColorOrangeARGB8,
  GColorChromeYellowARGB8,
  GColorYellowARGB8,
  GColorSpringBudARGB8,
  GColorBrightGreenARGB8,
  GColorGreenARGB8,
  GColorMalachiteARGB8,
  GColorMediumSpringGreenARGB8,
  GColorCyanARGB8,
  GColorVividCeruleanARGB8,
  GColorBlueMoonARGB8,
  GColorBlueARGB8,
  GColorElectricUltramarineARGB8,
  GColorVividVioletARGB8,
  GColorMagentaARGB8,
  GColorFashionMagentaARGB8,
  GColorFollyARGB8
};

extern uint8_t sprites_vram_offset;
extern const uint8_t ease_in_out_percentages[];

void hiscore_init(GBC_Graphics *graphics) {
#if defined(PBL_COLOR)
    GBC_Graphics_set_sprite_palette(graphics, HI_SCORE_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorRedARGB8, GColorWhiteARGB8);
#else
    GBC_Graphics_set_sprite_palette(graphics, HI_SCORE_PALETTE, GBC_WHITE, GBC_GRAY, GBC_BLACK, GBC_WHITE);
#endif

    GBC_Graphics_oam_set_sprite(graphics, HI_SCORE_OFFSET, 0, 0, sprites_vram_offset + HI_SCORE_SPRITE_1, GBC_Graphics_attr_make(HI_SCORE_PALETTE, 0, false, false, false));
    GBC_Graphics_oam_set_sprite(graphics, HI_SCORE_OFFSET + 1, 0, 0, sprites_vram_offset + HI_SCORE_SPRITE_2, GBC_Graphics_attr_make(HI_SCORE_PALETTE, 0, false, false, false));
    GBC_Graphics_oam_set_sprite(graphics, HI_SCORE_OFFSET + 2, 0, 0, sprites_vram_offset + HI_SCORE_SPRITE_3, GBC_Graphics_attr_make(HI_SCORE_PALETTE, 0, false, false, false));

    s_color_frame = 0;
    s_bob_frame = 0;
    s_bob_direction = D_DOWN;
}

static void draw_hiscore(GBC_Graphics *graphics) {
    uint16_t screen_x = GBC_Graphics_window_get_offset_x(graphics) + HI_SCORE_X_POS;
    if (screen_x > GBC_Graphics_get_screen_width(graphics) + SPRITE_OFFSET_X) {
        GBC_Graphics_oam_hide_sprite(graphics, HI_SCORE_OFFSET);
        GBC_Graphics_oam_hide_sprite(graphics, HI_SCORE_OFFSET + 1);
        GBC_Graphics_oam_hide_sprite(graphics, HI_SCORE_OFFSET + 2);
    } else {
        uint8_t screen_y = GBC_Graphics_window_get_offset_y(graphics) + HI_SCORE_Y_POS + s_screen_offset_y;
        GBC_Graphics_oam_set_sprite_pos(graphics, HI_SCORE_OFFSET, screen_x, screen_y);
        GBC_Graphics_oam_set_sprite_pos(graphics, HI_SCORE_OFFSET + 1, screen_x + 8, screen_y);
        GBC_Graphics_oam_set_sprite_pos(graphics, HI_SCORE_OFFSET + 2, screen_x + 16, screen_y);
    }
}

void hiscore_step(GBC_Graphics *graphics) {
    uint8_t animation_percent = ease_in_out_percentages[s_bob_frame];
    if (s_bob_direction == D_UP) {
        s_screen_offset_y = lerp(HI_SCORE_Y_OFFSET_MAX, 0, animation_percent);
    } else {
        s_screen_offset_y = lerp(0, HI_SCORE_Y_OFFSET_MAX, animation_percent);
    }
    if (animation_percent == 100) {
        s_bob_frame = 0;
        s_bob_direction = s_bob_direction == D_UP ? D_DOWN : D_UP;
    } else {
        s_bob_frame++;
    }

#if defined(PBL_COLOR)
    GBC_Graphics_set_sprite_palette_color(graphics, HI_SCORE_PALETTE, 2, rainbow_palette_colors[s_color_frame]);
    s_color_frame = (s_color_frame + 1) % sizeof(rainbow_palette_colors);
#endif

    draw_hiscore(graphics);
}

void hiscore_hide(GBC_Graphics *graphics) {
    GBC_Graphics_oam_hide_sprite(graphics, HI_SCORE_OFFSET);
    GBC_Graphics_oam_hide_sprite(graphics, HI_SCORE_OFFSET + 1);
    GBC_Graphics_oam_hide_sprite(graphics, HI_SCORE_OFFSET + 2);
}