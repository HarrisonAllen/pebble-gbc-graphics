#include "game.h"
#include "graphics/background.h"
#include "graphics/text.h"
#include "graphics/fuel.h"
#include "graphics/window.h"
#include "actors/player.h"
#include "actors/items.h"
#include "actors/hiscore.h"
#include "util.h"

static AppTimer *s_frame_timer;   // The timer used to setup the game step callback
static GBC_Graphics *s_graphics;  // The GBC_Graphics object used for rendering
static uint16_t s_player_score;   // The player's score
static uint16_t s_player_fuel;    // The player's fuel amount
static GameState s_game_state;    // The state of the game
static uint16_t s_high_score;     // The current high score
static bool s_new_high_score;     // Has a new high score been achieved this round?

/**
 * Checks if the player is overlapping any items, and handles the interactions
 */
static void check_collision() {
  for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
    bool item_overlaps_player = grects_overlap(get_player_collision(), get_item_collision(i));
    if (item_overlaps_player) {
      uint *item = get_item(i);
      switch(item[0]) {
        // Overlap a balloon, that's a point
        case IT_BALLOON:
          s_player_score++;
          break;
        
        // Overlap fuel, regain some fuel
        case IT_FUEL:
          s_player_fuel += MAX_PLAYER_FUEL / NUM_FUEL_BARS;
          break;
        default:
          break;
      }

      // Now let the items respond to the collision
      handle_collision(s_graphics, i);
    }
  }
}

/**
 * Consumes one unit of fuel, and renders the new fuel bar
 */
static void update_fuel() {
  if (s_player_fuel > 0) {
    s_player_fuel--;
  }
  if (s_player_fuel > MAX_PLAYER_FUEL) {
    s_player_fuel = MAX_PLAYER_FUEL;
  }
  // Slow down the engine as the player runs out of fuel
  float fuel_percentage = (float)s_player_fuel / MAX_PLAYER_FUEL;
  if (s_player_fuel == 0) {
    player_set_engine_speed(s_graphics, ES_STOPPED);
  } else if (fuel_percentage <= 0.1) {
    player_set_engine_speed(s_graphics, ES_SLOW);
  } else if (fuel_percentage <= 0.3) {
    player_set_engine_speed(s_graphics, ES_MEDIUM);
  } else {
    player_set_engine_speed(s_graphics, ES_FAST);
  }
  draw_fuel_bar(s_graphics, s_player_fuel, MAX_PLAYER_FUEL, NUM_FUEL_BARS, SCORE_BAR_OFFSET + 1, 0);
}

/**
 * Steps through the game logic
 */
static void game_step() {
  // Every step we want to keep the player moving and the background moving
  player_step(s_graphics);
  render_background(s_graphics, get_player_x(), get_player_y());
  
  switch (s_game_state) {
    // Animate the window flying in until it's done
    case GS_LOAD_IN_TRANSITION:
      if (!is_window_animating()) {
        s_game_state = GS_NEW_GAME;
      } else {
        step_window_animation(s_graphics);
      }
      break;

    // Wait for player input
    case GS_NEW_GAME:
      break;

    // Once window is off the screen, load the items
    case GS_NEW_GAME_TRANSITION:
      if (!is_window_animating()) {
        s_game_state = GS_PLAYING;
        items_init(s_graphics);
      } else {
        step_window_animation(s_graphics);
      }
      break;

    // Main gameplay state
    case GS_PLAYING:
      items_step(s_graphics); // Move the items

      check_collision(); // Handle player overlaps

      // Draw the score bar on the top row of the background
      char top_banner_text[33];
      snprintf(top_banner_text, 32, "f      bx%d", s_player_score);
      clear_top_row(s_graphics);
      draw_text_at_location(s_graphics, top_banner_text, SCORE_BAR_OFFSET, 0, 0, true);
      
      update_fuel(); // Consume and draw the fuel

      // Once the player runs out of fuel, the game is over
      if (s_player_fuel == 0) { 
        player_move_off_screen_down(); // Fall off the screen
        s_game_state = GS_MOVE_PLAYER_OFF_SCREEN;

        // Check if we have a new high score
        s_new_high_score = s_player_score > s_high_score;
        if (s_new_high_score) {
          s_high_score = s_player_score;
        }
      }
      break;

    // Move the player off the screen, and keep the items moving
    case GS_MOVE_PLAYER_OFF_SCREEN:
      items_step(s_graphics);

      // Once the player is off, slide the game over screen from the left
      if (!player_moving()) {
        char new_game_text[70];
        // Display a different message if player lost (fuel out) or quit (back button)
        snprintf(new_game_text, 70, "%s\n\nSCORE:   %5d\n\nHI SCORE:%5d\n\n PRESS SELECT",
                 s_player_fuel == 0 ? " OUT OF FUEL" : " FLIGHT ENDED", s_player_score, s_high_score);
        clear_window_layer(s_graphics);
        set_window_layer_frame(s_graphics, GAME_OVER_WINDOW, GAME_OVER_WINDOW_START);
        draw_text_at_location(s_graphics, new_game_text, PBL_IF_ROUND_ELSE(4, 2), PBL_IF_ROUND_ELSE(3, 7), WINDOW_PALETTE, false);
        set_window_has_priority(s_graphics, false); // Want to make sure the "NEW" sprite is visible

        start_window_animation(GAME_OVER_WINDOW_START, GAME_OVER_WINDOW_END);

        clear_top_row(s_graphics);
        draw_text_at_location(s_graphics, "GAME OVER", PBL_IF_ROUND_ELSE(6, 4), 0, 0, true);

        s_game_state = GS_GAME_OVER_TRANSITION;
      }

      break;

    // Game over screen moving on, items keep moving
    case GS_GAME_OVER_TRANSITION:
      if (!is_window_animating()) {
        items_clear();
        s_game_state = GS_GAME_OVER;
      } else {
        step_window_animation(s_graphics);
      }

      items_step(s_graphics);

      if (s_new_high_score) { // Move high score sprite with it
        hiscore_step(s_graphics);
      }
      break;

    // Wait for player input, keep high score sprite bobbing and changing color
    case GS_GAME_OVER:
      if (s_new_high_score) {
        hiscore_step(s_graphics);
      }
      break;

    // Move game over screen off screen
    case GS_GAME_OVER_NEW_GAME_TRANSITION:
      if (!is_window_animating()) {
        s_game_state = GS_MOVE_PLAYER_ON_SCREEN;
        player_move_on_screen_right();
      } else {
        step_window_animation(s_graphics);
      }

      if (s_new_high_score) { // Move high score sprite with it
        hiscore_step(s_graphics);
      }
      break;

    // Move player onto the screen 
    case GS_MOVE_PLAYER_ON_SCREEN:
      if (!player_moving()) {
        s_game_state = GS_PLAYING;
        items_init(s_graphics); // Reset the items here
      }
      break;
    default:
      break;
  }

  GBC_Graphics_render(s_graphics); // Render the screen every step
}

