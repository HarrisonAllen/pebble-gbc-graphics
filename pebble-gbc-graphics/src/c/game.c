#include "game.h"
#include "graphics/background.h"
#include "graphics/text.h"
#include "graphics/fuel.h"
#include "actors/player.h"
#include "actors/items.h"
#include "util.h"

static AppTimer *s_frame_timer; // Timer to animate the frames
static GBC_Graphics *s_graphics; // Need to keep a reference to the graphics object
static uint16_t s_player_score;
static uint16_t s_player_fuel = MAX_PLAYER_FUEL;

static void check_collision() {
  for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
    bool item_overlaps_player = grects_overlap(get_player_collision(), get_item_collision(i));
    if (item_overlaps_player) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Overlap! Item %d: Player (%d, %d), Item (%d, %d)", i, get_player_collision().origin.x, get_player_collision().origin.y, get_item_collision(i).origin.x, get_item_collision(i).origin.y);
      uint *item = get_item(i);
      switch(item[0]) {
        case BALLOON_ID:
          s_player_score++;
          break;
        case FUEL_ID:
          s_player_fuel += MAX_PLAYER_FUEL / NUM_FUEL_BARS;
          break;
        default:
          break;
      }
      handle_collision(s_graphics, i);
    }
  }
}

static void update_fuel() {
  if (s_player_fuel > 0) {
    s_player_fuel--;
  }
  if (s_player_fuel > MAX_PLAYER_FUEL) {
    s_player_fuel = MAX_PLAYER_FUEL;
  }
  draw_fuel_bar(s_graphics, s_player_fuel, MAX_PLAYER_FUEL, NUM_FUEL_BARS, SCORE_BAR_OFFSET + 8, 0);
}

static void game_step() {
  player_step(s_graphics);
  render_background(s_graphics, get_player_x(), get_player_y());
  items_step(s_graphics);
  animate_graphics(s_graphics);

  check_collision();

  char top_banner_text[33];
  snprintf(top_banner_text, 32, "bx%04d f", s_player_score);
  draw_text_at_location(s_graphics, top_banner_text, SCORE_BAR_OFFSET, 0, 0);

  update_fuel();

  // TODO:
  // - End game by having plane slide out to the left, window slide in from the right
  // - On the window, write "GAME OVER\nSCORE: 123\nHIGH SCORE: 1234"
  // - "PRESS SELECT TO START AGAIN"
  // - Reload the game 
  // - Have a start to the game, where the window is chilling and it says "PRESS SELECT TO BEGIN\nHIGH SCORE: 1234"
  // - And the plane is also chilling there, vibing
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

  // I have predefined some common screen sizes, prefixed by SCREEN_BOUNDS_,
  // but you can create your own with any resolution!
  // Keep in mind that drawing fewer pixels (i.e. smaller resolutions) will
  // lead to some serious performance gains
#if defined(PBL_ROUND)
  // For round, I want to make sure the score bar is visible, so I use
  // the large screen bounds and scoot it down. In main.c, I set the
  // window color to the same blue as the sky, in order to give the
  // illusion of a fullscreen application
  GRect round_bounds = SCREEN_BOUNDS_LARGE;
  round_bounds.origin.y += 18;
  GBC_Graphics_set_screen_bounds(graphics, round_bounds);
#else
  // For rectangle watches, we can use fullscreen bounds since
  // no readability is lost
  GBC_Graphics_set_screen_bounds(graphics, SCREEN_BOUNDS_FULL);
#endif
  
  // This means one sprite uses two tiles, stacked vertically
  // The Legend of Zelda: Link's Awakening uses this 8x16 mode,
  // meaning Link's 16x16 character model only uses 2 sprite slots
  // Pokemon, however, uses the 8x8 mode and uses 4 sprites slots
  // to render the 16x16 player model
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