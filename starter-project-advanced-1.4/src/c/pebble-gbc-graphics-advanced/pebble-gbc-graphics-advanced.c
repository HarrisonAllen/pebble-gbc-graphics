#include "pebble-gbc-graphics-advanced.h"
#include <stdarg.h>

const uint16_t BOOL_MASK[2] = {0x0000, 0xFFFF};

///> Forward declarations for static functions
static void graphics_update_proc(Layer *layer, GContext *ctx);

GBC_Graphics *GBC_Graphics_ctor(Window *window, uint8_t num_vram_banks, uint8_t num_backgrounds) { 
    GBC_Graphics *self = NULL;
    self = malloc(sizeof(GBC_Graphics));
    if (self == NULL)
        return NULL;
    
    // Initialize the bg/window and sprite layers
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    self->graphics_layer = layer_create_with_data(bounds, sizeof(self));
    *(GBC_Graphics * *)layer_get_data(self->graphics_layer) = self;
    layer_set_update_proc(self->graphics_layer, graphics_update_proc);
    layer_add_child(window_layer, self->graphics_layer);

    // Set the screen boundaries to the size of the Pebble display
    self->screen_x_origin = bounds.origin.x;
    self->screen_y_origin = bounds.origin.y;
    self->screen_width = bounds.size.w;
    self->screen_height = bounds.size.h;

    // Allocate banks of VRAM space
    self->vram = (uint8_t*)malloc(GBC_VRAM_BANK_NUM_BYTES * num_vram_banks);

    // Allocate space for the tilemaps and attributemaps
    self->bg_tilemaps = (uint8_t*)malloc(GBC_TILEMAP_NUM_BYTES * num_backgrounds);
    self->bg_attrmaps = (uint8_t*)malloc(GBC_ATTRMAP_NUM_BYTES * num_backgrounds);

    // Allocate space for the palette banks
    self->bg_palette_bank = (uint8_t*)malloc(GBC_PALETTE_BANK_NUM_BYTES);
    self->sprite_palette_bank = (uint8_t*)malloc(GBC_PALETTE_BANK_NUM_BYTES);

    self->num_backgrounds = num_backgrounds;
    self->bg_scroll_x = (short*)malloc(num_backgrounds * sizeof(short));
    self->bg_scroll_y = (short*)malloc(num_backgrounds * sizeof(short));

    // Allocate space for the OAM
    self->oam = (uint8_t*)malloc(GBC_OAM_NUM_BYTES);

    self->lcdc = 0xFF; // Start LCDC with everything enabled (render everything)
    self->stat = 0x00; // Start STAT empty

    return self;
}

void GBC_Graphics_destroy(GBC_Graphics *self) {
    free(self->vram);
    free(self->oam);
    free(self->bg_tilemaps);
    free(self->bg_attrmaps);
    free(self->bg_palette_bank);
    free(self->sprite_palette_bank);
    free(self->bg_scroll_x);
    free(self->bg_scroll_y);
    layer_destroy(self->graphics_layer);
    if (self == NULL) return;
        free(self);
}

void GBC_Graphics_set_screen_bounds(GBC_Graphics *self, GRect bounds) {
    self->screen_x_origin = bounds.origin.x;
    self->screen_y_origin = bounds.origin.y;
    self->screen_width = bounds.size.w;
    self->screen_height = bounds.size.h;
    layer_set_frame(self->graphics_layer, bounds);
}

void GBC_Graphics_set_screen_x_origin(GBC_Graphics *self, uint8_t new_x) {
    self->screen_x_origin = new_x;
}

void GBC_Graphics_set_screen_y_origin(GBC_Graphics *self, uint8_t new_y) {
    self->screen_y_origin = new_y;
}

void GBC_Graphics_set_screen_width(GBC_Graphics *self, uint8_t new_width) {
    self->screen_width = new_width;
}

void GBC_Graphics_set_screen_height(GBC_Graphics *self, uint8_t new_height) {
    self->screen_height = new_height;
}

GRect GBC_Graphics_get_screen_bounds(GBC_Graphics *self) {
    return GRect(self->screen_x_origin, self->screen_y_origin, self->screen_width, self->screen_height);
}

uint8_t GBC_Graphics_get_screen_x_origin(GBC_Graphics *self) {
    return self->screen_x_origin;
}

uint8_t GBC_Graphics_get_screen_y_origin(GBC_Graphics *self) {
    return self->screen_y_origin;
}

uint8_t GBC_Graphics_get_screen_width(GBC_Graphics *self) {
    return self->screen_width;
}

uint8_t GBC_Graphics_get_screen_height(GBC_Graphics *self) {
    return self->screen_height;
}

void GBC_Graphics_vram_move_tiles(GBC_Graphics *self, uint8_t src_vram_bank, uint8_t src_tile_offset, 
                                  uint8_t dest_vram_bank, uint8_t dest_tile_offset, uint8_t num_tiles_to_move, bool swap) {
    uint8_t *src_vram_offset = self->vram + GBC_VRAM_BANK_NUM_BYTES * src_vram_bank + src_tile_offset * GBC_TILE_NUM_BYTES;
    uint8_t *dest_vram_offset = self->vram + GBC_VRAM_BANK_NUM_BYTES * dest_vram_bank + dest_tile_offset * GBC_TILE_NUM_BYTES;
    size_t data_size = num_tiles_to_move * GBC_TILE_NUM_BYTES;
    if (swap) {
        uint8_t temp[data_size];
        memcpy(temp, dest_vram_offset, data_size);
        memcpy(dest_vram_offset, src_vram_offset, data_size);
        memcpy(src_vram_offset, temp, data_size);
    } else {
        memcpy(dest_vram_offset, src_vram_offset, data_size);
    }
}

void GBC_Graphics_load_from_tilesheet_into_vram(GBC_Graphics *self, uint32_t tilesheet_resource, uint16_t tilesheet_tile_offset, 
                                                uint16_t tiles_to_load, uint16_t vram_tile_offset, uint8_t vram_bank_number) {
    ResHandle tilesheet_handle = resource_get_handle(tilesheet_resource);
    uint8_t *vram_offset = self->vram + GBC_VRAM_BANK_NUM_BYTES * vram_bank_number + vram_tile_offset * GBC_TILE_NUM_BYTES;
    
    resource_load_byte_range(tilesheet_handle, tilesheet_tile_offset * GBC_TILE_NUM_BYTES, vram_offset, tiles_to_load * GBC_TILE_NUM_BYTES);
}

uint8_t *GBC_Graphics_get_vram_bank(GBC_Graphics *self, uint8_t vram_bank_number) {
    return &self->vram[vram_bank_number * GBC_VRAM_BANK_NUM_BYTES];
}

/**
 * Sets the colors of a palette in the given palette bank
 * 
 * @param palette_bank A pointer to the palette bank
 * @param palette_num The number of the palette to set, 0 to 7
 * @param num_colors The number of colors being set, from 1 to 16
 * @param args A va_list containing all of the colors
 */
static void set_palette(uint8_t *palette_bank, uint8_t palette_num, uint8_t num_colors, va_list args) {
    for (uint8_t i = 0; i < num_colors; i++) {
        palette_bank[palette_num*GBC_PALETTE_NUM_BYTES+i] = (uint8_t)va_arg(args, int);
    }
}

void GBC_Graphics_set_bg_palette(GBC_Graphics *self, uint8_t palette_num, uint8_t num_colors, ...) {
    va_list args;
    va_start(args, num_colors);
    set_palette(self->bg_palette_bank, palette_num, num_colors, args);
    va_end(args);
}

