#include <pebble.h>
#include "pebble-gbc-graphics/pebble-gbc-graphics.h"
#include "game.h"

static Window *s_window;

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_raw_click_subscribe(BUTTON_ID_UP, up_press_handler, up_release_handler, NULL);
  window_raw_click_subscribe(BUTTON_ID_DOWN, down_press_handler, down_release_handler, NULL);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(window, GColorPictonBlue);

  // First, create our game window
  game_init(window);
}

static void window_unload(Window *window) {

}

static void init(void) {
  s_window = window_create();
  window_set_click_config_provider(s_window, click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void deinit(void) {
  // Make sure to destroy the graphics object!
  game_deinit();
  window_destroy(s_window);
}

int main(void) {
  init();

  app_event_loop();
  deinit();
}
