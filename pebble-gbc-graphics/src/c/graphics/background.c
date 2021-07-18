#include "background.h"
#include "clouds.h"
#include "road.h"
#include "text.h"
#include "../util.h"

static uint8_t s_bg_anim_frame, s_window_anim_frame;
static bool s_animate_bg_tiles = false;
static bool s_animate_window_tiles = false;
static bool s_window_up = false;
static uint8_t s_bg_min_scroll_y, s_bg_max_scroll_y;
static uint8_t s_bg_scroll_x, s_bg_scroll_y;

extern uint8_t base_vram_offset, cloud_vram_offset, fuel_vram_offset, road_vram_offset, sprites_vram_offset, text_vram_offset;

static const uint8_t s_window_step_sizes[] = {
  1,
  1,
  2,
  4,
  5,
  6,
  6,
  6,
  6,
  6,
  5,
  4,
  2,
  1,
  1
};

// Set the palettes we want to use
static void setup_palettes(GBC_Graphics *graphics) {
  // BW palettes are different from color palettes
  #if defined(PBL_COLOR)
    // For color palettes, you can either use the GColorARGB8 definitions, or manually enter an argb8 number (e.g. 0b11011011)
    // GBC_Graphics_set_bg_palette(graphics, 0, GColorBlackARGB8, GColorDarkGrayARGB8, GColorLightGrayARGB8, GColorWhiteARGB8);
    // GBC_Graphics_set_bg_palette_array(graphics, 0, cloud_palette);
    GBC_Graphics_set_bg_palette(graphics, 1, GColorBlackARGB8, GColorWhiteARGB8, GColorWindsorTanARGB8, GColorPastelYellowARGB8);
    GBC_Graphics_set_bg_palette(graphics, 2, GColorDukeBlueARGB8, GColorBlueARGB8, GColorBlueMoonARGB8, GColorWhiteARGB8);
    GBC_Graphics_set_bg_palette(graphics, 3, GColorDarkCandyAppleRedARGB8, GColorRedARGB8, GColorSunsetOrangeARGB8, GColorWhiteARGB8);
  #else
    // For black and white palettes, you get two options: GBC_BLACK or GBC_WHITE
    // GBC_Graphics_set_bg_palette_array(graphics, 0, cloud_palette);
    GBC_Graphics_set_bg_palette(graphics, 1, GBC_BLACK, GBC_BLACK, GBC_WHITE, GBC_WHITE);
    GBC_Graphics_set_bg_palette(graphics, 2, GBC_BLACK, GBC_WHITE, GBC_BLACK, GBC_WHITE);
    GBC_Graphics_set_bg_palette(graphics, 2, GBC_BLACK, GBC_BLACK, GBC_WHITE, GBC_WHITE);
  #endif
}

static void clear_background_layer(GBC_Graphics *graphics) {
  for (uint8_t x = 0; x < TILEMAP_WIDTH; x++) {
    for (uint8_t y = 0; y < TILEMAP_HEIGHT; y++) {
      GBC_Graphics_bg_set_tile_and_attrs(graphics, x, y, SOLID_TILE_11, GBC_Graphics_attr_make(0, 0, false, false, false));
    }
  }
}

static void clear_window_layer(GBC_Graphics *graphics) {
  for (uint8_t x = 0; x < TILEMAP_WIDTH; x++) {
    for (uint8_t y = 0; y < TILEMAP_HEIGHT; y++) {
      GBC_Graphics_window_set_tile_and_attrs(graphics, x, y, SOLID_TILE_11, GBC_Graphics_attr_make(0, 0, false, false, false));
    }
  }
}