void GBC_Graphics_set_sprite_palette(GBC_Graphics *self, uint8_t palette_num, uint8_t num_colors, ...) {
    va_list args;
    va_start(args, num_colors);
    set_palette(self->sprite_palette_bank, palette_num, num_colors, args);
    va_end(args);
}

/**
 * Sets the colors of a palette in the given palette bank
 * 
 * @param palette_bank A pointer to the palette bank
 * @param palette_num The number of the palette to set, 0 to 7
 * @param palette_array A pointer to a 16-byte array containing the palette colors
 */
static void set_palette_array(uint8_t *palette_bank, uint8_t palette_num, uint8_t *palette_array) {
    memcpy(&palette_bank[palette_num*GBC_PALETTE_NUM_BYTES], palette_array, GBC_PALETTE_NUM_BYTES);
}

void GBC_Graphics_set_bg_palette_array(GBC_Graphics *self, uint8_t palette_num, uint8_t *palette_array) {
    set_palette_array(self->bg_palette_bank, palette_num, palette_array);
}

void GBC_Graphics_set_sprite_palette_array(GBC_Graphics *self, uint8_t palette_num, uint8_t *palette_array) {
    set_palette_array(self->sprite_palette_bank, palette_num, palette_array);
}

void GBC_Graphics_set_bg_palette_color(GBC_Graphics *self, uint8_t palette_num, uint8_t color_num, uint8_t c) {
    self->bg_palette_bank[palette_num*GBC_PALETTE_NUM_BYTES+color_num] = c;
}

void GBC_Graphics_set_sprite_palette_color(GBC_Graphics *self, uint8_t palette_num, uint8_t color_num, uint8_t c) {
    self->sprite_palette_bank[palette_num*GBC_PALETTE_NUM_BYTES+color_num] = c;
}

static void copy_palette_array(uint8_t *palette_bank, uint8_t palette_num, uint8_t *target_array) {
    memcpy(target_array, &palette_bank[palette_num*GBC_PALETTE_NUM_BYTES], GBC_PALETTE_NUM_BYTES);
}

void GBC_Graphics_copy_one_bg_palette(GBC_Graphics *self, uint8_t palette_num, uint8_t *target_array) {
    copy_palette_array(self->bg_palette_bank, palette_num, target_array);
}

void GBC_Graphics_copy_one_sprite_palette(GBC_Graphics *self, uint8_t palette_num, uint8_t *target_array) {
    copy_palette_array(self->sprite_palette_bank, palette_num, target_array);
}

static void copy_palette_bank(uint8_t *palette_bank, uint8_t *target_array) {
    memcpy(target_array, palette_bank, GBC_PALETTE_BANK_NUM_BYTES);
}

void GBC_Graphics_copy_all_bg_palettes(GBC_Graphics *self, uint8_t *target_array) {
    copy_palette_bank(self->bg_palette_bank, target_array);
}

void GBC_Graphics_copy_all_sprite_palettes(GBC_Graphics *self, uint8_t *target_array) {
    copy_palette_bank(self->sprite_palette_bank, target_array);
}

/**
 * Sets the bits outlined by mask to new_value
 * 
 * @param byte A pointer to the byte to modify
 * @param mask The bits to modify
 * @param new_value The new value to set the bits to
 * @param byte_start The offset of the data
 */
static void modify_byte(uint8_t *byte, uint8_t mask, uint8_t new_value, uint8_t byte_start) {
    *byte = (*byte & ~mask) | new_value * byte_start;
}

/**
 * Sets the bits outlined by mask to new_value
 * 
 * @param word A pointer to the word to modify
 * @param mask The bits to modify
 * @param new_value The new value to set the bits to
 * @param byte_start The offset of the data
 */
static void modify_word(uint16_t *word, uint16_t mask, uint16_t new_value, uint16_t word_start) {
    *word = (*word & ~mask) | new_value * word_start;
}


/**
 * Renders the backgrounds and sprites called from an update proc
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param layer A pointer to the layer to modify
 * @param ctx The graphics context for drawing
 */
