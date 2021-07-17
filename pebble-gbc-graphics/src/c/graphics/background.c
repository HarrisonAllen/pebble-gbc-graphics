#include "background.h"
#include "clouds.h"
#include "road.h"
#include "../util.h"

static uint8_t s_bg_anim_frame, s_window_anim_frame;
static bool s_animate_bg_tiles = false;
static bool s_animate_window_tiles = false;
static bool s_window_up = false;
static uint8_t s_bg_min_scroll_y, s_bg_max_scroll_y;

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
  for (uint8_t x = 0; x < LAYER_WIDTH; x++) {
    for (uint8_t y = 0; y < LAYER_HEIGHT; y++) {
      GBC_Graphics_bg_set_tile_and_attrs(graphics, x, y, SOLID_TILE_11, GBC_Graphics_attr_make(0, 0, false, false, false));
    }
  }
}

static void clear_window_layer(GBC_Graphics *graphics) {
  for (uint8_t x = 0; x < LAYER_WIDTH; x++) {
    for (uint8_t y = 0; y < LAYER_HEIGHT; y++) {
      GBC_Graphics_window_set_tile_and_attrs(graphics, x, y, SOLID_TILE_11, GBC_Graphics_attr_make(0, 0, false, false, false));
    }
  }
}

// Place the frame on the window layer and make the window layer visible
static void setup_window_layer(GBC_Graphics *graphics) {
  // Find the boundaries of the screen, in tiles
  uint8_t left_boundary = 0;
  uint8_t right_boundary = GBC_Graphics_get_screen_width(graphics) / TILE_WIDTH - 1;
  uint8_t upper_boundary = 0;
  uint8_t lower_boundary = 6;
  
  // These attributes set a tile to palette 1, vram bank 0, no x flip, no y flip, no priority over sprite
  uint8_t window_attrs = GBC_Graphics_attr_make(1, 0, false, false, false);

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
}

static void setup_bg_layer(GBC_Graphics *graphics) {
  // First, fill with clouds
  // then, draw the road
  // lastly, draw the score layer
  
  draw_clouds(graphics, cloud_vram_offset);
  draw_road(graphics, road_vram_offset);
}

void generate_new_game_background(GBC_Graphics *graphics) {
  clear_background_layer(graphics);
  setup_bg_layer(graphics);

  clear_window_layer(graphics);
  setup_window_layer(graphics);

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

void render_background(GBC_Graphics *graphics, uint32_t player_x, uint8_t player_y) {
  GBC_Graphics_bg_set_scroll_pos(graphics, (uint8_t)player_x, clamp_uint8_t(TILE_HEIGHT, player_y, LAYER_HEIGHT * TILE_HEIGHT - GBC_Graphics_get_screen_height(graphics)));
  GBC_Graphics_render(graphics);
}

void start_window_animation() {
  s_animate_window_tiles = true;
}