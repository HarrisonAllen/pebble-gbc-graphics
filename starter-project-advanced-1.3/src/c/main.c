#include <pebble.h>
#include "pebble-gbc-graphics-advanced/pebble-gbc-graphics-advanced.h"

/*
 * This is the duration of a frame, in milliseconds (more or less)
 * E.g. a FRAME_DURATION of 33ms will give ~30 fps
 * For "as fast as possible" (i.e. render as soon as the
 * graphics engine has completed its last draw cycle),
 * use a FRAME_DURATION of 2. 
 * Aplite has a slower CPU than the rest, so it will get worse frame 
 * rate no matter what
 * Basalt and Chalk are very similar, Chalk is just barely slower
 * Diorite has similar performance to Basalt and Chalk, but may
 * need to be tweaked to get the same performance
 */
#define FRAME_DURATION 100

#define NUM_VRAMS 1
#define NUM_BACKGROUNDS 4

#define Y_OFFSET (PBL_DISPLAY_HEIGHT - 180) / 2
#define X_OFFSET (PBL_DISPLAY_WIDTH - 180) / 2

static Window *s_window;
static GBC_Graphics *s_gbc_graphics;
static AppTimer *s_frame_timer;  // The timer used to setup the game step callback
bool sprite_reverse;
int sprite_min = 90 - 30 + X_OFFSET + GBC_SPRITE_OFFSET_X;
int sprite_max = 90 + 30 + X_OFFSET + GBC_SPRITE_OFFSET_X;
int sprite_layer = NUM_BACKGROUNDS - 1;


/**
 * Loads a tilesheet from the resources into a VRAM bank
 */
static void load_tilesheet() {
    // Calculate how many tiles are on the tilesheet
    ResHandle handle = resource_get_handle(RESOURCE_ID_DATA_SAMPLE_TILESHEET);
    size_t res_size = resource_size(handle);
    uint16_t tiles_to_load = res_size / GBC_TILE_NUM_BYTES;

    uint8_t tilesheet_start_offset = 0; // This is the tile on the tilesheet we want to start loading from
    uint8_t vram_start_offset = 0; // This is the tile in the VRAM we want to start loading into
    uint8_t vram_bank = 0; // The VRAM bank we want to store the tiles into
    GBC_Graphics_load_from_tilesheet_into_vram(s_gbc_graphics, RESOURCE_ID_DATA_SAMPLE_TILESHEET, tilesheet_start_offset, 
                                                tiles_to_load, vram_start_offset, vram_bank);
}

/**
 * Sets palettes for the background in various gradients. You may find this link helpful: https://developer.rebble.io/developer.pebble.com/guides/tools-and-resources/color-picker/index.html
 */
static void create_palettes() {
#if defined(PBL_COLOR) // Pebbles with color screens use the ARGB8 Pebble color definitions for palettes (which are just bytes)
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 0, 5, GColorBlackARGB8, GColorRedARGB8, GColorYellowARGB8, GColorBlueARGB8, GColorGreenARGB8);
    GBC_Graphics_set_sprite_palette(s_gbc_graphics, 0, 2, GColorBlackARGB8, GColorBlackARGB8);
#else // Pebbles with black and white screens use the GBC_COLOR definitions for palettes
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 0, 5, GBC_COLOR_BLACK, GBC_COLOR_GRAY, GBC_COLOR_GRAY, GBC_COLOR_WHITE, GBC_COLOR_BLACK);
    GBC_Graphics_set_sprite_palette(s_gbc_graphics, 0, 2, GBC_COLOR_BLACK, GBC_COLOR_BLACK);
#endif
}

/**
 * Sets the background tiles in a test pattern to demonstrate palettes
 */
static void generate_backgrounds() {
    for (uint8_t y = 0; y < GBC_TILEMAP_HEIGHT; y++) {
        for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) {
            for (uint8_t bg_num = 0; bg_num < NUM_BACKGROUNDS; bg_num++) {
                if (rand() % 8 > 0) {
                    GBC_Graphics_bg_set_tile(s_gbc_graphics, bg_num, x, y, 0);
                } else {
                    GBC_Graphics_bg_set_tile(s_gbc_graphics, bg_num, x, y, bg_num + 1);
                }
                GBC_Graphics_bg_set_tile_palette(s_gbc_graphics, bg_num, x, y, 0);
            }
        }
    }
}

static void generate_sprite() {
    GBC_Graphics_oam_set_sprite(s_gbc_graphics, 0, 90 + X_OFFSET + GBC_SPRITE_OFFSET_X, 90 + Y_OFFSET + GBC_SPRITE_OFFSET_Y, 1, GBC_Graphics_attr_make(0, 0, false, false, false));
}

static void step() {
    GBC_Graphics_bg_move(s_gbc_graphics, 0, 0, 1);
    GBC_Graphics_bg_move(s_gbc_graphics, 1, -1, 0);
    GBC_Graphics_bg_move(s_gbc_graphics, 2, 1, -1);
    GBC_Graphics_bg_move(s_gbc_graphics, 3, -1, 1);

    if (sprite_reverse) {
        GBC_Graphics_oam_move_sprite(s_gbc_graphics, 0, -2, 0);
        if (GBC_Graphics_oam_get_sprite_x(s_gbc_graphics, 0) < sprite_min) {
            sprite_reverse = false;
            sprite_layer = (sprite_layer - 1) % NUM_BACKGROUNDS;
            GBC_Graphics_lcdc_set_sprite_layer_z(s_gbc_graphics, sprite_layer);
        }
    } else {
        GBC_Graphics_oam_move_sprite(s_gbc_graphics, 0, 2, 0);
        if (GBC_Graphics_oam_get_sprite_x(s_gbc_graphics, 0) > sprite_max) {
            sprite_reverse = true;
        }
    }
    
    GBC_Graphics_render(s_gbc_graphics); // Render the screen every step
}

/**
 * The callback for the timer, this is where we call the game step
 */
static void frame_timer_handle(void* context) {
    s_frame_timer = app_timer_register(FRAME_DURATION, frame_timer_handle, NULL);
    step();
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
 * Execute all of the graphics functions
 */
static void window_load(Window *window) {
    // Create the GBC_Graphics object
    s_gbc_graphics = GBC_Graphics_ctor(s_window, NUM_VRAMS, NUM_BACKGROUNDS);

    GBC_Graphics_set_screen_bounds(s_gbc_graphics, GBC_SCREEN_BOUNDS_SQUARE);

    load_tilesheet();
    create_palettes();
    generate_backgrounds();
    generate_sprite();

    // Setup the frame timer that will call the game step function
    s_frame_timer = app_timer_register(FRAME_DURATION, frame_timer_handle, NULL);
    app_focus_service_subscribe(will_focus_handler);

    // Display the graphics
    GBC_Graphics_render(s_gbc_graphics);
}

static void window_unload(Window *window) {
    // Destroy the GBC_Graphics object
    GBC_Graphics_destroy(s_gbc_graphics);
}

static void init(void) {
    s_window = window_create();

    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    window_stack_push(s_window, animated);
}

static void deinit(void) {
    window_destroy(s_window);
}

int main(void) {
    init();

    app_event_loop();
    deinit();
}
