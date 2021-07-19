#include "game.h"
#include "graphics/background.h"
#include "graphics/text.h"
#include "graphics/fuel.h"
#include "graphics/window.h"
#include "actors/player.h"
#include "actors/items.h"
#include "util.h"

static AppTimer *s_frame_timer; // Timer to animate the frames
static GBC_Graphics *s_graphics; // Need to keep a reference to the graphics object
static uint16_t s_player_score;
static uint16_t s_player_fuel;
static uint8_t s_game_frame = 0;
static GameState s_game_state;
static uint16_t s_high_score = 12;

static void check_collision() {
  for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
    bool item_overlaps_player = grects_overlap(get_player_collision(), get_item_collision(i));
    if (item_overlaps_player) {
      uint *item = get_item(i);
      switch(item[0]) {
        case IT_BALLOON:
          s_player_score++;
          break;
        case IT_FUEL:
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
  draw_fuel_bar(s_graphics, s_player_fuel, MAX_PLAYER_FUEL, NUM_FUEL_BARS, SCORE_BAR_OFFSET + 1, 0);
}

static void game_step() {
  switch (s_game_state) {
    case GS_LOAD_IN_TRANSITION:
      player_step(s_graphics);
      render_background(s_graphics, get_player_x(), get_player_y());

      if (!is_window_animating()) {
        s_game_state = GS_NEW_GAME;
      } else {
        step_window_animation(s_graphics);
      }
      break;
    case GS_NEW_GAME:
      player_step(s_graphics);
      render_background(s_graphics, get_player_x(), get_player_y());
      break;
    case GS_NEW_GAME_TRANSITION:
      player_step(s_graphics);
      render_background(s_graphics, get_player_x(), get_player_y());

      if (!is_window_animating()) {
        s_game_state = GS_PLAYING;
        items_init(s_graphics);
      } else {
        step_window_animation(s_graphics);
      }
      break;
    case GS_PLAYING:
      player_step(s_graphics);
      render_background(s_graphics, get_player_x(), get_player_y());
      items_step(s_graphics);

      check_collision();

      char top_banner_text[33];
      snprintf(top_banner_text, 32, "f      bx%d", s_player_score);
      clear_top_row(s_graphics);
      draw_text_at_location(s_graphics, top_banner_text, SCORE_BAR_OFFSET, 0, 0, true);
      
      update_fuel();

      if (s_player_fuel == 0) {
        player_set_vertical_direction(D_NONE);
        player_move_off_screen_down();
        s_game_state = GS_MOVE_PLAYER_OFF_SCREEN;
      }
      break;
    case GS_MOVE_PLAYER_OFF_SCREEN:
      player_step(s_graphics);
      render_background(s_graphics, get_player_x(), get_player_y());
      items_step(s_graphics);

      if (!player_moving()) {
        char new_game_text[70];
        snprintf(new_game_text, 70, "  OUT OF FUEL\n\nSCORE:   %5d\n\nHI SCORE:%5d\n\n PRESS SELECT", s_player_score, s_high_score);
        clear_window_layer(s_graphics);
        set_window_layer_frame(s_graphics, GAME_OVER_WINDOW, GAME_OVER_WINDOW_START);
        draw_text_at_location(s_graphics, new_game_text, PBL_IF_ROUND_ELSE(4, 2), PBL_IF_ROUND_ELSE(3, 7), WINDOW_PALETTE, false);

        start_window_animation(GAME_OVER_WINDOW_START, GAME_OVER_WINDOW_END);

        clear_top_row(s_graphics);
        draw_text_at_location(s_graphics, "GAME OVER", PBL_IF_ROUND_ELSE(7, 5), 0, 0, true);

        s_game_state = GS_GAME_OVER_TRANSITION;
      }

      break;
    case GS_GAME_OVER_TRANSITION:
      player_step(s_graphics);
      render_background(s_graphics, get_player_x(), get_player_y());
      items_step(s_graphics);

      if (!is_window_animating()) {
        items_clear();
        s_game_state = GS_GAME_OVER;
      } else {
        step_window_animation(s_graphics);
      }
      break;
    case GS_GAME_OVER:
      break;
    case GS_GAME_OVER_NEW_GAME_TRANSITION:
      player_step(s_graphics);
      render_background(s_graphics, get_player_x(), get_player_y());
      items_step(s_graphics);

      if (!is_window_animating()) {
        s_game_state = GS_MOVE_PLAYER_ON_SCREEN;
        player_move_on_screen_right();
      } else {
        step_window_animation(s_graphics);
      }
      break;
    case GS_MOVE_PLAYER_ON_SCREEN:
      player_step(s_graphics);
      render_background(s_graphics, get_player_x(), get_player_y());
      items_step(s_graphics);

      if (!player_moving()) {
        s_game_state = GS_PLAYING;
        items_init(s_graphics);
      }
      break;
    default:
      break;
  }
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

void new_game() {
  generate_new_game_background(s_graphics);
  render_background(s_graphics, get_player_x(), get_player_y());
  player_init(s_graphics);
  s_player_fuel = MAX_PLAYER_FUEL;
  s_player_score = 0;
}

void game_init(Window *window) {
  s_graphics = init_gbc_graphics(window);
  
  window_init(s_graphics);
  text_init(s_graphics);
  new_game();
  player_move_on_screen_right();
  
  s_game_state = GS_LOAD_IN_TRANSITION;

  clear_top_row(s_graphics);
  draw_text_at_location(s_graphics, "START GAME", PBL_IF_ROUND_ELSE(7, 4), 0, 0, true);
  
  char new_game_text[33];
  snprintf(new_game_text, 32, "HI SCORE:%5d\n\n PRESS SELECT", s_high_score);
  clear_window_layer(s_graphics);
  set_window_layer_frame(s_graphics, NEW_GAME_WINDOW, NEW_GAME_WINDOW_START);
  draw_text_at_location(s_graphics, new_game_text, PBL_IF_ROUND_ELSE(4, 2), 1, WINDOW_PALETTE, false);
  start_window_animation(NEW_GAME_WINDOW_START, NEW_GAME_WINDOW_END);

  s_frame_timer = app_timer_register(FRAME_DURATION, frame_timer_handle, NULL);
  app_focus_service_subscribe(will_focus_handler);
}

void game_deinit() {
  GBC_Graphics_destroy(s_graphics);
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // Do different things depending on the current state
  switch (s_game_state) {
    case GS_NEW_GAME:
      clear_top_row(s_graphics);
      draw_text_at_location(s_graphics, "GET READY", PBL_IF_ROUND_ELSE(7, 5), 0, 0, true);
      start_window_animation(NEW_GAME_WINDOW_END, NEW_GAME_WINDOW_START);
      s_game_state = GS_NEW_GAME_TRANSITION;
      break;
    case GS_GAME_OVER:
      new_game();
      clear_top_row(s_graphics);
      draw_text_at_location(s_graphics, "FLY AGAIN", PBL_IF_ROUND_ELSE(7, 5), 0, 0, true);
      start_window_animation(GAME_OVER_WINDOW_END, GAME_OVER_WINDOW_START);
      s_game_state = GS_GAME_OVER_NEW_GAME_TRANSITION;
    default:
      break;
  }
}

void up_press_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_game_state == GS_PLAYING) {
    player_set_vertical_direction(D_UP);
  }
}

void up_release_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_game_state == GS_PLAYING) {
    player_set_vertical_direction(D_NONE);
  }
}

void down_press_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_game_state == GS_PLAYING) {
    player_set_vertical_direction(D_DOWN);
  }
}

void down_release_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_game_state == GS_PLAYING) {
    player_set_vertical_direction(D_NONE);
  }
}