static void render_graphics(GBC_Graphics *self, Layer *layer, GContext *ctx) {
    if (!(self->lcdc & GBC_LCDC_ENABLE_FLAG)) return;
    GBitmap *fb = graphics_capture_frame_buffer(ctx);

    // Predefine the variables we'll use in the loop
    uint8_t map_x, map_y, map_tile_x, map_tile_y, tile_num, tile_attr;
    uint8_t *tilemap, *attrmap;
    uint16_t offset;
    uint8_t *tile;
    uint8_t pixel_x, pixel_y, pixel_byte, pixel_color, pixel;
    uint8_t shift;
    uint8_t x;
    uint8_t flip_x, flip_y;
    short bg_num;
    
    uint8_t tile_x, tile_y;
    uint8_t x_on_sprite, y_on_sprite;
    uint8_t *sprite;
    uint8_t sprite_w, sprite_h;
    uint8_t sprite_tile_offset;
    uint8_t sprite_x, sprite_y;
    short sprite_id;

    uint8_t sprites_on_this_line[GBC_NUM_SPRITES];
    uint8_t num_sprites_on_this_line;
    uint8_t sprite_line_index;
    bool on_this_line;

    uint8_t current_pixel_color, new_pixel_color;

    uint8_t background_start = self->num_backgrounds - 1;
    bool pixel_set;
    uint8_t sprite_layer_z;
    uint8_t alpha_mode;
    bool alpha_enabled;
    uint8_t alpha_result;
    uint8_t alpha_mode_results[8];

    // Start by going through all of the rows
    self->stat &= ~GBC_STAT_VBLANK_FLAG; // No longer in VBlank while we draw
    for (self->line_y = 0; self->line_y < self->screen_height; self->line_y++) {
        // Check if the current line matches the line compare value, and then do the callback
        self->stat &= ~GBC_STAT_LINE_COMP_FLAG;
        self->stat |= GBC_STAT_LINE_COMP_FLAG * (self->line_y == self->line_y_compare);
        if ((self->stat & (GBC_STAT_LINE_COMP_INT_FLAG | GBC_STAT_LINE_COMP_FLAG)) == (GBC_STAT_LINE_COMP_INT_FLAG | GBC_STAT_LINE_COMP_FLAG)) {
            self->line_compare_interrupt_callback(self);
        }

        GBitmapDataRowInfo info = gbitmap_get_data_row_info(fb, self->line_y + self->screen_y_origin);
        uint8_t min_x = GBC_MAX(info.min_x, self->screen_x_origin);
        uint8_t max_x = GBC_MIN(info.max_x+1, self->screen_x_origin + self->screen_width);

        sprite_layer_z = (self->lcdc & GBC_LCDC_SPRITE_LAYER_Z_MASK) >> GBC_LCDC_SPRITE_LAYER_Z_SHIFT;
        sprite_layer_z = GBC_MIN(sprite_layer_z, background_start);
        num_sprites_on_this_line = 0;
        for (sprite_id = 0; sprite_id < (GBC_NUM_SPRITES & BOOL_MASK[(self->lcdc & GBC_LCDC_SPRITE_ENABLE_FLAG) != 0]); sprite_id++) {
            sprite = &self->oam[sprite_id * GBC_SPRITE_NUM_BYTES];
            sprite_x = sprite[GBC_OAM_X_POS_BYTE] - GBC_SPRITE_OFFSET_X;
            sprite_y = sprite[GBC_OAM_Y_POS_BYTE] - GBC_SPRITE_OFFSET_Y;
            sprite_w = GBC_TILE_WIDTH << ((sprite[GBC_OAM_DIMS_BYTE] & GBC_OAM_SPRITE_WIDTH_MASK) >> GBC_OAM_SPRITE_WIDTH_SHIFT); // tile_width * (2 ^ sprite_width)
            sprite_h = GBC_TILE_HEIGHT << ((sprite[GBC_OAM_DIMS_BYTE] & GBC_OAM_SPRITE_HEIGHT_MASK) >> GBC_OAM_SPRITE_HEIGHT_SHIFT); // tile_height * (2 ^ sprite_height)
            on_this_line = self->line_y >= sprite_y && self->line_y < (sprite_y + sprite_h)
                            && (sprite[GBC_OAM_ATTR_BYTE] & GBC_ATTR_HIDE_FLAG) == 0 
                            && sprite_x <= max_x && (sprite_x + sprite_w) >= min_x  ;

            sprites_on_this_line[num_sprites_on_this_line] = sprite_id & BOOL_MASK[on_this_line];
            num_sprites_on_this_line += on_this_line;
        }

        self->stat &= ~GBC_STAT_HBLANK_FLAG; // No longer in HBlank while we draw the line

        // Now for each pixel in the row:
        for(x = min_x; x < max_x; x++) {
            pixel_color = 0x00;

            // First draw the backgrounds below the sprite layer
            for (bg_num = 0; bg_num <= sprite_layer_z; bg_num++) {
                map_x = (x - self->screen_x_origin) + self->bg_scroll_x[bg_num];
                map_y = self->line_y + self->bg_scroll_y[bg_num];
                tile = self->vram + (bg_num << 10) + offset; // self->vram + vram_bank_number * GBC_VRAM_BANK_NUM_BYTES + offset
                tilemap = self->bg_tilemaps + (bg_num << 10); // self->bg_tilemaps + bg_num * GBC_TILEMAP_NUM_BYTES
                attrmap = self->bg_attrmaps + (bg_num << 10); // self->bg_attrmaps + bg_num * GBC_ATTRMAP_NUM_BYTES

                // Find the tile that the pixel is on
                map_tile_x = map_x >> 3; // map_x / GBC_TILE_WIDTH
                map_tile_y = map_y >> 3; // map_y / GBC_TILE_HEIGHT

                // Get the tile and attrs from the map
                tile_num = tilemap[map_tile_x + (map_tile_y << 5)]; // map_tile_y * MAP_WIDTH ??? what ???
                tile_attr = attrmap[map_tile_x + (map_tile_y << 5)];
                
                // Get the tile from vram
                offset = tile_num << 5; // tile_num * GBC_TILE_NUM_BYTES
                tile = self->vram + ((((tile_attr & GBC_ATTR_VRAM_BANK_MASK) >> 3)) << 13) + offset; // self->vram + vram_bank_number * GBC_VRAM_BANK_NUM_BYTES + offset

                // Next, we extract and return the 4bpp pixel from the tile
                pixel_x = map_x & 7; // map_x % GBC_TILE_WIDTH
                pixel_y = map_y & 7; // map_x % GBC_TILE_HEIGHT

                // Apply flip flags if necessary
                flip_x = (tile_attr & GBC_ATTR_FLIP_FLAG_X) != 0; // Flip becomes 1
                pixel_x = pixel_x + (BOOL_MASK[flip_x] & (GBC_TILE_WIDTH - (pixel_x << 1) - 1)); // pixel_x = flip_x ? sprite_w - pixel_x - 1 : pixel_x
                flip_y = (tile_attr & GBC_ATTR_FLIP_FLAG_Y) != 0; // Flip becomes 1
                pixel_y = pixel_y + (BOOL_MASK[flip_y] & (GBC_TILE_HEIGHT - (pixel_y << 1) - 1)); // pixel_y = flip_y ? sprite_w - pixel_y - 1 : pixel_y

                // To get the pixel, we first need to get the corresponding byte the pixel is in
                // There are 4 bytes per row (y * 4), and 2 pixels per byte (x / 2)
                offset = (pixel_y << 2) + (pixel_x >> 1); // pixel y * 4 + pixel_x / 2
                pixel_byte = tile[offset];

                // Once we have the byte, we need to get the 4 bit pixel out of the byte
                // This is achieved by shifting the byte (1 - x % 2) * (4 bits per pixel)
                shift = (1 ^ (pixel_x & 1)) << 2; // (1 - pixel_x % 2) * 4

                // We shift the byte and get rid of any unwanted bits
                pixel = 0b1111 & (pixel_byte >> shift);

                // Hide pixel if background disabled
                pixel = pixel & BOOL_MASK[(self->lcdc & (GBC_LCDC_BG_1_ENABLE_FLAG << bg_num)) != 0];

                // Hide pixel if tile is hidden
                pixel = pixel & BOOL_MASK[(tile_attr & GBC_ATTR_HIDE_FLAG) == 0];

                // Finally, we get the corresponding color from attribute palette
                new_pixel_color = self->bg_palette_bank[((tile_attr & GBC_ATTR_PALETTE_MASK) << 4) + pixel]; // (tile_attr & GBC_ATTR_PALETTE_MASK) * GBC_PALETTE_NUM_BYTES + pixel

                // Check if we should do an alpha blend
                alpha_enabled = (self->a_mode & (GBC_ALPHA_MODE_BG_ENABLED_FLAG << (bg_num << 2))) != 0;
                alpha_mode = (((self->a_mode & (GBC_ALPHA_MODE_MASK << (bg_num << 2))) >> (bg_num << 2)) >> GBC_ALPHA_MODE_SHIFT) & BOOL_MASK[alpha_enabled];
                
                // Create the alpha blends
                // Mode 0: Normal
                alpha_mode_results[0] = (pixel_color & BOOL_MASK[pixel == 0 && bg_num != 0]) + (new_pixel_color & BOOL_MASK[pixel != 0 || bg_num == 0]);
            #if defined(PBL_COLOR)
                // Mode 1: Add
                alpha_mode_results[1] = GBC_MAKE_COLOR(
                    GBC_ADD_CEIL(GBC_GET_RED(pixel_color), GBC_GET_RED(new_pixel_color), 0b11),
                    GBC_ADD_CEIL(GBC_GET_BLUE(pixel_color), GBC_GET_BLUE(new_pixel_color), 0b11),
                    GBC_ADD_CEIL(GBC_GET_GREEN(pixel_color), GBC_GET_GREEN(new_pixel_color), 0b11)
                );
                // Mode 2: Subtract
                alpha_mode_results[2] = GBC_MAKE_COLOR(
                    GBC_SUB_FLOOR(GBC_GET_RED(pixel_color), GBC_GET_RED(new_pixel_color)),
                    GBC_SUB_FLOOR(GBC_GET_BLUE(pixel_color), GBC_GET_BLUE(new_pixel_color)),
                    GBC_SUB_FLOOR(GBC_GET_GREEN(pixel_color), GBC_GET_GREEN(new_pixel_color))
                );
                // Mode 3: Average
                alpha_mode_results[3] = GBC_MAKE_COLOR(
                    (GBC_GET_RED(pixel_color) + GBC_GET_RED(new_pixel_color)) >> 1,
                    (GBC_GET_BLUE(pixel_color) + GBC_GET_BLUE(new_pixel_color)) >> 1,
                    (GBC_GET_GREEN(pixel_color) + GBC_GET_GREEN(new_pixel_color)) >> 1
                );
                // Mode 4: AND
                alpha_mode_results[4] = GBC_MAKE_COLOR(
                    GBC_GET_RED(pixel_color) & GBC_GET_RED(new_pixel_color),
                    GBC_GET_BLUE(pixel_color) & GBC_GET_BLUE(new_pixel_color),
                    GBC_GET_GREEN(pixel_color) & GBC_GET_GREEN(new_pixel_color)
                );
                // Mode 5: OR
                alpha_mode_results[5] = GBC_MAKE_COLOR(
                    GBC_GET_RED(pixel_color) | GBC_GET_RED(new_pixel_color),
                    GBC_GET_BLUE(pixel_color) | GBC_GET_BLUE(new_pixel_color),
                    GBC_GET_GREEN(pixel_color) | GBC_GET_GREEN(new_pixel_color)
                );
                // Mode 6: XOR
                alpha_mode_results[6] = GBC_MAKE_COLOR(
                    GBC_GET_RED(pixel_color) ^ GBC_GET_RED(new_pixel_color),
                    GBC_GET_BLUE(pixel_color) ^ GBC_GET_BLUE(new_pixel_color),
                    GBC_GET_GREEN(pixel_color) ^ GBC_GET_GREEN(new_pixel_color)
                );
            #else
                // Mode 1: Add
                alpha_mode_results[1] = GBC_ADD_CEIL(pixel_color, new_pixel_color, 0b11);
                // Mode 2: Subtract
                alpha_mode_results[2] = GBC_SUB_FLOOR(pixel_color, new_pixel_color);
                // Mode 3: Average
                alpha_mode_results[3] = (pixel_color + new_pixel_color) >> 1;
                // Mode 4: AND
                alpha_mode_results[4] = pixel_color & new_pixel_color;
                // Mode 5: OR
                alpha_mode_results[5] = pixel_color | new_pixel_color;
                // Mode 6: XOR
                alpha_mode_results[6] = pixel_color ^ new_pixel_color;
            #endif

                // Now pick the appropriate alpha result
                pixel_color = alpha_mode_results[alpha_mode];
            }

            // Draw the sprite layer, but only the sprites on this line
            for (sprite_line_index = 0; sprite_line_index < num_sprites_on_this_line; sprite_line_index++) {
                // Get the sprite from OAM
                sprite_id = sprites_on_this_line[sprite_line_index];
                sprite = &self->oam[sprite_id*GBC_SPRITE_NUM_BYTES];

                // Check if the pixel we're rendering is within the sprite
                sprite_x = sprite[GBC_OAM_X_POS_BYTE] - GBC_SPRITE_OFFSET_X;
                sprite_y = sprite[GBC_OAM_Y_POS_BYTE] - GBC_SPRITE_OFFSET_Y;
                sprite_w = GBC_TILE_WIDTH << ((sprite[GBC_OAM_DIMS_BYTE] & GBC_OAM_SPRITE_WIDTH_MASK) >> GBC_OAM_SPRITE_WIDTH_SHIFT); // tile_width * (2 ^ sprite_width)
                sprite_h = GBC_TILE_HEIGHT << ((sprite[GBC_OAM_DIMS_BYTE] & GBC_OAM_SPRITE_HEIGHT_MASK) >> GBC_OAM_SPRITE_HEIGHT_SHIFT); // tile_height * (2 ^ sprite_height)

                // Find the pixel on the sprite
                x_on_sprite = x - sprite_x;
                y_on_sprite = self->line_y - sprite_y;

                // Apply flip flags
                flip_x = (sprite[GBC_OAM_ATTR_BYTE] & GBC_ATTR_FLIP_FLAG_X) >> 5; // Flip becomes 1
                x_on_sprite = x_on_sprite + (BOOL_MASK[flip_x] & (sprite_w - (x_on_sprite << 1) - 1)); // pixel_x = flip_x ? sprite_w - pixel_x - 1 : pixel_x
                flip_y = (sprite[GBC_OAM_ATTR_BYTE] & GBC_ATTR_FLIP_FLAG_Y) >> 6; // Flip becomes 1
                y_on_sprite = y_on_sprite + (BOOL_MASK[flip_y] & (sprite_h - (y_on_sprite << 1) - 1)); // pixel_y = flip_y ? sprite_w - pixel_y - 1 : pixel_y

                tile_x = x_on_sprite >> 3; // (x - sprite_x) / GBC_TILE_WIDTH (8)
                tile_y = y_on_sprite >> 3; // (y - sprite_y) / GBC_TILE_HEIGHT (8)
                sprite_tile_offset = tile_x + (tile_y << ((sprite[GBC_OAM_DIMS_BYTE] & GBC_OAM_SPRITE_WIDTH_MASK) >> GBC_OAM_SPRITE_WIDTH_SHIFT)); // tile_x + tile_y * (sprite_width)
                offset = (sprite[GBC_OAM_TILE_POS_BYTE] + sprite_tile_offset) << 5; // (tile_num + sprite_tile_offset) * GBC_TILE_NUM_BYTES (32)
                tile = self->vram + (((sprite[GBC_OAM_ATTR_BYTE] & GBC_ATTR_VRAM_BANK_MASK) >> 3) << 13) + offset; // self->vram + vram_bank_number * GBC_VRAM_BANK_NUM_BYTES (8192) + offset

                // Find the pixel on this tile
                pixel_x = x_on_sprite & 7; // tile_x % GBC_TILE_WIDTH (8)
                pixel_y = y_on_sprite & 7; // tile_y % GBC_TILE_HEIGHT (8)

                // To get the pixel, we first need to get the corresponding byte the pixel is in
                // There are 4 bytes per row (y * 4), and 2 pixels per byte (x / 2)
                offset = (pixel_y << 2) + (pixel_x >> 1); // pixel y * 4 + pixel_x / 2
                pixel_byte = tile[offset];

                // Once we have the byte, we need to get the 4 bit pixel out of the byte
                // This is achieved by shifting the byte (1 - x % 2) * (4 bits per pixel)
                shift = (1 ^ (pixel_x & 1)) << 2; // (1 - pixel_x % 2) * 4

                // We shift the byte and get rid of any unwanted bits
                pixel = 0b1111 & (pixel_byte >> shift);

                // Check if pixel is actually on the sprite
                pixel = pixel & BOOL_MASK[x >= sprite_x && x < (sprite_x + sprite_w)];

                // Hide pixel if sprites disabled
                pixel = pixel & BOOL_MASK[(self->lcdc & GBC_LCDC_SPRITE_ENABLE_FLAG) != 0];

                // Hide pixel if sprite is hidden
                pixel = pixel & BOOL_MASK[(sprite[GBC_OAM_ATTR_BYTE] & GBC_ATTR_HIDE_FLAG) == 0];

                new_pixel_color = self->sprite_palette_bank[((sprite[GBC_OAM_ATTR_BYTE] & GBC_ATTR_PALETTE_MASK) << 4) + pixel]; // (tile_attr & GBC_ATTR_PALETTE_MASK) * GBC_PALETTE_NUM_BYTES + pixel
                
                // Now replace the pixel if we have a color
                pixel_color = (pixel_color & BOOL_MASK[pixel == 0]) + (new_pixel_color & BOOL_MASK[pixel != 0]);
            }
            
            // And last, draw the backgrounds above the sprite layer
            for (bg_num = sprite_layer_z + 1; bg_num < self->num_backgrounds; bg_num++) {
                map_x = (x - self->screen_x_origin) + self->bg_scroll_x[bg_num];
                map_y = self->line_y + self->bg_scroll_y[bg_num];
                tile = self->vram + (bg_num << 10) + offset; // self->vram + vram_bank_number * GBC_VRAM_BANK_NUM_BYTES + offset
                tilemap = self->bg_tilemaps + (bg_num << 10); // self->bg_tilemaps + bg_num * GBC_TILEMAP_NUM_BYTES
                attrmap = self->bg_attrmaps + (bg_num << 10); // self->bg_attrmaps + bg_num * GBC_ATTRMAP_NUM_BYTES

                // Find the tile that the pixel is on
                map_tile_x = map_x >> 3; // map_x / GBC_TILE_WIDTH
                map_tile_y = map_y >> 3; // map_y / GBC_TILE_HEIGHT

                // Get the tile and attrs from the map
                tile_num = tilemap[map_tile_x + (map_tile_y << 5)]; // map_tile_y * MAP_WIDTH ??? what ???
                tile_attr = attrmap[map_tile_x + (map_tile_y << 5)];
                
                // Get the tile from vram
                offset = tile_num << 5; // tile_num * GBC_TILE_NUM_BYTES
                tile = self->vram + ((((tile_attr & GBC_ATTR_VRAM_BANK_MASK) >> 3)) << 13) + offset; // self->vram + vram_bank_number * GBC_VRAM_BANK_NUM_BYTES + offset

                // Next, we extract and return the 4bpp pixel from the tile
                pixel_x = map_x & 7; // map_x % GBC_TILE_WIDTH
                pixel_y = map_y & 7; // map_x % GBC_TILE_HEIGHT

                // Apply flip flags if necessary
                flip_x = (tile_attr & GBC_ATTR_FLIP_FLAG_X) != 0; // Flip becomes 1
                pixel_x = pixel_x + (BOOL_MASK[flip_x] & (GBC_TILE_WIDTH - (pixel_x << 1) - 1)); // pixel_x = flip_x ? sprite_w - pixel_x - 1 : pixel_x
                flip_y = (tile_attr & GBC_ATTR_FLIP_FLAG_Y) != 0; // Flip becomes 1
                pixel_y = pixel_y + (BOOL_MASK[flip_y] & (GBC_TILE_HEIGHT - (pixel_y << 1) - 1)); // pixel_y = flip_y ? sprite_w - pixel_y - 1 : pixel_y

                // To get the pixel, we first need to get the corresponding byte the pixel is in
                // There are 4 bytes per row (y * 4), and 2 pixels per byte (x / 2)
                offset = (pixel_y << 2) + (pixel_x >> 1); // pixel y * 4 + pixel_x / 2
                pixel_byte = tile[offset];

                // Once we have the byte, we need to get the 4 bit pixel out of the byte
                // This is achieved by shifting the byte (1 - x % 2) * (4 bits per pixel)
                shift = (1 ^ (pixel_x & 1)) << 2; // (1 - pixel_x % 2) * 4

                // We shift the byte and get rid of any unwanted bits
                pixel = 0b1111 & (pixel_byte >> shift);

                // Hide pixel if background disabled
                pixel = pixel & BOOL_MASK[(self->lcdc & (GBC_LCDC_BG_1_ENABLE_FLAG << bg_num)) != 0];

                // Hide pixel if tile is hidden
                pixel = pixel & BOOL_MASK[(tile_attr & GBC_ATTR_HIDE_FLAG) == 0];

                // Finally, we get the corresponding color from attribute palette
                new_pixel_color = self->bg_palette_bank[((tile_attr & GBC_ATTR_PALETTE_MASK) << 4) + pixel]; // (tile_attr & GBC_ATTR_PALETTE_MASK) * GBC_PALETTE_NUM_BYTES + pixel

                // Check if we should do an alpha blend
                alpha_enabled = (self->a_mode & (GBC_ALPHA_MODE_BG_ENABLED_FLAG << (bg_num << 2))) != 0;
                alpha_mode = (((self->a_mode & (GBC_ALPHA_MODE_MASK << (bg_num << 2))) >> (bg_num << 2)) >> GBC_ALPHA_MODE_SHIFT) & BOOL_MASK[alpha_enabled];
                
                // Create the alpha blends
                // Mode 0: Normal
                alpha_mode_results[0] = (pixel_color & BOOL_MASK[pixel == 0 && bg_num != 0]) + (new_pixel_color & BOOL_MASK[pixel != 0 || bg_num == 0]);
            #if defined(PBL_COLOR)
                // Mode 1: Add
                alpha_mode_results[1] = GBC_MAKE_COLOR(
                    GBC_ADD_CEIL(GBC_GET_RED(pixel_color), GBC_GET_RED(new_pixel_color), 0b11),
                    GBC_ADD_CEIL(GBC_GET_BLUE(pixel_color), GBC_GET_BLUE(new_pixel_color), 0b11),
                    GBC_ADD_CEIL(GBC_GET_GREEN(pixel_color), GBC_GET_GREEN(new_pixel_color), 0b11)
                );
                // Mode 2: Subtract
                alpha_mode_results[2] = GBC_MAKE_COLOR(
                    GBC_SUB_FLOOR(GBC_GET_RED(pixel_color), GBC_GET_RED(new_pixel_color)),
                    GBC_SUB_FLOOR(GBC_GET_BLUE(pixel_color), GBC_GET_BLUE(new_pixel_color)),
                    GBC_SUB_FLOOR(GBC_GET_GREEN(pixel_color), GBC_GET_GREEN(new_pixel_color))
                );
                // Mode 3: Average
                alpha_mode_results[3] = GBC_MAKE_COLOR(
                    (GBC_GET_RED(pixel_color) + GBC_GET_RED(new_pixel_color)) >> 1,
                    (GBC_GET_BLUE(pixel_color) + GBC_GET_BLUE(new_pixel_color)) >> 1,
                    (GBC_GET_GREEN(pixel_color) + GBC_GET_GREEN(new_pixel_color)) >> 1
                );
                // Mode 4: AND
                alpha_mode_results[4] = GBC_MAKE_COLOR(
                    GBC_GET_RED(pixel_color) & GBC_GET_RED(new_pixel_color),
                    GBC_GET_BLUE(pixel_color) & GBC_GET_BLUE(new_pixel_color),
                    GBC_GET_GREEN(pixel_color) & GBC_GET_GREEN(new_pixel_color)
                );
                // Mode 5: OR
                alpha_mode_results[5] = GBC_MAKE_COLOR(
                    GBC_GET_RED(pixel_color) | GBC_GET_RED(new_pixel_color),
                    GBC_GET_BLUE(pixel_color) | GBC_GET_BLUE(new_pixel_color),
                    GBC_GET_GREEN(pixel_color) | GBC_GET_GREEN(new_pixel_color)
                );
                // Mode 6: XOR
                alpha_mode_results[6] = GBC_MAKE_COLOR(
                    GBC_GET_RED(pixel_color) ^ GBC_GET_RED(new_pixel_color),
                    GBC_GET_BLUE(pixel_color) ^ GBC_GET_BLUE(new_pixel_color),
                    GBC_GET_GREEN(pixel_color) ^ GBC_GET_GREEN(new_pixel_color)
                );
            #else
                // Mode 1: Add
                alpha_mode_results[1] = GBC_ADD_CEIL(pixel_color, new_pixel_color, 0b11);
                // Mode 2: Subtract
                alpha_mode_results[2] = GBC_SUB_FLOOR(pixel_color, new_pixel_color);
                // Mode 3: Average
                alpha_mode_results[3] = (pixel_color + new_pixel_color) >> 1;
                // Mode 4: AND
                alpha_mode_results[4] = pixel_color & new_pixel_color;
                // Mode 5: OR
                alpha_mode_results[5] = pixel_color | new_pixel_color;
                // Mode 6: XOR
                alpha_mode_results[6] = pixel_color ^ new_pixel_color;
            #endif

                // Now pick the appropriate alpha result
                pixel_color = alpha_mode_results[alpha_mode];
            }

        #if defined(PBL_COLOR)
            memset(&info.data[x], pixel_color, 1);
        #else
            pixel_color = (pixel_color >> 1) & ((pixel_color & 1) | ((x + self->line_y) & 0b11));
            uint16_t byte = (x >> 3); // x / 8
            uint8_t bit = x & 7; // x % 8
            uint8_t *byte_mod = &info.data[byte];
            *byte_mod ^= (-pixel_color ^ *byte_mod) & (1 << bit);
        #endif

        }

        // Now we're in the HBlank state, run the callback
        self->stat |= GBC_STAT_HBLANK_FLAG;
        if (self->stat & GBC_STAT_HBLANK_INT_FLAG) {
            self->hblank_interrupt_callback(self);
        }
    }
    self->stat &= ~GBC_STAT_LINE_COMP_FLAG; // Clear line compare flag

    graphics_release_frame_buffer(ctx, fb);

    // Finished drawing sprites, call OAM callback
    self->stat |= GBC_STAT_OAM_FLAG;
    if (self->stat & GBC_STAT_OAM_INT_FLAG) {
        self->oam_interrupt_callback(self);
    }

    // Done drawing, now we're in VBlank, run the callback
    self->stat |= GBC_STAT_VBLANK_FLAG;
    if (self->stat & GBC_STAT_VBLANK_INT_FLAG) {
        self->vblank_interrupt_callback(self);
    }
}

