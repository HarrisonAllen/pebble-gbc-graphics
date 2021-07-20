#include "window.h"
#include "background.h"
#include "../util.h"

static uint8_t s_window_anim_frame; // The current frame of the window animation
static bool s_window_animating; // The animation status of the window
static GPoint s_anim_start_pos; // The start position of the animation
static GPoint s_anim_end_pos; // The end position of the animation

extern const uint8_t ease_in_out_percentages[]; // Want to grab the ease in, ease out animation sequence

/**
 * Set the window palette and hide the window
 * 
 * @param graphics The GBC_Graphics object for rendering
 */
static void setup_window_layer(GBC_Graphics *graphics) {
  /**
   * The colors are:
   *   1. Sky color
   *   2. Canvas color
   *   3. Border fill color
   *   4. Border outline color
   */
  #if defined(PBL_COLOR)
    GBC_Graphics_set_bg_palette(graphics, WINDOW_PALETTE, GColorPictonBlueARGB8, GColorPastelYellowARGB8, GColorWindsorTanARGB8, GColorBlackARGB8);
  #else
    GBC_Graphics_set_bg_palette(graphics, WINDOW_PALETTE, GBC_COLOR_WHITE, GBC_COLOR_WHITE, GBC_COLOR_GRAY, GBC_COLOR_BLACK);
  #endif

  // Hide the window layer
  GBC_Graphics_window_set_offset_pos(graphics, GBC_Graphics_get_screen_width(graphics), GBC_Graphics_get_screen_height(graphics));
}

void clear_window_layer(GBC_Graphics *graphics) {
  for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) {
    for (uint8_t y = 0; y < GBC_TILEMAP_HEIGHT; y++) {
      GBC_Graphics_window_set_tile_and_attrs(graphics, x, y, SOLID_TILE_00, GBC_Graphics_attr_make(WINDOW_PALETTE, 0, false, false, true));
    }
  }
}

void window_init(GBC_Graphics *graphics) {
    setup_window_layer(graphics);
    clear_window_layer(graphics);
}

void set_window_layer_frame(GBC_Graphics *graphics, GRect frame, GPoint origin) {
  // Determine the boundaries for this new frame
  uint8_t left_boundary = frame.origin.x;
  uint8_t right_boundary = frame.origin.x + frame.size.w;
  uint8_t upper_boundary = frame.origin.y;
  uint8_t lower_boundary = frame.origin.y + frame.size.h;
  
  uint8_t window_attrs = GBC_Graphics_attr_make(WINDOW_PALETTE, 0, false, false, true);

  // Draw the corners
  GBC_Graphics_window_set_tile_and_attrs(graphics, left_boundary, upper_boundary, WINDOW_FRAME_CORNER, window_attrs);

  GBC_Graphics_window_set_tile_and_attrs(graphics, right_boundary, upper_boundary, WINDOW_FRAME_CORNER, window_attrs);
  GBC_Graphics_window_set_tile_x_flip(graphics, right_boundary, upper_boundary, true); // flip the top right corner

  GBC_Graphics_window_set_tile_and_attrs(graphics, left_boundary, lower_boundary, WINDOW_FRAME_CORNER, window_attrs);
  GBC_Graphics_window_set_tile_y_flip(graphics, left_boundary, lower_boundary, true); // flip the bottom left corner

  GBC_Graphics_window_set_tile_and_attrs(graphics, right_boundary, lower_boundary, WINDOW_FRAME_CORNER, window_attrs);
  GBC_Graphics_window_set_tile_y_flip(graphics, right_boundary, lower_boundary, true); // flip the bottom right corner
  GBC_Graphics_window_set_tile_x_flip(graphics, right_boundary, lower_boundary, true); // flip the bottom right corner

  // Draw the top and bottom edges
  for (uint8_t x = left_boundary+1; x < right_boundary; x++) {
    GBC_Graphics_window_set_tile_and_attrs(graphics, x, upper_boundary, WINDOW_FRAME_HORIZONTAL, window_attrs);
    GBC_Graphics_window_set_tile_and_attrs(graphics, x, lower_boundary, WINDOW_FRAME_HORIZONTAL, window_attrs);
    GBC_Graphics_window_set_tile_y_flip(graphics, x, lower_boundary, true); // flip the bottom edge
  }

  // Draw the left and right edges
  for (uint8_t y = upper_boundary+1; y < lower_boundary; y++) {
    GBC_Graphics_window_set_tile_and_attrs(graphics, left_boundary, y, WINDOW_FRAME_VERTICAL, window_attrs);
    
    GBC_Graphics_window_set_tile_and_attrs(graphics, right_boundary, y, WINDOW_FRAME_VERTICAL, window_attrs);
    GBC_Graphics_window_set_tile_x_flip(graphics, right_boundary, y, true); // flip the bottom edge
  }

  // Fill the middle
  for (uint8_t x = left_boundary+1; x < right_boundary; x++) {
    for (uint8_t y = upper_boundary+1; y < lower_boundary; y++) {
      GBC_Graphics_window_set_tile_and_attrs(graphics, x, y, SOLID_TILE_01, window_attrs);
    }
  }

  // And move the window to the position
  GBC_Graphics_window_set_offset_x(graphics, origin.x);
  GBC_Graphics_window_set_offset_y(graphics, origin.y);
}

void start_window_animation(GPoint start, GPoint end) {
  s_anim_start_pos = start;
  s_anim_end_pos = end;
  s_window_animating = true;
  s_window_anim_frame = 0;
}

void step_window_animation(GBC_Graphics *graphics) {
  uint8_t animation_percent = ease_in_out_percentages[s_window_anim_frame];
  GBC_Graphics_window_set_offset_x(graphics, lerp(s_anim_start_pos.x, s_anim_end_pos.x, animation_percent));
  GBC_Graphics_window_set_offset_y(graphics, lerp(s_anim_start_pos.y, s_anim_end_pos.y, animation_percent));
  s_window_anim_frame++;
  s_window_animating = animation_percent < 100;
}

bool is_window_animating() {
  return s_window_animating;
}

void set_window_has_priority(GBC_Graphics *graphics, bool has_priority) {
  for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) {
    for (uint8_t y = 0; y < GBC_TILEMAP_HEIGHT; y++) {
      GBC_Graphics_window_set_tile_priority(graphics, x, y, has_priority);
    }
  }
}