#include <pebble.h>
#include "pebble-gbc-graphics/pebble-gbc-graphics.h"

#define NUMBER_OF_VRAM_BANKS_TO_GENERATE 1

static Window *s_window;
static GBC_Graphics *s_gbc_graphics;

static void window_load(Window *window) {
  
}

static void window_unload(Window *window) {

}

static void init(void) {
    s_window = window_create();
    
    // Create the GBC_Graphics object
    s_gbc_graphics = GBC_Graphics_ctor(s_window, NUMBER_OF_VRAM_BANKS_TO_GENERATE);

    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    window_stack_push(s_window, animated);
}

static void deinit(void) {
    // Destroy the GBC_Graphics object
    GBC_Graphics_destroy(s_gbc_graphics);

    window_destroy(s_window);
}

int main(void) {
    init();

    app_event_loop();
    deinit();
}
