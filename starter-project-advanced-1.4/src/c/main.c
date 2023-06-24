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

#if defined(PBL_COLOR)
static uint8_t black = 0b11000000;
static uint8_t dark_gray = 0b11010101;
static uint8_t light_gray = 0b11101010;
static uint8_t white = 0b11111111;
static uint8_t full_palette[] = {
    0b11110000,
    0b11110100,
    0b11111000,
    0b11111100,
    0b11101100,
    0b11011100,
    0b11001100,
    0b11001101,
    0b11001110,
    0b11001111,
    0b11001011,
    0b11000111,
    0b11000011,
    0b11010011,
    0b11100011,
    0b11110011,
    0b11110010,
    0b11110001,
    0b11000000,
    0b11010101,
    0b11101010,
    0b11111111
};
static uint8_t color_pairs[][2] = {
    {0b11110000, 0b11111010},
    {0b11001100, 0b11101110},
    {0b11000011, 0b11101011},
    {0b11000000, 0b11101010}
};
#else
static uint8_t black = GBC_COLOR_BLACK;
static uint8_t dark_gray = GBC_COLOR_GRAY;
static uint8_t light_gray = GBC_COLOR_ALPHA_GRAY;
static uint8_t white = GBC_COLOR_WHITE;
static uint8_t full_palette[] = {
    GBC_COLOR_BLACK,
    GBC_COLOR_BLACK,
    GBC_COLOR_BLACK,
    GBC_COLOR_BLACK,
    GBC_COLOR_GRAY,
    GBC_COLOR_GRAY,
    GBC_COLOR_GRAY,
    GBC_COLOR_GRAY,
    GBC_COLOR_WHITE,
    GBC_COLOR_WHITE,
    GBC_COLOR_WHITE,
    GBC_COLOR_WHITE,
    GBC_COLOR_GRAY,
    GBC_COLOR_GRAY,
    GBC_COLOR_GRAY,
    GBC_COLOR_GRAY
};
static uint8_t color_pairs[][2] = {
    {GBC_COLOR_BLACK, GBC_COLOR_GRAY},
    {GBC_COLOR_BLACK, GBC_COLOR_GRAY},
    {GBC_COLOR_BLACK, GBC_COLOR_GRAY},
    {GBC_COLOR_BLACK, GBC_COLOR_GRAY}
};
#endif

static Window *s_window;
static GBC_Graphics *s_gbc_graphics;
static AppTimer *s_frame_timer;  // The timer used to setup the game step callback
bool sprite_reverse;
int sprite_min = 90 - 30 + X_OFFSET + GBC_SPRITE_OFFSET_X;
int sprite_max = 90 + 30 + X_OFFSET + GBC_SPRITE_OFFSET_X;
int sprite_layer = NUM_BACKGROUNDS - 1;
int sprite_palette = 0;
int sprite_width, sprite_height;
bool sprite_flip_x, sprite_flip_y;
int sprite_num = 28;
int alpha_mode = 0;


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
 * Sets palettes for the backgrounds and sprites. You may find this link helpful: https://developer.rebble.io/developer.pebble.com/guides/tools-and-resources/color-picker/index.html
 */
static void create_palettes() {
    for (uint8_t i = 0; i < NUM_BACKGROUNDS; i++) {
        GBC_Graphics_set_bg_palette(s_gbc_graphics, i, 3, white, color_pairs[i][0], color_pairs[i][1]);
        GBC_Graphics_set_sprite_palette(s_gbc_graphics, i, 3, white, color_pairs[i][0], color_pairs[i][1]);
    }
    GBC_Graphics_set_bg_palette(s_gbc_graphics, 5, 5, black, black, dark_gray, light_gray, white);
}

/**
 * Sets the background tiles in a test pattern to demonstrate palettes
 */