void GBC_Graphics_render(GBC_Graphics *self) {
    layer_mark_dirty(self->graphics_layer); // All layers will be redrawn, so we don't need to mark the sprite layer dirty
}

/**
 * The update proc to call when the bg and window layers are dirty
 * 
 * @param layer A pointer to the target layer
 * @param ctx A pointer to the graphics context
 */
static void graphics_update_proc(Layer *layer, GContext *ctx) {
    render_graphics(*(GBC_Graphics * *)layer_get_data(layer), layer, ctx);
}

void GBC_Graphics_lcdc_set(GBC_Graphics *self, uint8_t new_lcdc) {
    self->lcdc = new_lcdc;
}

void GBC_Graphics_lcdc_set_enabled(GBC_Graphics *self, bool enabled) {
    modify_byte(&self->lcdc, GBC_LCDC_ENABLE_FLAG, enabled, GBC_LCDC_ENABLE_FLAG);
}

void GBC_Graphics_lcdc_set_bg_layer_enabled(GBC_Graphics *self, uint8_t bg_num, bool enabled) {
    modify_byte(&self->lcdc, GBC_LCDC_BG_1_ENABLE_FLAG << bg_num, enabled, GBC_LCDC_BG_1_ENABLE_FLAG << bg_num);
}

void GBC_Graphics_lcdc_set_sprite_layer_enabled(GBC_Graphics *self, bool enabled) {
    modify_byte(&self->lcdc, GBC_LCDC_SPRITE_ENABLE_FLAG, enabled, GBC_LCDC_SPRITE_ENABLE_FLAG);
}

