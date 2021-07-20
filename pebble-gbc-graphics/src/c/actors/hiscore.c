#include "hiscore.h"
#include "../graphics/window.h"

static uint8_t s_offset_y;          // The current y offset relative to the window layer
static uint8_t s_color_frame;       // The frame to keep track of the current color
static uint8_t s_bob_frame;         // The frame to keep track of the up and down bob position
static Direction s_bob_direction;   // The direction the bob is currenly moving

/**
 * The colors that the sprites will shift through during animation
 */
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

extern uint8_t sprites_vram_offset; // Need to grab the offset of the sprites tilesheet
extern const uint8_t ease_in_out_percentages[]; // Also want to use this animation sequence

/**
 * Setup the sprites, palettes, and data
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
void hiscore_init(GBC_Graphics *graphics) {
    /**
     * The colors are:
     *   1. Transparent (doesn't matter)
     *   2. Shadow
     *   3. Primary color (shifts on color Pebbles, static black on b&w pebbles)
     *   4. Unused, but I set it to white anyways
     */
#if defined(PBL_COLOR)
    GBC_Graphics_set_sprite_palette(graphics, HI_SCORE_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorRedARGB8, GColorWhiteARGB8);
#else
    GBC_Graphics_set_sprite_palette(graphics, HI_SCORE_PALETTE, GBC_COLOR_WHITE, GBC_COLOR_GRAY, GBC_COLOR_BLACK, GBC_COLOR_WHITE);
#endif

    // Go ahead and set the sprites
    uint8_t hiscore_attrs = GBC_Graphics_attr_make(HI_SCORE_PALETTE, 0, false, false, false);
    GBC_Graphics_oam_set_sprite(graphics, HI_SCORE_OAM_OFFSET, 0, 0, sprites_vram_offset + HI_SCORE_SPRITE_1, hiscore_attrs);
    GBC_Graphics_oam_set_sprite(graphics, HI_SCORE_OAM_OFFSET + 1, 0, 0, sprites_vram_offset + HI_SCORE_SPRITE_2, hiscore_attrs);
    GBC_Graphics_oam_set_sprite(graphics, HI_SCORE_OAM_OFFSET + 2, 0, 0, sprites_vram_offset + HI_SCORE_SPRITE_3, hiscore_attrs);

    // Reset the data
    s_color_frame = 0;
    s_bob_frame = 0;
    s_bob_direction = D_DOWN;
}

/**
 * Draws the high score sprites at their corresponding offset
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
static void draw_hiscore(GBC_Graphics *graphics) {
    uint16_t screen_x = GBC_Graphics_window_get_offset_x(graphics) + HI_SCORE_X_POS;
    if (screen_x > GBC_Graphics_get_screen_width(graphics) + GBC_SPRITE_OFFSET_X) { // Hide when offscreen
        GBC_Graphics_oam_hide_sprite(graphics, HI_SCORE_OAM_OFFSET);
        GBC_Graphics_oam_hide_sprite(graphics, HI_SCORE_OAM_OFFSET + 1);
        GBC_Graphics_oam_hide_sprite(graphics, HI_SCORE_OAM_OFFSET + 2);
    } else {
        // Places the highscore relative to the window layer
        uint8_t screen_y = GBC_Graphics_window_get_offset_y(graphics) + HI_SCORE_Y_POS + s_offset_y;
        GBC_Graphics_oam_set_sprite_pos(graphics, HI_SCORE_OAM_OFFSET, screen_x, screen_y);
        GBC_Graphics_oam_set_sprite_pos(graphics, HI_SCORE_OAM_OFFSET + 1, screen_x + GBC_TILE_WIDTH, screen_y);
        GBC_Graphics_oam_set_sprite_pos(graphics, HI_SCORE_OAM_OFFSET + 2, screen_x + GBC_TILE_WIDTH * 2, screen_y);
    }
}

void hiscore_step(GBC_Graphics *graphics) {
    // Bob up and down using the ease in and ease out animation sequence
    uint8_t animation_percent = ease_in_out_percentages[s_bob_frame];
    if (s_bob_direction == D_UP) {
        s_offset_y = lerp(HI_SCORE_Y_OFFSET_MAX, 0, animation_percent);
    } else {
        s_offset_y = lerp(0, HI_SCORE_Y_OFFSET_MAX, animation_percent);
    }
    if (animation_percent == 100) { // Animation complete, start in other direction
        s_bob_frame = 0;
        s_bob_direction = s_bob_direction == D_UP ? D_DOWN : D_UP;
    } else {
        s_bob_frame++;
    }

    // On color Pebbles, shift the primary color through the rainbow palette colors
#if defined(PBL_COLOR)
    GBC_Graphics_set_sprite_palette_color(graphics, HI_SCORE_PALETTE, 2, rainbow_palette_colors[s_color_frame]);
    s_color_frame = (s_color_frame + 1) % sizeof(rainbow_palette_colors);
#endif

    // And don't forget to move or hide the sprites!
    draw_hiscore(graphics);
}