// Place the frame on the window layer and make the window layer visible
static void setup_window_layer(GBC_Graphics *graphics) {
  #if defined(PBL_COLOR)
    GBC_Graphics_set_bg_palette(graphics, WINDOW_PALETTE, GColorBlackARGB8, GColorWhiteARGB8, GColorWindsorTanARGB8, GColorPastelYellowARGB8);
  #else
    GBC_Graphics_set_bg_palette(graphics, WINDOW_PALETTE, GBC_BLACK, GBC_BLACK, GBC_WHITE, GBC_WHITE);
  #endif

  // Find the boundaries of the screen, in tiles
  uint8_t left_boundary = 0;
  uint8_t right_boundary = (GBC_Graphics_get_screen_width(graphics) / 2) / TILE_WIDTH - 1;
  uint8_t upper_boundary = 0;
  uint8_t lower_boundary = 6;
  
  // These attributes set a tile to palette 1, vram bank 0, no x flip, no y flip, no priority over sprite
  uint8_t window_attrs = GBC_Graphics_attr_make(WINDOW_PALETTE, 0, false, false, false);

  // Draw the corners
  GBC_Graphics_window_set_tile_and_attrs(graphics, left_boundary, upper_boundary, WINDOW_FRAME_CORNER, window_attrs);
  GBC_Graphics_window_set_tile_and_attrs(graphics, right_boundary, upper_boundary, WINDOW_FRAME_CORNER, window_attrs);
  GBC_Graphics_window_set_tile_x_flip(graphics, right_boundary, upper_boundary, true); // flip the top right corner
  GBC_Graphics_window_set_tile_and_attrs(graphics, left_boundary, lower_boundary, WINDOW_FRAME_CORNER, window_attrs);
  GBC_Graphics_window_set_tile_y_flip(graphics, left_boundary, lower_boundary, true); // flip the bottom left corner
  GBC_Graphics_window_set_tile_and_attrs(graphics, right_boundary, lower_boundary, WINDOW_FRAME_CORNER, window_attrs);
  GBC_Graphics_window_set_tile_y_flip(graphics, right_boundary, lower_boundary, true); // flip the bottom right corner
  GBC_Graphics_window_set_tile_x_flip(graphics, right_boundary, lower_boundary, true); // flip the bottom right corner

  // Draw the edges
  for (uint8_t x = left_boundary+1; x < right_boundary; x++) {
    GBC_Graphics_window_set_tile_and_attrs(graphics, x, upper_boundary, WINDOW_FRAME_HORIZONTAL, window_attrs);
    GBC_Graphics_window_set_tile_and_attrs(graphics, x, lower_boundary, WINDOW_FRAME_HORIZONTAL, window_attrs);
    GBC_Graphics_window_set_tile_y_flip(graphics, x, lower_boundary, true); // flip the bottom edge
  }
  for (uint8_t y = upper_boundary+1; y < lower_boundary; y++) {
    GBC_Graphics_window_set_tile_and_attrs(graphics, left_boundary, y, WINDOW_FRAME_VERTICAL, window_attrs);
    GBC_Graphics_window_set_tile_and_attrs(graphics, right_boundary, y, WINDOW_FRAME_VERTICAL, window_attrs);
    GBC_Graphics_window_set_tile_x_flip(graphics, right_boundary, y, true); // flip the bottom edge
  }

  // Fill the middle
  for (uint8_t x = left_boundary+1; x < right_boundary; x++) {
    for (uint8_t y = upper_boundary+1; y < lower_boundary; y++) {
      GBC_Graphics_window_set_tile_and_attrs(graphics, x, y, SOLID_TILE_11, window_attrs);
    }
  }

  // And now we hide the window layer by setting it to the edge of the screen
  GBC_Graphics_window_set_offset_y(graphics, GBC_Graphics_get_screen_height(graphics));
  GBC_Graphics_window_set_offset_x(graphics, GBC_Graphics_get_screen_width(graphics) / 2);
}

static void setup_bg_layer(GBC_Graphics *graphics) {
  // First, fill with clouds
  // then, draw the road
  // lastly, draw the score layer
  
  draw_clouds(graphics, cloud_vram_offset);
  clear_top_row(graphics);
  draw_road(graphics, road_vram_offset);
}

void generate_new_game_background(GBC_Graphics *graphics) {
  clear_background_layer(graphics);
  setup_bg_layer(graphics);

  clear_window_layer(graphics);
  setup_window_layer(graphics);

  s_bg_max_scroll_y = TILEMAP_HEIGHT * TILE_HEIGHT - GBC_Graphics_get_screen_height(graphics);

  GBC_Graphics_render(graphics);
}