void GBC_Graphics_lcdc_set_sprite_layer_z(GBC_Graphics *self, uint8_t layer_z) {
    modify_byte(&self->lcdc, GBC_LCDC_SPRITE_LAYER_Z_MASK, layer_z, GBC_LCDC_SPRITE_LAYER_Z_START);
}

void GBC_Graphics_alpha_mode_set_bg_enabled(GBC_Graphics *self, uint8_t bg_num, bool enabled) {
    modify_word(&self->a_mode, GBC_ALPHA_MODE_BG_ENABLED_FLAG << (bg_num * 4), enabled, GBC_ALPHA_MODE_BG_ENABLED_FLAG << (bg_num * 4));
}

void GBC_Graphics_alpha_mode_set_mode(GBC_Graphics *self, uint8_t bg_num, uint8_t mode) {
    modify_word(&self->a_mode, GBC_ALPHA_MODE_MASK << (bg_num * 4), mode, GBC_ALPHA_MODE_START << (bg_num * 4));
}


uint8_t GBC_Graphics_stat_get_current_line(GBC_Graphics *self) {
    return self->line_y;
}

uint8_t GBC_Graphics_stat_get_line_y_compare(GBC_Graphics *self) {
    return self->line_y_compare;
}

bool GBC_Graphics_stat_check_hblank_flag(GBC_Graphics *self) {
    return self->stat & GBC_STAT_HBLANK_FLAG;
}

