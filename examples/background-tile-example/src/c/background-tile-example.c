#include <pebble.h>
#include "pebble-gbc-graphics-advanced/pebble-gbc-graphics-advanced.h"
#include "tile-information.h"

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
#define FRAME_DURATION 500

#define NUM_VRAMS 1
#define NUM_BACKGROUNDS 1

static Window *s_window;
static GBC_Graphics *s_gbc_graphics;
static AppTimer *s_frame_timer;  // The timer used to setup the game step callback
static int s_animation_frame; // The frame of the background tile animation

/**
 * Loads a tilesheet from the resources into a VRAM bank
 */
static void load_tilesheet() {
    // Calculate how many tiles are on the tilesheet
    ResHandle handle = resource_get_handle(RESOURCE_ID_TILESHEET);
    size_t res_size = resource_size(handle);
    uint16_t tiles_to_load = res_size / GBC_TILE_NUM_BYTES;

    uint8_t tilesheet_start_offset = 0; // This is the tile on the tilesheet we want to start loading from
    uint8_t vram_start_offset = 0; // This is the tile in the VRAM we want to start loading into
    uint8_t vram_bank = 0; // The VRAM bank we want to store the tiles into
    GBC_Graphics_load_from_tilesheet_into_vram(s_gbc_graphics, RESOURCE_ID_TILESHEET, tilesheet_start_offset, 
                                                tiles_to_load, vram_start_offset, vram_bank);
}

/**
 * Sets palettes for the backgrounds and sprites. You may find this link helpful: https://developer.rebble.io/developer.pebble.com/guides/tools-and-resources/color-picker/index.html
 */
static void create_palettes() {
    GBC_Graphics_set_bg_palette_array(s_gbc_graphics, 0, TILESHEET_PALETTE);
}

// Draws a block (2x2 tiles) on the background
static void draw_background_block(int block_x, int block_y, int block_id) {
    int tile_x = block_x * BLOCK_WIDTH;
    int tile_y = block_y * BLOCK_HEIGHT;
    int block_tile_offset = TILESHEET_OFFSETS[block_id];
    for (int y = 0; y < BLOCK_HEIGHT; y++) {
        for (int x = 0; x < BLOCK_WIDTH; x++) {
            uint8_t tile = block_tile_offset + x + y * BLOCK_WIDTH;
            GBC_Graphics_bg_set_tile(s_gbc_graphics, 0, tile_x + x, tile_y + y, tile);
        }
    }
}

/**
 * Fills in the background with blocks (which are sets of tiles)
 */
static void generate_background() {
    // Draw the background blocks
    for (int block_y = 0; block_y < MAP_BLOCK_HEIGHT; block_y++) {
        for (int block_x = 0; block_x < MAP_BLOCK_WIDTH; block_x++) {
            uint8_t block = EXAMPLE_MAP[block_x + block_y * MAP_BLOCK_WIDTH];
            draw_background_block(block_x, block_y, block);
        }
    }
}

static void step() {
    // Here I'll animate the tiles
    for (int i = 0; i < NUM_ANIMATED_BLOCKS; i++) {
        draw_background_block(BLOCKS_TO_ANIMATE[i][0], BLOCKS_TO_ANIMATE[i][1], ANIMATION_SEQUENCE[s_animation_frame]);
    }
    s_animation_frame = (s_animation_frame + 1) % NUM_ANIMATION_FRAMES;

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

    GBC_Graphics_set_screen_bounds(s_gbc_graphics, GBC_SCREEN_BOUNDS_RECT);

    load_tilesheet();
    create_palettes();
    generate_background();

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