static void generate_backgrounds() {
    GRect bounds = GBC_Graphics_get_screen_bounds(s_gbc_graphics);

    GBC_Graphics_alpha_mode_set_bg_enabled(s_gbc_graphics, 3, true);
    // GBC_Graphics_lcdc_set_alpha_blend_mode_enabled(s_gbc_graphics, true);
    for (uint8_t y = 0; y < GBC_TILEMAP_HEIGHT; y++) {
        for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) {
            uint8_t bg_num = 0;
            // I couldn't figure this out so I hardcoded it lol
            if ((x + 1) % 4 < 2 && (y + 1) % 4 < 2) {
                GBC_Graphics_bg_set_tile(s_gbc_graphics, bg_num, x, y, 16 + bg_num);
                GBC_Graphics_bg_set_tile_x_flip(s_gbc_graphics, bg_num, x, y, x % 2 == 0);
                GBC_Graphics_bg_set_tile_y_flip(s_gbc_graphics, bg_num, x, y, y % 2 == 0);
                GBC_Graphics_bg_set_tile_palette(s_gbc_graphics, bg_num, x, y, bg_num);
            }
            bg_num = 1;
            if ((x) % 4 < 2 && (y + 1) % 4 < 2) {
                GBC_Graphics_bg_set_tile(s_gbc_graphics, bg_num, x, y, 16 + bg_num);
                GBC_Graphics_bg_set_tile_x_flip(s_gbc_graphics, bg_num, x, y, x % 2 == 0);
                GBC_Graphics_bg_set_tile_y_flip(s_gbc_graphics, bg_num, x, y, y % 2 == 0);
                GBC_Graphics_bg_set_tile_palette(s_gbc_graphics, bg_num, x, y, bg_num);
            }
            bg_num = 2;
            if ((x + 1) % 4 < 2 && (y) % 4 < 2) {
                GBC_Graphics_bg_set_tile(s_gbc_graphics, bg_num, x, y, 16 + bg_num);
                GBC_Graphics_bg_set_tile_x_flip(s_gbc_graphics, bg_num, x, y, x % 2 == 0);
                GBC_Graphics_bg_set_tile_y_flip(s_gbc_graphics, bg_num, x, y, y % 2 == 0);
                GBC_Graphics_bg_set_tile_palette(s_gbc_graphics, bg_num, x, y, bg_num);
            }
            bg_num = 3;
            GBC_Graphics_bg_set_tile(s_gbc_graphics, bg_num, x, y, 1 + (x/((bounds.size.w / GBC_TILE_WIDTH) / 2)) % 2 + ((y/((bounds.size.h / GBC_TILE_HEIGHT) / 2)) % 2) * 2);
            GBC_Graphics_bg_set_tile_palette(s_gbc_graphics, bg_num, x, y, 5);
        }
    }
}

static void generate_sprite() {
    GRect bounds = GBC_Graphics_get_screen_bounds(s_gbc_graphics);

    GBC_Graphics_lcdc_set_sprite_layer_z(s_gbc_graphics, sprite_layer);
    GBC_Graphics_oam_set_sprite(s_gbc_graphics, sprite_num, (bounds.size.w / 2) - GBC_TILE_HEIGHT + sprite_width + GBC_SPRITE_OFFSET_X, (bounds.size.h / 2) - GBC_TILE_HEIGHT + GBC_SPRITE_OFFSET_Y, 16, GBC_Graphics_attr_make(0, 0, false, false, false), 1, 1);
}

static void step() {
    GBC_Graphics_bg_move(s_gbc_graphics, 0, 0, 1);
    GBC_Graphics_bg_move(s_gbc_graphics, 1, -1, 0);
    GBC_Graphics_bg_move(s_gbc_graphics, 2, 1, -1);
    // GBC_Graphics_bg_move(s_gbc_graphics, 3, -1, 1);

    if (sprite_reverse) {
        GBC_Graphics_oam_move_sprite(s_gbc_graphics, sprite_num, -2, 0);
        if (GBC_Graphics_oam_get_sprite_x(s_gbc_graphics, sprite_num) < sprite_min) {
            sprite_reverse = false;

            // sprite_layer = (sprite_layer - 1) % NUM_BACKGROUNDS;
            // GBC_Graphics_lcdc_set_sprite_layer_z(s_gbc_graphics, sprite_layer);
            sprite_palette = (sprite_palette + 1) % NUM_BACKGROUNDS;
            GBC_Graphics_oam_set_sprite_palette(s_gbc_graphics, sprite_num, sprite_palette);

            sprite_flip_x = !sprite_flip_x;
            GBC_Graphics_oam_set_sprite_x_flip(s_gbc_graphics, sprite_num, sprite_flip_x);

            alpha_mode = (alpha_mode + 1) % 7;
            GBC_Graphics_alpha_mode_set_mode(s_gbc_graphics, 3, alpha_mode);
        }
    } else {
        GBC_Graphics_oam_move_sprite(s_gbc_graphics, sprite_num, 2, 0);
        if (GBC_Graphics_oam_get_sprite_x(s_gbc_graphics, sprite_num) > sprite_max) {
            sprite_reverse = true;

            sprite_flip_x = !sprite_flip_x;
            sprite_flip_y = !sprite_flip_y;
            GBC_Graphics_oam_set_sprite_x_flip(s_gbc_graphics, sprite_num, sprite_flip_x);
            GBC_Graphics_oam_set_sprite_y_flip(s_gbc_graphics, sprite_num, sprite_flip_y);

            alpha_mode = (alpha_mode + 1) % 7;
            GBC_Graphics_alpha_mode_set_mode(s_gbc_graphics, 3, alpha_mode);
        }
    }

    if (sprite_reverse) {
        GBC_Graphics_oam_set_sprite_mosaic_y(s_gbc_graphics, sprite_num, 3 - (GBC_Graphics_oam_get_sprite_x(s_gbc_graphics, sprite_num) / 8) % 4);
        GBC_Graphics_oam_set_sprite_mosaic_x(s_gbc_graphics, sprite_num, 0);
    } else {
        GBC_Graphics_oam_set_sprite_mosaic_x(s_gbc_graphics, sprite_num, (GBC_Graphics_oam_get_sprite_x(s_gbc_graphics, sprite_num) / 8) % 4);
        GBC_Graphics_oam_set_sprite_mosaic_y(s_gbc_graphics, sprite_num, 0);
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
