#include "pebble-gbc-graphics-advanced.h"
#include <stdarg.h>

///> Forward declarations for static functions
static void graphics_update_proc(Layer *layer, GContext *ctx);

GBC_Graphics *GBC_Graphics_ctor(Window *window, uint8_t num_vram_banks) { 
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
    self->bg_tilemaps = (uint8_t*)malloc(GBC_TILEMAP_NUM_BYTES * GBC_NUM_BG_LAYERS);
    self->bg_attrmaps = (uint8_t*)malloc(GBC_ATTRMAP_NUM_BYTES * GBC_NUM_BG_LAYERS);

    // Allocate space for the palette banks
    self->bg_palette_bank = (uint8_t*)malloc(GBC_PALETTE_BANK_NUM_BYTES);
    self->sprite_palette_bank = (uint8_t*)malloc(GBC_PALETTE_BANK_NUM_BYTES);

    // TOOD: Scrollx and scroll y   
    self->bg_scroll_x = (short*)malloc(GBC_NUM_BG_LAYERS * sizeof(short));
    self->bg_scroll_y = (short*)malloc(GBC_NUM_BG_LAYERS * sizeof(short));

    // Allocate space for the OAM
    self->oam = (uint8_t*)malloc(GBC_OAM_NUM_BYTES);

    self->lcdc = 0xFF; // Start LCDC with everything enable (render everything)
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
 * Clamps a short variable between two uint8_t values
 * 
 * @param to_clamp The value to clamp
 * @param lower_bound The lower bound on the return value
 * @param uppper_bound The upper bound on the return value
 * 
 * @return The clamped value as uint8_t
 */
static uint8_t clamp_short_to_uint8_t(short to_clamp, uint8_t lower_bound, uint8_t upper_bound) {
    if (to_clamp < (short)lower_bound) {
        return lower_bound;
    }
    if (to_clamp > (short)upper_bound) {
        return upper_bound;
    }
    return (uint8_t) to_clamp;
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
 * Renders the backgrounds and sprites called from an update proc
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param layer A pointer to the layer to modify
 * @param ctx The graphics context for drawing
 */
static void render_graphics(GBC_Graphics *self, Layer *layer, GContext *ctx) {
    // Return early if we don't need to render the backgrounds or sprites
    if (!(self->lcdc & GBC_LCDC_ENABLE_FLAG) || (!(self->lcdc & GBC_LCDC_BCKGND_ENABLE_FLAG) && !(self->lcdc & GBC_LCDC_SPRITE_ENABLE_FLAG))) {
        return;
    }
    GBitmap *fb = graphics_capture_frame_buffer(ctx);

    // Predefine the variables we'll use in the loop
    uint8_t map_x, map_y, map_tile_x, map_tile_y, tile_num, tile_attr;
    uint8_t *tilemap, *attrmap;
    uint16_t offset;
    uint8_t *tile;
    uint8_t pixel_x, pixel_y, pixel_byte, pixel_color, pixel;
    uint8_t shift;
    uint8_t x;
    uint8_t flip;
    short bg_num;
    
    short screen_x, screen_y;
    uint8_t tile_x, tile_y;
    uint8_t bg_tile_num, bg_tile_attr;
    uint8_t *sprite, *bg_tile;
    uint8_t num_overlapped_sprites;
    uint8_t overlapped_sprites[GBC_NUM_SPRITES];
    bool line_overlap;
    uint8_t i;
    uint8_t sprite_y;
    short sprite_id;

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

        self->stat &= ~GBC_STAT_HBLANK_FLAG; // No longer in HBlank while we draw the line
        // First, draw the backgrounds
        for(x = min_x; x < max_x; x++) {
            // Decide what pixel to draw, in reverse bg order
            for (bg_num = GBC_NUM_BG_LAYERS - 1; bg_num >= 0; bg_num--) {
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
                flip = tile_attr & GBC_ATTR_FLIP_FLAG_X;
                pixel_x = ((pixel_x >> (flip >> 3)) - ((pixel_x + 1) >> ((GBC_ATTR_FLIP_FLAG_X ^ flip) >> 3))) & 7; // flip ? 7 - pixel_x : pixel_x
                flip = tile_attr & GBC_ATTR_FLIP_FLAG_Y;
                pixel_y = ((pixel_y >> (flip >> 3)) - ((pixel_y + 1) >> ((GBC_ATTR_FLIP_FLAG_Y ^ flip) >> 3))) & 7;

                // To get the pixel, we first need to get the corresponding byte the pixel is in
                // There are 4 bytes per row (y * 4), and 2 pixels per byte (x / 2)
                offset = (pixel_y << 2) + (pixel_x >> 1); // pixel y * 4 + pixel_x / 2
                pixel_byte = tile[offset];

                // Once we have the byte, we need to get the 4 bit pixel out of the byte
                // This is achieved by shifting the byte (1 - x % 2) * (4 bits per pixel)
                shift = (1 ^ (pixel_x & 1)) << 2; // (1 - pixel_x % 2) * 4

                // We shift the byte and get rid of any unwanted bits
                pixel = 0b1111 & (pixel_byte >> shift);
                
                // The 0th palette color is transparency, move to lower layer
                if (pixel == 0) {
                    continue;
                }

                // Finally, we get the corresponding color from attribute palette
                pixel_color = self->bg_palette_bank[((tile_attr & GBC_ATTR_PALETTE_MASK) << 4) + pixel]; // (tile_attr & GBC_ATTR_PALETTE_MASK) * GBC_PALETTE_NUM_BYTES + pixel

            #if defined(PBL_COLOR)
                memset(&info.data[x], pixel_color, 1);
            #else
                pixel_color = (pixel_color >> 1) & ((pixel_color & 1) | ((x + self->line_y) & 0b11));
                uint16_t byte = (x >> 3); // x / 8
                uint8_t bit = x & 7; // x % 8
                uint8_t *byte_mod = &info.data[byte];
                *byte_mod ^= (-pixel_color ^ *byte_mod) & (1 << bit);
            #endif
                break;
            }
        }

        // Next, draw any sprites that happen to be on this line
        num_overlapped_sprites = 0;
        for (sprite_id = (GBC_NUM_SPRITES - 1) * ((self->lcdc & GBC_LCDC_SPRITE_ENABLE_FLAG) > 0); sprite_id >= 0; sprite_id--) {
            sprite = &self->oam[sprite_id*4];
            sprite_y = sprite[1] - GBC_SPRITE_OFFSET_Y;
            line_overlap = ((uint8_t)(self->line_y - sprite_y)) < (GBC_TILE_HEIGHT << ((self->lcdc & GBC_LCDC_SPRITE_SIZE_FLAG) > 0));
            
            overlapped_sprites[num_overlapped_sprites] = sprite_id * line_overlap;
            num_overlapped_sprites += line_overlap;
        }

        for (i = 0; i < num_overlapped_sprites; i++) {
            sprite = &self->oam[overlapped_sprites[i]*4];
            sprite_y = sprite[1] - GBC_SPRITE_OFFSET_Y;
            // Don't draw the sprite if it's offscreen
            if (sprite[0] == 0 || sprite[1] == 0 || sprite[0] >= self->screen_width + GBC_SPRITE_OFFSET_X || sprite[1] >= self->screen_height + GBC_SPRITE_OFFSET_Y) {
                continue;
            }

            // Get the tile from vram
            offset = sprite[2] << 5; // tile_num * GBC_TILE_NUM_BYTES
            tile = self->vram + ((((sprite[3] & GBC_ATTR_VRAM_BANK_MASK) >> 3)) << 13) + offset; // self->vram + vram_bank_number * GBC_VRAM_BANK_NUM_BYTES + offset

            // Then we draw the tile row by row
            sprite_y = sprite[1] - GBC_SPRITE_OFFSET_Y;
            tile_y = self->line_y - sprite_y;

            uint8_t min_x = GBC_MAX(info.min_x, self->screen_x_origin);
            uint8_t max_x = GBC_MIN(info.max_x, self->screen_x_origin + self->screen_width);

            for (tile_x = 0; tile_x < GBC_TILE_WIDTH; tile_x++) {
                // Check if the pixel is on the screen
                screen_x = sprite[0] + tile_x - GBC_SPRITE_OFFSET_X + self->screen_x_origin;
                if (screen_x > max_x) {
                    break;
                }
                if (screen_x < min_x) {
                    continue;
                }
        
                // Now we get the pixel from the sprite tile
                pixel_x = tile_x & 7; // tile_x % GBC_TILE_WIDTH
                pixel_y = tile_y & (7 + (((self->lcdc & GBC_LCDC_SPRITE_SIZE_FLAG) > 0) << 3)); // tile_y % (8x16 sprite ? GBC_TILE_HEIGHT : GBC_TILE_HEIGHT * 2)

                // Apply flip flags if necessary
                flip = sprite[3] & GBC_ATTR_FLIP_FLAG_X;
                pixel_x = ((pixel_x >> (flip >> 3)) - ((pixel_x + 1) >> ((GBC_ATTR_FLIP_FLAG_X ^ flip) >> 3))) & 7;
                flip = sprite[3] & GBC_ATTR_FLIP_FLAG_Y;
                pixel_y = ((pixel_y >> (flip >> 3)) - ((pixel_y + 1) >> ((GBC_ATTR_FLIP_FLAG_Y ^ flip) >> 3))) & (7 + ((self->lcdc & GBC_LCDC_SPRITE_SIZE_FLAG) > 0) * 8);

                // To get the pixel, we first need to get the corresponding byte the pixel is in
                // There are 4 bytes per row (y * 4), and 2 pixels per byte (x / 2)
                offset = (pixel_y << 2) + (pixel_x >> 1); // pixel y * 4 + pixel_x / 2
                pixel_byte = tile[offset];

                // Once we have the byte, we need to get the 4 bit pixel out of the byte
                // This is achieved by shifting the byte (1 - x % 2) * (4 bits per pixel)
                shift = (1 ^ (pixel_x & 1)) << 2; // (1 - pixel_x % 2) * 4

                // We shift the byte and get rid of any unwanted bits
                pixel = 0b1111 & (pixel_byte >> shift);

                // The 0th palette color is transparency
                if (pixel == 0) {
                    continue;
                }
        
                pixel_color = self->sprite_palette_bank[((sprite[3] & GBC_ATTR_PALETTE_MASK) << 4) + pixel]; // (tile_attr & GBC_ATTR_PALETTE_MASK) * GBC_PALETTE_NUM_BYTES + pixel
        
            #if defined(PBL_COLOR)
                memset(&info.data[screen_x], pixel_color, 1);
            #else
                pixel_color = (pixel_color >> 1) & ((pixel_color & 1) | ((screen_x + self->line_y) & 0b11));
                uint16_t byte = ((screen_x) >> 3);
                uint8_t bit = screen_x & 7; // x % 8
                uint8_t *byte_mod = &info.data[byte];
                *byte_mod ^= (-pixel_color ^ *byte_mod) & (1 << bit);
            #endif
            }
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

void GBC_Graphics_lcdc_set_bg_layer_enabled(GBC_Graphics *self, bool enabled) {
    modify_byte(&self->lcdc, GBC_LCDC_BCKGND_ENABLE_FLAG, enabled, GBC_LCDC_BCKGND_ENABLE_FLAG);
}

void GBC_Graphics_lcdc_set_sprite_layer_enabled(GBC_Graphics *self, bool enabled) {
    modify_byte(&self->lcdc, GBC_LCDC_SPRITE_ENABLE_FLAG, enabled, GBC_LCDC_SPRITE_ENABLE_FLAG);
}

void GBC_Graphics_lcdc_set_8x16_sprite_mode_enabled(GBC_Graphics *self, bool use_8x16_sprites) {
    modify_byte(&self->lcdc, GBC_LCDC_SPRITE_SIZE_FLAG, use_8x16_sprites, GBC_LCDC_SPRITE_SIZE_FLAG);
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

uint8_t GBC_Graphics_attr_make(uint8_t palette, uint8_t vram_bank, bool is_x_flipped, bool is_y_flipped) {
    return (palette) | (vram_bank * GBC_ATTR_VRAM_BANK_START) | (is_x_flipped * GBC_ATTR_FLIP_FLAG_X) | (is_y_flipped * GBC_ATTR_FLIP_FLAG_Y);
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
    return self->oam[sprite_num*4+0];
}

uint8_t GBC_Graphics_oam_get_sprite_y(GBC_Graphics *self, uint8_t sprite_num) {
    return self->oam[sprite_num*4+1];
}

uint8_t GBC_Graphics_oam_get_sprite_tile(GBC_Graphics *self, uint8_t sprite_num) {
    return self->oam[sprite_num*4+2];
}

uint8_t GBC_Graphics_oam_get_sprite_attrs(GBC_Graphics *self, uint8_t sprite_num) {
    return self->oam[sprite_num*4+3];
}

void GBC_Graphics_oam_set_sprite(GBC_Graphics *self, uint8_t sprite_num, uint8_t x, uint8_t y, uint8_t tile_position, uint8_t attributes) {
    self->oam[sprite_num*4+0] = x;
    self->oam[sprite_num*4+1] = y;
    self->oam[sprite_num*4+2] = tile_position;
    self->oam[sprite_num*4+3] = attributes;
}

void GBC_Graphics_oam_move_sprite(GBC_Graphics *self, uint8_t sprite_num, short dx, short dy) {
    short new_x, new_y;
    new_x = self->oam[sprite_num*4+0] + dx;
    new_y = self->oam[sprite_num*4+1] + dy;
    self->oam[sprite_num*4+0] = new_x;
    self->oam[sprite_num*4+1] = new_y;
}

void GBC_Graphics_oam_set_sprite_x(GBC_Graphics *self, uint8_t sprite_num, uint8_t x) {
    self->oam[sprite_num*4+0] = x;
}

void GBC_Graphics_oam_set_sprite_y(GBC_Graphics *self, uint8_t sprite_num, uint8_t y) {
    self->oam[sprite_num*4+1] = y;
}

void GBC_Graphics_oam_set_sprite_pos(GBC_Graphics *self, uint8_t sprite_num, uint8_t x, uint8_t y) {
    self->oam[sprite_num*4+0] = x;
    self->oam[sprite_num*4+1] = y;
}

void GBC_Graphics_oam_hide_sprite(GBC_Graphics *self, uint8_t sprite_num) {
    self->oam[sprite_num*4+0] = 0;
    self->oam[sprite_num*4+1] = 0;
}

void GBC_Graphics_oam_set_sprite_tile(GBC_Graphics *self, uint8_t sprite_num, uint8_t tile_position) {
    self->oam[sprite_num*4+2] = tile_position;
}

void GBC_Graphics_oam_set_sprite_attrs(GBC_Graphics *self, uint8_t sprite_num, uint8_t attributes) {
    self->oam[sprite_num*4+3] = attributes;
}

void GBC_Graphics_oam_set_sprite_palette(GBC_Graphics *self, uint8_t sprite_num, uint8_t palette) {
    modify_byte(&self->oam[sprite_num * 4 + 3], GBC_ATTR_PALETTE_MASK, palette, GBC_ATTR_PALETTE_START);
}

void GBC_Graphics_oam_set_sprite_vram_bank(GBC_Graphics *self, uint8_t sprite_num, uint8_t vram_bank) {
    modify_byte(&self->oam[sprite_num * 4 + 3], GBC_ATTR_VRAM_BANK_MASK, vram_bank, GBC_ATTR_VRAM_BANK_START);
}

void GBC_Graphics_oam_set_sprite_x_flip(GBC_Graphics *self, uint8_t sprite_num, bool flipped) {
    modify_byte(&self->oam[sprite_num * 4 + 3], GBC_ATTR_FLIP_FLAG_X, flipped, GBC_ATTR_FLIP_FLAG_X);
}

void GBC_Graphics_oam_set_sprite_y_flip(GBC_Graphics *self, uint8_t sprite_num, bool flipped) {
    modify_byte(&self->oam[sprite_num * 4 + 3], GBC_ATTR_FLIP_FLAG_Y, flipped, GBC_ATTR_FLIP_FLAG_Y);
}

void GBC_Graphics_oam_change_sprite_num(GBC_Graphics *self, uint8_t source_sprite_num, uint8_t target_sprite_num, bool copy) {
    uint8_t *source = &self->oam[source_sprite_num*4];
    uint8_t *target = &self->oam[target_sprite_num*4];
    for (uint8_t i = 0; i < 4; i++) {
            target[i] = source[i];
        if (!copy) {
            source[i] = 0;
        }
    }
}

void GBC_Graphics_oam_swap_sprites(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2) {
    uint8_t temp;
    uint8_t *source_1 = &self->oam[sprite_num_1*4];
    uint8_t *source_2 = &self->oam[sprite_num_2*4];
    for (uint8_t i = 0; i < 4; i++) {
        temp = source_1[i];
        source_1[i] = source_2[i];
        source_2[i] = temp;
    }
}

void GBC_Graphics_oam_swap_sprite_tiles(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2) {
    uint8_t temp;
    uint8_t *source_1 = &self->oam[sprite_num_1*4];
    uint8_t *source_2 = &self->oam[sprite_num_2*4];
    uint8_t i = 2;
    temp = source_1[i];
    source_1[i] = source_2[i];
    source_2[i] = temp;
}

void GBC_Graphics_oam_swap_sprite_attrs(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2) {
    uint8_t temp;
    uint8_t *source_1 = &self->oam[sprite_num_1*4];
    uint8_t *source_2 = &self->oam[sprite_num_2*4];
    uint8_t i = 3;
    temp = source_1[i];
    source_1[i] = source_2[i];
    source_2[i] = temp;
}

void GBC_Graphics_oam_swap_sprite_tiles_and_attrs(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2) {
    uint8_t temp;
    uint8_t *source_1 = &self->oam[sprite_num_1*4];
    uint8_t *source_2 = &self->oam[sprite_num_2*4];
    for (uint8_t i = 2; i < 4; i++) {
        temp = source_1[i];
        source_1[i] = source_2[i];
        source_2[i] = temp;
    }
}

void GBC_Graphics_copy_background(GBC_Graphics *self, uint8_t source_bg_layer, uint8_t target_bg_layer) {
    memcpy((self->bg_tilemaps + source_bg_layer * GBC_TILEMAP_NUM_BYTES), (self->bg_tilemaps + target_bg_layer * GBC_TILEMAP_NUM_BYTES), GBC_TILEMAP_NUM_BYTES);
    memcpy((self->bg_attrmaps + source_bg_layer * GBC_ATTRMAP_NUM_BYTES), (self->bg_attrmaps + target_bg_layer * GBC_ATTRMAP_NUM_BYTES), GBC_ATTRMAP_NUM_BYTES);
}