/**
 * The callback for the timer, this is where we call the game step
 */
static void frame_timer_handle(void* context) {
  s_frame_timer = app_timer_register(FRAME_DURATION, frame_timer_handle, NULL);
  game_step();
}

/**
 * Handler for when the app loses or gains focus
 */
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

/**
 * Initialize the GBC_Graphics object
 * 
 * @param window The window to render the GBC_Graphics object in
 * @return A pointer to a GBC_Graphics object
 */
GBC_Graphics *init_gbc_graphics(Window *window) {
  // Create with 1 VRAM bank since that's all the space we'll need
  // Plus Aplite can only really support 1 bank
  GBC_Graphics *graphics = GBC_Graphics_ctor(window, 1);

  // I have predefined some common screen sizes, prefixed by GBC_SCREEN_BOUNDS_,
  // but you can create your own with any resolution!
  // Keep in mind that drawing fewer pixels (i.e. smaller resolutions) will
  // lead to some serious performance gains
#if defined(PBL_ROUND)
  // For round, I want to make sure the score bar is visible, so I use
  // the wide screen bounds and move it down. In main.c, I set the
  // window color to the same blue as the sky, in order to give the
  // illusion of a fullscreen application
  GRect round_bounds = GBC_SCREEN_BOUNDS_WIDE;
  round_bounds.origin.y += 18;
  GBC_Graphics_set_screen_bounds(graphics, round_bounds);
#else
  // For rectangle watches, we can use fullscreen bounds since
  // no readability is lost
  GBC_Graphics_set_screen_bounds(graphics, GBC_SCREEN_BOUNDS_FULL);
#endif
  
  // This means one sprite uses two tiles, stacked vertically
  // The Legend of Zelda: Link's Awakening uses this 8x16 mode,
  // meaning Link's 16x16 character model only uses 2 sprite slots
  // Pokemon, however, uses the 8x8 mode and uses 4 sprites slots
  // to render the 16x16 player model
  GBC_Graphics_lcdc_set_8x16_sprite_mode_enabled(graphics, true);

  load_tilesheets(graphics);

  return graphics;
}

/**
 * Starts a new game
 */
void new_game() {
  // Setup the background and player
  generate_new_game_background(s_graphics); // Create a new, randomized background
  render_background(s_graphics, get_player_x(), get_player_y());
  player_init(s_graphics);

  // Reset the player stat variables
  s_player_fuel = MAX_PLAYER_FUEL;
  s_player_score = 0;
}

/**
 * Handles select clicks, for starting the game
 */
