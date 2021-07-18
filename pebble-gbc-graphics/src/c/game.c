#include "game.h"
#include "graphics/background.h"
#include "graphics/text.h"
#include "actors/player.h"
#include "actors/items.h"
#include "util.h"

static AppTimer *s_frame_timer; // Timer to animate the frames
static GBC_Graphics *s_graphics; // Need to keep a reference to the graphics object
static uint16_t player_score;
static uint8_t player_fuel;

static void game_step() {
  player_step(s_graphics);
  render_background(s_graphics, get_player_x(), get_player_y());
  items_step(s_graphics);
  animate_graphics(s_graphics);

  // TODO: Add collision, only on the body and nose of the plane

  char top_banner_text[33];
  snprintf(top_banner_text, 32, "bx001 f  ");
  draw_text_at_location(s_graphics, top_banner_text, PBL_IF_ROUND_ELSE(8, 5), 0, 0);
}

static void frame_timer_handle(void* context) {
  s_frame_timer = app_timer_register(FRAME_DURATION, frame_timer_handle, NULL);
  game_step();
}

static void will_focus_handler(bool in_focus) {
  if (!in_focus) {
    // If a notification pops up while the timer is firing
    // very rapidly, it will crash the entire watch :)
    // Stopping the timer when a notification appears will prevent this
    if (s_frame_timer != NULL) {
      app_timer_cancel(s_frame_timer);
    }
  } else {
    if (s_frame_timer != NULL) {
      s_frame_timer = app_timer_register(FRAME_DURATION, frame_timer_handle, NULL);
    }
  }
}

// Initialize the GBC_Graphics object
GBC_Graphics *init_gbc_graphics(Window *window) {
  // Create with 1 vram bank since that's all the space we'll need
  GBC_Graphics *graphics = GBC_Graphics_ctor(window, 1);

  // This will fill a good portion of the screen on all pebbles, and
  // have a decent frame rate. I prefer SCREEN_BOUNDS_SQUARE for the best
  // compromise between size and frame rate. You can also define your own bounds!
  GBC_Graphics_set_screen_bounds(graphics, SCREEN_BOUNDS_LARGE);
  
  // One OAM slot will use two tiles
  GBC_Graphics_lcdc_set_8x16_sprite_mode_enabled(graphics, true);

  load_tilesheets(graphics);

  GBC_Graphics_render(graphics);

  return graphics;
}

void game_init(Window *window) {
  s_graphics = init_gbc_graphics(window);
  generate_new_game_background(s_graphics);
  s_frame_timer = app_timer_register(FRAME_DURATION, frame_timer_handle, NULL);
  app_focus_service_subscribe(will_focus_handler);

  player_init(s_graphics);
  render_background(s_graphics, get_player_x(), get_player_y());
  items_init(s_graphics);
  text_init(s_graphics);
}

void game_deinit() {
  GBC_Graphics_destroy(s_graphics);
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  start_window_animation();
}

void up_press_handler(ClickRecognizerRef recognizer, void *context) {
  player_set_vertical_direction(D_UP);
}

void up_release_handler(ClickRecognizerRef recognizer, void *context) {
  player_set_vertical_direction(D_NONE);
}

void down_press_handler(ClickRecognizerRef recognizer, void *context) {
  player_set_vertical_direction(D_DOWN);
}

void down_release_handler(ClickRecognizerRef recognizer, void *context) {
  player_set_vertical_direction(D_NONE);
}