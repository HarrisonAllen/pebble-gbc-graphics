#include "background.h"
#include "clouds.h"
#include "road.h"
#include "text.h"
#include "../util.h"

static uint8_t s_bg_max_scroll_y; // The farthest the background layer can scroll without flowing over
static uint8_t s_bg_scroll_x;     // The x scroll position of the background
static uint8_t s_bg_scroll_y;     // The y scroll position of the background

extern uint8_t cloud_vram_offset, road_vram_offset; // Need the cloud and road VRAM offsets

/**
 * Replaces all of the tiles in the background with blank tiles
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
static void clear_background_layer(GBC_Graphics *graphics) {
    for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) {
        for (uint8_t y = 0; y < GBC_TILEMAP_HEIGHT; y++) {
            GBC_Graphics_bg_set_tile_and_attrs(graphics, x, y, SOLID_TILE_11, GBC_Graphics_attr_make(0, 0, false, false, false));
        }
    }
}

/**
 * Draws the clouds and road
 */
static void setup_bg_layer(GBC_Graphics *graphics) {
    draw_clouds(graphics, cloud_vram_offset);
    draw_road(graphics, road_vram_offset);
}

void generate_new_game_background(GBC_Graphics *graphics) {
    clear_background_layer(graphics);
    setup_bg_layer(graphics);

    s_bg_max_scroll_y = GBC_TILEMAP_HEIGHT * GBC_TILE_HEIGHT - GBC_Graphics_get_screen_height(graphics); // Dynamically set for different screens
}

/**
 * Scroll interrupt handler
 * @note By triggering a scroll interrupt, we're able to achieve cool effects
 * 
 * The first is rendering the score bar. The score bar is drawn on the top
 * left of the background tilemap. To show it, we move the background scroll
 * to (0, 0) when the line interrupt is zero. Once we've finished drawing the
 * score bar, we trigger another interrupt at line 8.
 * 
 * The other cool effect is a parallax effect (foreground moves faster than background)
 * I designed 4 separate sections to the background: the sky, the background trees,
 * the road, and the foreground trees:
 * -The sky, the farthest back, moves the slowest, moving at half of the player speed.
 * -The background trees, closer than the sky, moves at 2x the speed of the sky.
 * -The road moves 3x the speed of the sky, and 1.5x the speed of the background trees
 * -And lastly, the foreground trees move 4x the speed of the sky, and 1.33x the speed of the road.
 * 
 * To achieve this effect, I set an interrupt to fire at the top of each section. Once the interrupt fires,
 * I set the background scroll x to farther than the previous section, causing the corresponding section
 * to move across the screen faster.
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
static void handle_scroll_interrupt(GBC_Graphics *graphics) {
    int8_t tree_position = s_bg_scroll_y - (s_bg_max_scroll_y - GBC_TILE_HEIGHT * 5);   // The position of the trees on top of the road
    int8_t road_position = s_bg_scroll_y - (s_bg_max_scroll_y - GBC_TILE_HEIGHT * 3);   // The position of the top of the road
    int8_t bottom_position = s_bg_scroll_y - (s_bg_max_scroll_y - GBC_TILE_HEIGHT * 1); // The position of the trees below the road
    if (GBC_Graphics_stat_get_line_y_compare(graphics) == 0) {
        // Move to top left of background where the score bar resides
        GBC_Graphics_bg_set_scroll_pos(graphics, 0, 0);
    
        // Interrupt on the end of the score bar
        GBC_Graphics_stat_set_line_y_compare(graphics, 8); 
    } else if (GBC_Graphics_stat_get_line_y_compare(graphics) == 8) { 
        // Done drawing score bar, return to the current scroll position
        GBC_Graphics_bg_set_scroll_pos(graphics, s_bg_scroll_x, s_bg_scroll_y);
    
        if (tree_position > 0) { 
            // Top row of trees is visible, so we want to interrupt on that position
            GBC_Graphics_stat_set_line_y_compare(graphics, GBC_Graphics_get_screen_height(graphics) - tree_position);
        } else { 
            // No trees visible, interrupt on next pass to the score bar
            GBC_Graphics_stat_set_line_y_compare(graphics, 0);
        }
    } else { 
      // We're drawing one of the features on the bottom
        if (GBC_Graphics_stat_get_line_y_compare(graphics) == GBC_Graphics_get_screen_height(graphics) - tree_position) {
            // Move the top trees 2x as fast as the sky
            GBC_Graphics_bg_set_scroll_x(graphics, s_bg_scroll_x * 2);
        
            if (road_position > 0) {
                // Road is visible, interrupt on that position
                GBC_Graphics_stat_set_line_y_compare(graphics, GBC_Graphics_get_screen_height(graphics) - road_position);
            } else {
                // No road visible, interrupt on next pass to the score bar
                GBC_Graphics_stat_set_line_y_compare(graphics, 0);
            }
        } else if (GBC_Graphics_stat_get_line_y_compare(graphics) == GBC_Graphics_get_screen_height(graphics) - road_position) {
            // Move the road 3x as fast as the sky
            GBC_Graphics_bg_set_scroll_x(graphics, s_bg_scroll_x * 3);
        
            if (bottom_position > 0) {
                // Bottom trees are visible, interrupt on that position
                GBC_Graphics_stat_set_line_y_compare(graphics, GBC_Graphics_get_screen_height(graphics) - bottom_position);
            } else {
                // No road visible, interrupt on next pass to the score bar
                GBC_Graphics_stat_set_line_y_compare(graphics, 0);
            }
        } else {
            // Move the bottom trees 4x as fast as the sky
            GBC_Graphics_bg_set_scroll_x(graphics, s_bg_scroll_x * 4);
        
            // Interrupt on next pass to the score bar
            GBC_Graphics_stat_set_line_y_compare(graphics, 0);
        }
    } 
}

void render_background(GBC_Graphics *graphics, uint32_t player_x, uint8_t player_y) {
    // Here, I directly set the scroll pos based on the player pos.
    // However, you could use GBC_Graphics_bg_move() to move the background by an increment instead
    s_bg_scroll_y = clamp_int(0, player_y - (GBC_Graphics_get_screen_height(graphics) / 2), s_bg_max_scroll_y);
    s_bg_scroll_x = (uint8_t)(player_x / 2);

    // To enable a scroll interrupt, we have to set an interrupt callback, the line to interrupt on, then enable it
    GBC_Graphics_set_line_compare_interrupt_callback(graphics, handle_scroll_interrupt);
    GBC_Graphics_stat_set_line_y_compare(graphics, 0);
    GBC_Graphics_stat_set_line_compare_interrupt_enabled(graphics, true);
}

uint8_t get_bg_scroll_x() {
    return s_bg_scroll_x;
}

uint8_t get_bg_scroll_y() {
    return s_bg_scroll_y;
}