static void animate_window(GBC_Graphics *graphics) {
  int window_step;
  if (s_window_up) { // Window is up, so it should be going down
    window_step = s_window_step_sizes[s_window_anim_frame];
  } else { // Window is down, so it should be going up
    window_step = -s_window_step_sizes[s_window_anim_frame];
  }
  GBC_Graphics_window_move(graphics, 0, window_step);
  s_window_anim_frame = (s_window_anim_frame + 1) % sizeof(s_window_step_sizes);
  if (s_window_anim_frame == 0) {
    s_window_up = !s_window_up;
    s_animate_window_tiles = false;
  }

  GBC_Graphics_render(graphics);
}


static void handle_scroll_interrupt(GBC_Graphics *graphics) {
  int8_t tree_position = s_bg_scroll_y - (s_bg_max_scroll_y - TILE_HEIGHT * 5);
  int8_t road_position = s_bg_scroll_y - (s_bg_max_scroll_y - TILE_HEIGHT * 3);
  int8_t bottom_position = s_bg_scroll_y - (s_bg_max_scroll_y - TILE_HEIGHT * 1);
  if (GBC_Graphics_stat_get_line_y_compare(graphics) == 0) {
    GBC_Graphics_bg_set_scroll_pos(graphics, 0, 0);
    GBC_Graphics_stat_set_line_y_compare(graphics, 8);
  } else if (GBC_Graphics_stat_get_line_y_compare(graphics) == 8) {
    GBC_Graphics_bg_set_scroll_pos(graphics, s_bg_scroll_x, s_bg_scroll_y);
    if (tree_position > 0) {
      GBC_Graphics_stat_set_line_y_compare(graphics, GBC_Graphics_get_screen_height(graphics) - tree_position);
    } else {
      GBC_Graphics_stat_set_line_y_compare(graphics, 0);
    }
  } else {
    if (GBC_Graphics_stat_get_line_y_compare(graphics) == GBC_Graphics_get_screen_height(graphics) - tree_position) {
      GBC_Graphics_bg_set_scroll_x(graphics, s_bg_scroll_x * 2);
      if (road_position > 0) {
        GBC_Graphics_stat_set_line_y_compare(graphics, GBC_Graphics_get_screen_height(graphics) - road_position);
      } else {
        GBC_Graphics_stat_set_line_y_compare(graphics, 0);
      }
    } else if (GBC_Graphics_stat_get_line_y_compare(graphics) == GBC_Graphics_get_screen_height(graphics) - road_position) {
      GBC_Graphics_bg_set_scroll_x(graphics, s_bg_scroll_x * 3);
      if (bottom_position > 0) {
        GBC_Graphics_stat_set_line_y_compare(graphics, GBC_Graphics_get_screen_height(graphics) - bottom_position);
      } else {
        GBC_Graphics_stat_set_line_y_compare(graphics, 0);
      }
    } else {
      GBC_Graphics_bg_set_scroll_x(graphics, s_bg_scroll_x * 4);
      GBC_Graphics_stat_set_line_y_compare(graphics, 0);
    }
  } 
}

void render_background(GBC_Graphics *graphics, uint player_x, uint8_t player_y) {
  // Here, I explicity set the scroll pos based on the player pos.
  // However, you could use GBC_Graphics_bg_move() to move the background, like I do with the window
  s_bg_scroll_y = clamp_int(0, player_y - (GBC_Graphics_get_screen_height(graphics) / 2), s_bg_max_scroll_y);
  s_bg_scroll_x = (uint8_t)player_x;

  
  GBC_Graphics_set_line_compare_interrupt_callback(graphics, handle_scroll_interrupt);
  GBC_Graphics_stat_set_line_compare_interrupt_enabled(graphics, true);

  GBC_Graphics_render(graphics);
}

void animate_graphics(GBC_Graphics *graphics) {
  if (s_animate_window_tiles) {
    animate_window(graphics);
  }
}

void start_window_animation() {
  s_animate_window_tiles = true;
}