bool GBC_Graphics_stat_check_vblank_flag(GBC_Graphics *self) {
    return self->stat & GBC_STAT_VBLANK_FLAG;
}

bool GBC_Graphics_stat_check_line_comp_flag(GBC_Graphics *self) {
    return self->stat & GBC_STAT_LINE_COMP_FLAG;
}

bool GBC_Graphics_stat_check_oam_flag(GBC_Graphics *self) {
    return self->stat & GBC_STAT_OAM_FLAG;
}

void GBC_Graphics_stat_set(GBC_Graphics *self, uint8_t new_stat) {
    modify_byte(&self->stat, GBC_STAT_WRITEABLE_MASK, new_stat & GBC_STAT_WRITEABLE_MASK, 1);
}

void GBC_Graphics_stat_set_hblank_interrupt_enabled(GBC_Graphics *self, bool enabled) {
    modify_byte(&self->stat, GBC_STAT_HBLANK_INT_FLAG, enabled, GBC_STAT_HBLANK_INT_FLAG);
}

void GBC_Graphics_stat_set_vblank_interrupt_enabled(GBC_Graphics *self, bool enabled) {
    modify_byte(&self->stat, GBC_STAT_VBLANK_INT_FLAG, enabled, GBC_STAT_VBLANK_INT_FLAG);
}

void GBC_Graphics_stat_set_line_compare_interrupt_enabled(GBC_Graphics *self, bool enabled) {
    modify_byte(&self->stat, GBC_STAT_LINE_COMP_INT_FLAG, enabled, GBC_STAT_LINE_COMP_INT_FLAG);
}

void GBC_Graphics_stat_set_oam_interrupt_enabled(GBC_Graphics *self, bool enabled) {
    modify_byte(&self->stat, GBC_STAT_OAM_INT_FLAG, enabled, GBC_STAT_OAM_INT_FLAG);
}

void GBC_Graphics_stat_set_line_y_compare(GBC_Graphics *self, uint8_t new_line_y_comp) {
    self->line_y_compare = new_line_y_comp;
}

void GBC_Graphics_set_hblank_interrupt_callback(GBC_Graphics *self, void (*callback)(GBC_Graphics *)) {
    self->hblank_interrupt_callback = callback;
}

void GBC_Graphics_set_vblank_interrupt_callback(GBC_Graphics *self, void (*callback)(GBC_Graphics *)) {
    self->vblank_interrupt_callback = callback;
}

void GBC_Graphics_set_line_compare_interrupt_callback(GBC_Graphics *self, void (*callback)(GBC_Graphics *)) {
    self->line_compare_interrupt_callback = callback;
}

void GBC_Graphics_set_oam_interrupt_callback(GBC_Graphics *self, void (*callback)(GBC_Graphics *)) {
    self->oam_interrupt_callback = callback;
}