void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  switch (s_game_state) {
    // Exit new game menu, start the game
    case GS_NEW_GAME:
      clear_top_row(s_graphics);
      draw_text_at_location(s_graphics, "GET READY", PBL_IF_ROUND_ELSE(6, 4), 0, 0, true);
      start_window_animation(NEW_GAME_WINDOW_END, NEW_GAME_WINDOW_START);
      s_game_state = GS_NEW_GAME_TRANSITION;
      s_new_high_score = false;
      break;

    // Exit the game over menu, start a new game
    case GS_GAME_OVER:
      new_game();
      clear_top_row(s_graphics);
      draw_text_at_location(s_graphics, "FLY AGAIN", PBL_IF_ROUND_ELSE(6, 4), 0, 0, true);
      start_window_animation(GAME_OVER_WINDOW_END, GAME_OVER_WINDOW_START);
      s_game_state = GS_GAME_OVER_NEW_GAME_TRANSITION;
      break;
    default:
      break;
  }
}

/**
 * Ends a round when by way of the back button press
 */
static void end_round() {
  player_set_vertical_direction(D_NONE);
  player_move_off_screen_left(); // Slide off the scren
  s_game_state = GS_MOVE_PLAYER_OFF_SCREEN;

  // Game is over, so check high score!
  s_new_high_score = s_player_score > s_high_score;
  if (s_new_high_score) {
    s_high_score = s_player_score;
  }
}

/**
 * Handles back clicks, ending round when playing, quitting app when not playing
 */
void back_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_game_state == GS_PLAYING) {
    end_round();
  } else {
    window_stack_pop(true);
  }
}

/**
 * Handle up presses, moves player up while playing
 */
void up_press_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_game_state == GS_PLAYING) {
    player_set_vertical_direction(D_UP);
  }
}

/**
 * Handle up releases, stops player movement while playing
 */
void up_release_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_game_state == GS_PLAYING) {
    player_set_vertical_direction(D_NONE);
  }
}

/**
 * Handle down presses, moves player down while playing
 */
void down_press_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_game_state == GS_PLAYING) {
    player_set_vertical_direction(D_DOWN);
  }
}

/**
 * Handle down releases, stops player movement while playing
 */
void down_release_handler(ClickRecognizerRef recognizer, void *context) {
  if (s_game_state == GS_PLAYING) {
    player_set_vertical_direction(D_NONE);
  }
}

/**
 * Setup input callbacks
 */
static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
  window_raw_click_subscribe(BUTTON_ID_UP, up_press_handler, up_release_handler, NULL);
  window_raw_click_subscribe(BUTTON_ID_DOWN, down_press_handler, down_release_handler, NULL);
}

/**
 * Load prevously saved data
 * 
 * @param data A pointer to a SaveData object to load into
 * @return true if previous save data was found, false if no previous save
 */
static bool load(SaveData *data) {
  return persist_read_data(SAVE_KEY, data, sizeof(SaveData)) != E_DOES_NOT_EXIST;
}

/**
 * Saves the player's high score
 */
static void save() {
  // Compare high score in case the player beat it
  if (s_player_score > s_high_score) {
    s_high_score = s_player_score;
  }

  SaveData data = (SaveData) {
    .high_score = s_high_score,
  };
  persist_write_data(SAVE_KEY, &data, sizeof(SaveData));
}

/**
 * Create the game
 * 
 * @param window The window to render the GBC_Graphics object in
 */
void game_init(Window *window) {
  s_graphics = init_gbc_graphics(window);

  // Load the previous high score if it exists
  SaveData data;
  if (load(&data)) {
    s_high_score = data.high_score;
  }
  
  // Setup the game
  window_init(s_graphics);
  text_init(s_graphics);
  hiscore_init(s_graphics);
  new_game();

  // Draw top bar text
  clear_top_row(s_graphics);
  draw_text_at_location(s_graphics, "START GAME", PBL_IF_ROUND_ELSE(6, 4), 0, 0, true);
  
  // Set up the new game window
  char new_game_text[33];
  snprintf(new_game_text, 32, "HI SCORE:%5d\n\n PRESS SELECT", s_high_score);
  clear_window_layer(s_graphics);
  set_window_layer_frame(s_graphics, NEW_GAME_WINDOW, NEW_GAME_WINDOW_START);
  draw_text_at_location(s_graphics, new_game_text, PBL_IF_ROUND_ELSE(4, 2), 1, WINDOW_PALETTE, false);


  // Move the player on from the left, and move the window on from the bottom
  player_move_on_screen_right();
  start_window_animation(NEW_GAME_WINDOW_START, NEW_GAME_WINDOW_END);
  s_game_state = GS_LOAD_IN_TRANSITION;
  
  // Setup input
  window_set_click_config_provider(window, click_config_provider);

  // Setup the frame timer that will call the game step function
  s_frame_timer = app_timer_register(FRAME_DURATION, frame_timer_handle, NULL);
  app_focus_service_subscribe(will_focus_handler);

  // Do an initial render of our input
  GBC_Graphics_render(s_graphics);
}

/**
 * Destroy the game
 */
void game_deinit() {
  // Save the high score before exiting
  save();
  // Be sure to destroy the graphic object when done!
  GBC_Graphics_destroy(s_graphics);
}