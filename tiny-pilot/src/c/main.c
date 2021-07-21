#include <pebble.h>
#include "game.h"

static Window *s_window;

static void window_load(Window *window) {
    window_set_background_color(window, GColorPictonBlue); // This is the color of the sky in-game
}

static void window_unload(Window *window) {

}

static void init(void) {
    s_window = window_create();
    
    // Start the game
    game_init(s_window);

    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    window_stack_push(s_window, animated);
}

static void deinit(void) {
    // End the game
    game_deinit();
    window_destroy(s_window);
}

int main(void) {
    init();

    app_event_loop();
    deinit();
}