uint8_t GBC_Graphics_attr_make(uint8_t palette, uint8_t vram_bank, bool is_x_flipped, bool is_y_flipped, bool is_hidden) {
    return (palette) | (vram_bank * GBC_ATTR_VRAM_BANK_START) | (is_x_flipped * GBC_ATTR_FLIP_FLAG_X) | (is_y_flipped * GBC_ATTR_FLIP_FLAG_Y) | (is_hidden * GBC_ATTR_HIDE_FLAG);
}

uint8_t GBC_Graphics_attr_get_palette_num(uint8_t attributes) {
    return attributes & GBC_ATTR_PALETTE_MASK;
}

uint8_t GBC_Graphics_attr_get_vram_bank(uint8_t attributes) {
    return (attributes & GBC_ATTR_VRAM_BANK_MASK) / GBC_ATTR_VRAM_BANK_START;
}

bool GBC_Graphics_attr_is_x_flipped(uint8_t attributes) {
    return (bool)(attributes & GBC_ATTR_FLIP_FLAG_X);
}

bool GBC_Graphics_attr_is_y_flipped(uint8_t attributes) {
    return (bool)(attributes & GBC_ATTR_FLIP_FLAG_Y);
}

bool GBC_Graphics_attr_is_hidden(uint8_t attributes) {
    return (bool)(attributes & GBC_ATTR_HIDE_FLAG);
}

uint8_t GBC_Graphics_bg_get_scroll_x(GBC_Graphics *self, uint8_t bg_layer) {
    return self->bg_scroll_x[bg_layer];
}

uint8_t GBC_Graphics_bg_get_scroll_y(GBC_Graphics *self, uint8_t bg_layer) {
    return self->bg_scroll_y[bg_layer];
}

uint8_t GBC_Graphics_bg_get_tile(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y) {
    return (self->bg_tilemaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(x, y)];
}

uint8_t GBC_Graphics_bg_get_attr(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y) {
    return (self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(x, y)];
}

void GBC_Graphics_bg_move(GBC_Graphics *self, uint8_t bg_layer, short dx, short dy) {
    short new_x = self->bg_scroll_x[bg_layer] + dx;
    short new_y = self->bg_scroll_y[bg_layer] + dy;

    if (new_x < 0) { // Wrap x 
        new_x = GBC_TILEMAP_WIDTH * GBC_TILE_WIDTH + new_x;
    } else {
        new_x = new_x % (GBC_TILEMAP_WIDTH * GBC_TILE_WIDTH);
    }

    if (new_y < 0) { // Wrap y
        new_y = GBC_TILEMAP_HEIGHT * GBC_TILE_HEIGHT + new_y;
    } else {
        new_y = new_y % (GBC_TILEMAP_HEIGHT * GBC_TILE_HEIGHT);
    }

    self->bg_scroll_x[bg_layer] = new_x;
    self->bg_scroll_y[bg_layer] = new_y;
}

void GBC_Graphics_bg_set_scroll_x(GBC_Graphics *self, uint8_t bg_layer, uint8_t x) {
    self->bg_scroll_x[bg_layer] = x;
}

void GBC_Graphics_bg_set_scroll_y(GBC_Graphics *self, uint8_t bg_layer, uint8_t y) {
    self->bg_scroll_y[bg_layer] = y;
}

void GBC_Graphics_bg_set_scroll_pos(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y) {
    self->bg_scroll_x[bg_layer] = x;
    self->bg_scroll_y[bg_layer] = y;
}

void GBC_Graphics_bg_set_tile(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, uint8_t tile_number) {
    (self->bg_tilemaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(x, y)] = tile_number;
}

void GBC_Graphics_bg_set_attrs(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, uint8_t attributes) {
    (self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(x, y)] = attributes;
}

void GBC_Graphics_bg_set_tile_and_attrs(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, uint8_t tile_number, uint8_t attributes) {
    (self->bg_tilemaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(x, y)] = tile_number;
    (self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(x, y)] = attributes;
}

void GBC_Graphics_bg_set_tile_palette(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, uint8_t palette) {
    modify_byte(&(self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(x, y)], GBC_ATTR_PALETTE_MASK, palette, GBC_ATTR_PALETTE_START);
}

void GBC_Graphics_bg_set_tile_vram_bank(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, uint8_t vram_bank) {
    modify_byte(&(self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(x, y)], GBC_ATTR_VRAM_BANK_MASK, vram_bank, GBC_ATTR_VRAM_BANK_START);
}

void GBC_Graphics_bg_set_tile_x_flip(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, bool flipped) {
    modify_byte(&(self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(x, y)], GBC_ATTR_FLIP_FLAG_X, flipped, GBC_ATTR_FLIP_FLAG_X);
}

void GBC_Graphics_bg_set_tile_y_flip(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, bool flipped) {
    modify_byte(&(self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(x, y)], GBC_ATTR_FLIP_FLAG_Y, flipped, GBC_ATTR_FLIP_FLAG_Y);
}

void GBC_Graphics_bg_set_tile_hidden(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, bool hidden) {
    modify_byte(&(self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(x, y)], GBC_ATTR_HIDE_FLAG, hidden, GBC_ATTR_HIDE_FLAG);
}

void GBC_Graphics_bg_move_tile(GBC_Graphics *self, uint8_t bg_layer, uint8_t src_x, uint8_t src_y, uint8_t dest_x, uint8_t dest_y, bool swap) {
    uint8_t src_tile = (self->bg_tilemaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(src_x, src_y)];
    uint8_t src_attr = (self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(src_x, src_y)];
    uint8_t dest_tile = (self->bg_tilemaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(dest_x, dest_y)];
    uint8_t dest_attr = (self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(dest_x, dest_y)];
    (self->bg_tilemaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(dest_x, dest_y)] = src_tile;
    (self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(dest_x, dest_y)] = src_attr;

    if (swap) {
        (self->bg_tilemaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(src_x, src_y)] = dest_tile;
        (self->bg_attrmaps + bg_layer * GBC_TILEMAP_NUM_BYTES)[GBC_POINT_TO_OFFSET(src_x, src_y)] = dest_attr;
    }
}

uint8_t GBC_Graphics_oam_get_sprite_x(GBC_Graphics *self, uint8_t sprite_num) {
    return self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_X_POS_BYTE];
}

uint8_t GBC_Graphics_oam_get_sprite_y(GBC_Graphics *self, uint8_t sprite_num) {
    return self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_Y_POS_BYTE];
}

uint8_t GBC_Graphics_oam_get_sprite_tile(GBC_Graphics *self, uint8_t sprite_num) {
    return self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_TILE_POS_BYTE];
}

uint8_t GBC_Graphics_oam_get_sprite_attrs(GBC_Graphics *self, uint8_t sprite_num) {
    return self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_ATTR_BYTE];
}

uint8_t GBC_Graphics_oam_get_sprite_width(GBC_Graphics *self, uint8_t sprite_num) {
    return (self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_DIMS_BYTE] & GBC_OAM_SPRITE_WIDTH_MASK) >> GBC_OAM_SPRITE_WIDTH_SHIFT;
}

uint8_t GBC_Graphics_oam_get_sprite_height(GBC_Graphics *self, uint8_t sprite_num) {
    return (self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_DIMS_BYTE] & GBC_OAM_SPRITE_HEIGHT_MASK) >> GBC_OAM_SPRITE_HEIGHT_SHIFT;
}

void GBC_Graphics_oam_set_sprite(GBC_Graphics *self, uint8_t sprite_num, uint8_t x, uint8_t y, uint8_t tile_position, uint8_t attributes, uint8_t width, uint8_t height) {
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_X_POS_BYTE] = x;
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_Y_POS_BYTE] = y;
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_TILE_POS_BYTE] = tile_position;
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_ATTR_BYTE] = attributes;
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_DIMS_BYTE] = (
        ((width & (GBC_OAM_SPRITE_WIDTH_MASK >> GBC_OAM_SPRITE_WIDTH_SHIFT)) << GBC_OAM_SPRITE_WIDTH_SHIFT) |
        ((height & (GBC_OAM_SPRITE_HEIGHT_MASK >> GBC_OAM_SPRITE_HEIGHT_SHIFT)) << GBC_OAM_SPRITE_HEIGHT_SHIFT));
}

void GBC_Graphics_oam_move_sprite(GBC_Graphics *self, uint8_t sprite_num, short dx, short dy) {
    short new_x, new_y;
    new_x = self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_X_POS_BYTE] + dx;
    new_y = self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_Y_POS_BYTE] + dy;
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_X_POS_BYTE] = new_x;
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_Y_POS_BYTE] = new_y;
}

void GBC_Graphics_oam_set_sprite_x(GBC_Graphics *self, uint8_t sprite_num, uint8_t x) {
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_X_POS_BYTE] = x;
}

void GBC_Graphics_oam_set_sprite_y(GBC_Graphics *self, uint8_t sprite_num, uint8_t y) {
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_Y_POS_BYTE] = y;
}

void GBC_Graphics_oam_set_sprite_pos(GBC_Graphics *self, uint8_t sprite_num, uint8_t x, uint8_t y) {
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_X_POS_BYTE] = x;
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_Y_POS_BYTE] = y;
}

void GBC_Graphics_oam_set_sprite_tile(GBC_Graphics *self, uint8_t sprite_num, uint8_t tile_position) {
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_TILE_POS_BYTE] = tile_position;
}

void GBC_Graphics_oam_set_sprite_attrs(GBC_Graphics *self, uint8_t sprite_num, uint8_t attributes) {
    self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_ATTR_BYTE] = attributes;
}

void GBC_Graphics_oam_set_sprite_palette(GBC_Graphics *self, uint8_t sprite_num, uint8_t palette) {
    modify_byte(&self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_ATTR_BYTE], GBC_ATTR_PALETTE_MASK, palette, GBC_ATTR_PALETTE_START);
}

void GBC_Graphics_oam_set_sprite_vram_bank(GBC_Graphics *self, uint8_t sprite_num, uint8_t vram_bank) {
    modify_byte(&self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_ATTR_BYTE], GBC_ATTR_VRAM_BANK_MASK, vram_bank, GBC_ATTR_VRAM_BANK_START);
}

void GBC_Graphics_oam_set_sprite_x_flip(GBC_Graphics *self, uint8_t sprite_num, bool flipped) {
    modify_byte(&self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_ATTR_BYTE], GBC_ATTR_FLIP_FLAG_X, flipped, GBC_ATTR_FLIP_FLAG_X);
}

void GBC_Graphics_oam_set_sprite_y_flip(GBC_Graphics *self, uint8_t sprite_num, bool flipped) {
    modify_byte(&self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_ATTR_BYTE], GBC_ATTR_FLIP_FLAG_Y, flipped, GBC_ATTR_FLIP_FLAG_Y);
}

void GBC_Graphics_oam_set_sprite_hidden(GBC_Graphics *self, uint8_t sprite_num, bool hidden) {
    modify_byte(&self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_ATTR_BYTE], GBC_ATTR_HIDE_FLAG, hidden, GBC_ATTR_HIDE_FLAG);
}

void GBC_Graphics_oam_set_sprite_width(GBC_Graphics *self, uint8_t sprite_num, uint8_t width) {
    modify_byte(&self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_DIMS_BYTE], GBC_OAM_SPRITE_WIDTH_MASK, width, GBC_OAM_SPRITE_WIDTH_START);
}

void GBC_Graphics_oam_set_sprite_height(GBC_Graphics *self, uint8_t sprite_num, uint8_t height) {
    modify_byte(&self->oam[sprite_num * GBC_SPRITE_NUM_BYTES + GBC_OAM_DIMS_BYTE], GBC_OAM_SPRITE_HEIGHT_MASK, height, GBC_OAM_SPRITE_HEIGHT_START);
}

void GBC_Graphics_oam_change_sprite_num(GBC_Graphics *self, uint8_t source_sprite_num, uint8_t target_sprite_num, bool copy) {
    uint8_t *source = &self->oam[source_sprite_num * GBC_SPRITE_NUM_BYTES];
    uint8_t *target = &self->oam[target_sprite_num * GBC_SPRITE_NUM_BYTES];
    for (uint8_t i = 0; i < GBC_SPRITE_NUM_BYTES; i++) {
            target[i] = source[i];
        if (!copy) {
            source[i] = 0;
        }
    }
}

void GBC_Graphics_oam_swap_sprites(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2) {
    uint8_t temp;
    uint8_t *source_1 = &self->oam[sprite_num_1 * GBC_SPRITE_NUM_BYTES];
    uint8_t *source_2 = &self->oam[sprite_num_2 * GBC_SPRITE_NUM_BYTES];
    for (uint8_t i = 0; i < GBC_SPRITE_NUM_BYTES; i++) {
        temp = source_1[i];
        source_1[i] = source_2[i];
        source_2[i] = temp;
    }
}

void GBC_Graphics_oam_swap_sprite_tiles(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2) {
    uint8_t temp;
    uint8_t *source_1 = &self->oam[sprite_num_1 * GBC_SPRITE_NUM_BYTES];
    uint8_t *source_2 = &self->oam[sprite_num_2 * GBC_SPRITE_NUM_BYTES];
    temp = source_1[GBC_OAM_TILE_POS_BYTE];
    source_1[GBC_OAM_TILE_POS_BYTE] = source_2[GBC_OAM_TILE_POS_BYTE];
    source_2[GBC_OAM_TILE_POS_BYTE] = temp;
}

void GBC_Graphics_oam_swap_sprite_attrs(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2) {
    uint8_t temp;
    uint8_t *source_1 = &self->oam[sprite_num_1 * GBC_SPRITE_NUM_BYTES];
    uint8_t *source_2 = &self->oam[sprite_num_2 * GBC_SPRITE_NUM_BYTES];
    temp = source_1[GBC_OAM_ATTR_BYTE];
    source_1[GBC_OAM_ATTR_BYTE] = source_2[GBC_OAM_ATTR_BYTE];
    source_2[GBC_OAM_ATTR_BYTE] = temp;
}

void GBC_Graphics_oam_swap_sprite_tiles_and_attrs(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2) {
    GBC_Graphics_oam_swap_sprite_tiles(self, sprite_num_1, sprite_num_2);
    GBC_Graphics_oam_swap_sprite_attrs(self, sprite_num_1, sprite_num_2);
}

void GBC_Graphics_copy_background(GBC_Graphics *self, uint8_t source_bg_layer, uint8_t target_bg_layer) {
    memcpy((self->bg_tilemaps + source_bg_layer * GBC_TILEMAP_NUM_BYTES), (self->bg_tilemaps + target_bg_layer * GBC_TILEMAP_NUM_BYTES), GBC_TILEMAP_NUM_BYTES);
    memcpy((self->bg_attrmaps + source_bg_layer * GBC_ATTRMAP_NUM_BYTES), (self->bg_attrmaps + target_bg_layer * GBC_ATTRMAP_NUM_BYTES), GBC_ATTRMAP_NUM_BYTES);
}