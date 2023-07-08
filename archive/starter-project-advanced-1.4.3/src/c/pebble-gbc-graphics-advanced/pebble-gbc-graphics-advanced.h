#pragma once         ///> Prevent errors by being included multiple times

#include <pebble.h>  ///> Pebble SDK symbols

/**
 * This is a graphics engine that renders Game Boy Color style graphics
 * on the Pebble smartwatch, with some Game Boy Advance style modifications
 * @file pebble-gbc-graphics-advanced.h
 * @author Harrison Allen
 * @version 1.4.3 7/4/2023
 * 
 * Questions? Feel free to send me an email at harrisonallen555@gmail.com
 */

/** Size definitions */
#define GBC_TILE_WIDTH 8  ///> Width of a tile in pixels
#define GBC_TILE_HEIGHT 8 ///> Height of a tile in pixels
/**
 * Number of bytes that one tile takes up, calculated by:
 * 4 bits per pixel * 8 pixels wide * 8 pixels tall = 256 bits
 * 256 bits / 8 bits per byte = 32 bytes
 */
#define GBC_TILE_NUM_BYTES 32
#define GBC_VRAM_BANK_NUM_TILES 256 ///> The number of tiles per VRAM bank
/**
 * Number of bytes per VRAM bank, calculated by:
 * 256 tiles per bank * 32 bytes per tile = 8192 bytes
 */
#define GBC_VRAM_BANK_NUM_BYTES 8192
#define GBC_TILEMAP_WIDTH 32  ///> Width of the background layers in tiles
#define GBC_TILEMAP_HEIGHT 32 ///> Height of the background layers in tiles
/**
 * Size of the tilemap in bytes, calculated by:
 * 1 byte per tile location * 32 tiles wide * 32 tiles tall = 1024 bytes
 */
#define GBC_TILEMAP_NUM_BYTES 1024
/**
 * Size of the attributemap in bytes, calculated by:
 * 1 byte per attribute * 32 tiles wide * 32 tiles tall = 1024 bytes
 */
#define GBC_ATTRMAP_NUM_BYTES 1024
/**
 * The size of one palette, calculated by:
 * 1 byte per color * 16 colors per palette = 16 bytes
 */
#define GBC_PALETTE_NUM_BYTES 16
#define GBC_PALETTE_NUM_PALETTES 8 ///> The number of palettes
/**
 * The size of one palette bank, calculated by:
 * 16 bytes per palette * 8 palettes = 128 bytes
 */
#define GBC_PALETTE_BANK_NUM_BYTES 128
#define GBC_NUM_SPRITES 40 ///> The maximum number of sprites
#define GBC_SPRITE_NUM_BYTES 5 ///> The number of bytes per sprite
/**
 * The size of the OAM, calculated by:
 * 5 bytes per sprite * 40 sprite slots = 160 bytes
 */
#define GBC_OAM_NUM_BYTES 200
#define GBC_OAM_X_POS_BYTE 0    ///> The OAM x byte offset
#define GBC_OAM_Y_POS_BYTE 1    ///> The OAM y byte offset
#define GBC_OAM_TILE_POS_BYTE 2 ///> The OAM VRAM tile pos byte offset
#define GBC_OAM_ATTR_BYTE 3     ///> The OAM ATTR byte offset
#define GBC_OAM_DIMS_BYTE 4     ///> The OAM dimensions byte offset
#define GBC_SPRITE_OFFSET_X 64  ///> The x offset to allow for offscreen rendering
#define GBC_SPRITE_OFFSET_Y 64  ///> The y offset to allow for offscreen rendering

/** Attribute flags */
#define GBC_ATTR_PALETTE_MASK 0x07      ///> Mask for the palette number
#define GBC_ATTR_PALETTE_START 0x01     ///> LSB of the palette mask
#define GBC_ATTR_VRAM_BANK_MASK 0X18    ///> Mask for the VRAM bank number
#define GBC_ATTR_VRAM_BANK_START 0X08   ///> LSB of the VRAM bank mask
#define GBC_ATTR_VRAM_BANK_00_FLAG 0X00 ///> Convenience flag for bank 0
#define GBC_ATTR_VRAM_BANK_01_FLAG 0x08 ///> Convenience flag for bank 1
#define GBC_ATTR_VRAM_BANK_02_FLAG 0x10 ///> Convenience flag for bank 2
#define GBC_ATTR_VRAM_BANK_03_FLAG 0X18 ///> Convenience flag for bank 3
#define GBC_ATTR_FLIP_FLAG_X 0x20       ///> Flag for horizontal flip
#define GBC_ATTR_FLIP_FLAG_Y 0x40       ///> Flag for vertical flip
#define GBC_ATTR_HIDE_FLAG 0X80         ///> Flag for hide bit

/** LCDC flags */
#define GBC_LCDC_ENABLE_FLAG 0x01          ///> Flag for LCDC enable bit
#define GBC_LCDC_BG_1_ENABLE_FLAG 0x02     ///> Flag for LCDC BG 1 enable bit
#define GBC_LCDC_BG_2_ENABLE_FLAG 0x04     ///> Flag for LCDC BG 2 enable bit
#define GBC_LCDC_BG_3_ENABLE_FLAG 0x08     ///> Flag for LCDC BG 3 enable bit
#define GBC_LCDC_BG_4_ENABLE_FLAG 0x10     ///> Flag for LCDC BG 4 enable bit
#define GBC_LCDC_SPRITE_ENABLE_FLAG 0X20   ///> Flag for LCDC sprite enable bit
#define GBC_LCDC_SPRITE_LAYER_Z_MASK 0xC0  ///> Mask for sprite layer z number
#define GBC_LCDC_SPRITE_LAYER_Z_START 0x40 ///> LSB of the sprite layer z mask
#define GBC_LCDC_SPRITE_LAYER_Z_SHIFT 6    ///> The bitshift for start of layer z mask

/** Alpha Mode flags */
#define GBC_ALPHA_MODE_BG_ENABLED_FLAG 0x01   ///> Flag for BG Alpha Mode enable bit
#define GBC_ALPHA_MODE_MASK 0x0E              ///> Mask for alpha mode
#define GBC_ALPHA_MODE_START 0x02             ///> LSB of the alpha mode
#define GBC_ALPHA_MODE_SHIFT 1                ///> The bitshift for start of alpha mode
/** Alpha Modes */
#define GBC_ALPHA_MODE_NORMAL 0   ///> Normal Mode - Alpha layer renders normally
#define GBC_ALPHA_MODE_ADD 1      ///> Alpha Mode Add - Adds alpha layer colors to current colors
#define GBC_ALPHA_MODE_SUBTRACT 2 ///> Alpha Mode Subtract - Subtracts alpha layer colors from current colors
#define GBC_ALPHA_MODE_AVERAGE 3  ///> Alpha Mode Average - Averages alpha layer colors with current colors
#define GBC_ALPHA_MODE_AND 4      ///> Alpha Mode AND - ANDs the alpha layer colors with current colors
#define GBC_ALPHA_MODE_OR 5       ///> Alpha Mode OR - ORs the alpha layer colors with current colors
#define GBC_ALPHA_MODE_XOR 6      ///> Alpha Mode XOR - XORs the alpha layer colors with current colors

/** STAT flags */
#define GBC_STAT_HBLANK_FLAG 0x01        ///> Flag for STAT HBlank flag bit
#define GBC_STAT_VBLANK_FLAG 0X02        ///> Flag for STAT VBlank flag bit
#define GBC_STAT_LINE_COMP_FLAG 0X04     ///> Flag for STAT line compare flag bit
#define GBC_STAT_OAM_FLAG 0X08           ///> Flag for STAT OAM flag bit
#define GBC_STAT_HBLANK_INT_FLAG 0X10    ///> Flag for STAT HBlank interrupt bit
#define GBC_STAT_VBLANK_INT_FLAG 0X20    ///> Flag for STAT VBlank interrupt bit
#define GBC_STAT_LINE_COMP_INT_FLAG 0X40 ///> Flag for STAT line compare interrupt bit
#define GBC_STAT_OAM_INT_FLAG 0X80       ///> Flag for STAT OAM interrupt bit
#define GBC_STAT_READ_ONLY_MASK 0x0F     ///> Mask for the read only bits of STAT
#define GBC_STAT_WRITEABLE_MASK 0xF0     ///> Mask for the writeable bits of STAT

/** OAM flags */
#define GBC_OAM_SPRITE_WIDTH_MASK 0x03     ///> Mask for OAM sprite width
#define GBC_OAM_SPRITE_WIDTH_START 0x01    ///> LSB of the OAM sprite width
#define GBC_OAM_SPRITE_WIDTH_SHIFT 0       ///> The bitshift for start of OAM sprite width
#define GBC_OAM_SPRITE_HEIGHT_MASK 0x0C    ///> Mask for OAM sprite height
#define GBC_OAM_SPRITE_HEIGHT_START 0x04   ///> LSB of the OAM sprite height
#define GBC_OAM_SPRITE_HEIGHT_SHIFT 2      ///> The bitshift for start of OAM sprite height
#define GBC_OAM_SPRITE_MOSAIC_X_MASK 0x30  ///> Mask for OAM sprite mosaic x
#define GBC_OAM_SPRITE_MOSAIC_X_START 0x10 ///> LSB of the OAM sprite mosaic x
#define GBC_OAM_SPRITE_MOSAIC_X_SHIFT 4    ///> The bitshift for start of OAM sprite mosaic x
#define GBC_OAM_SPRITE_MOSAIC_Y_MASK 0xC0  ///> Mask for OAM sprite mosaic y
#define GBC_OAM_SPRITE_MOSAIC_Y_START 0x40 ///> LSB of the OAM sprite mosaic y
#define GBC_OAM_SPRITE_MOSAIC_Y_SHIFT 6    ///> The bitshift for start of OAM sprite mosaic y

/** Helpful macros */
#define GBC_MIN(x, y) ((y) ^ (((x) ^ (y)) & -((x) < (y)))) ///> Finds the minimum of two values
#define GBC_MAX(x, y) ((x) ^ (((x) ^ (y)) & -((x) < (y)))) ///> Finds the maximum of two values
#define GBC_POINT_TO_OFFSET(x, y) ((x) & (GBC_TILEMAP_WIDTH - 1)) + ((y) & (GBC_TILEMAP_HEIGHT - 1)) * GBC_TILEMAP_WIDTH ///> Converts an x, y point on a bg map to the tile/attrmap offset
#define GBC_SUB_FLOOR(a, b) ((a) - GBC_MIN((a), (b))) ///> Subtracts b from a flooring at zero
#define GBC_ADD_CEIL(a, b, m) GBC_MIN((a) + (b), (m)) ///> Adds a to b, ceil at m
#define GBC_GET_RED(c) (((c) >> 4) & 0b11)
#define GBC_GET_GREEN(c) (((c) >> 2) & 0b11)
#define GBC_GET_BLUE(c) (((c) >> 0) & 0b11)
#define GBC_MAKE_COLOR(r, g, b) (0b11000000 | ((r) << 4) | ((g) << 2) | ((b) << 0))

/** Predefined Screen boundaries for convenience*/
#if defined(PBL_ROUND)
    #define GBC_SCREEN_BOUNDS_FULL GRect(0, 0, 180, 180)     ///> Fullscreen
    #define GBC_SCREEN_BOUNDS_LARGE GRect(2, 2, 176, 176)    ///> Largest size that maintains even number of tiles vertically and horizontally
    #define GBC_SCREEN_BOUNDS_RECT GRect(18, 6, 144, 168)    ///> Dimensions of a rectangular Pebble
    #define GBC_SCREEN_BOUNDS_SQUARE GRect(18, 18, 144, 144) ///> Square, equal number of tiles horizontal and vertical. Best fit for consistent viewing across all Pebbles.
    #define GBC_SCREEN_BOUNDS_WIDE GRect(2, 18, 176, 144)    ///> Widescreen, square stretched horizontally to max even horizontal tiles
    #define GBC_SCREEN_BOUNDS_TALL GRect(18, 2, 144, 176)    ///> Tallscreen, square stretched vertically to max even vertical tiles
    #define GBC_SCREEN_BOUNDS_SMALL GRect(26, 26, 128, 128)  ///> Smaller square
#else
    #define GBC_SCREEN_BOUNDS_FULL GRect(0, 0, 144, 168)    ///> Fullscreen
    #define GBC_SCREEN_BOUNDS_LARGE GRect(0, 4, 144, 160)   ///> Largest size that maintains even number of tiles vertically and horizontally
    #define GBC_SCREEN_BOUNDS_RECT GRect(0, 0, 144, 168)    ///> Dimensions of a rectangular Pebble
    #define GBC_SCREEN_BOUNDS_SQUARE GRect(0, 12, 144, 144) ///> Square, equal number of tiles horizontal and vertical. Best fit for consistent viewing across all Pebbles.
    #define GBC_SCREEN_BOUNDS_WIDE GRect(0, 12, 144, 144)   ///> Widescreen, square stretched horizontally to max even horizontal tiles
    #define GBC_SCREEN_BOUNDS_TALL GRect(0, 0, 144, 160)    ///> Tallscreen, square stretched vertically to max even vertical tiles
    #define GBC_SCREEN_BOUNDS_SMALL GRect(8, 16, 128, 128)  ///> Smaller square
#endif

/** Colors for black and white palettes */
#define GBC_COLOR_BLACK 0b00
#define GBC_COLOR_ALPHA_GRAY 0b01 // Won't actually show up, but is useful for alpha operations
#define GBC_COLOR_GRAY 0b10
#define GBC_COLOR_WHITE 0b11

/** The GBC Graphics "class" struct */
typedef struct _gbc_graphics GBC_Graphics;
struct _gbc_graphics {
    Layer *graphics_layer; ///< The Layer on which to render the graphics
    /**
     * The LCD Control Byte
     *  -Bit 0: Enable - Setting bit to 0 disables drawing of backgrounds and sprites
     *  -Bit 1: BG 1 Display Enable - Setting bit to 0 disables drawing of BG 1
     *  -Bit 2: BG 2 Display Enable - Setting bit to 0 disables drawing of BG 2
     *  -Bit 3: BG 3 Display Enable - Setting bit to 0 disables drawing of BG 3
     *  -Bit 4: BG 4 Display Enable - Setting bit to 0 disables drawing of BG 4
     *  -Bit 5: Sprite Display Enable - Setting bit to 0 disables drawing of sprites
     *  -Bits 6-7: Sprite Layer Z, from 0 to 3
     *      e.g. @ z = 3, sprite renders on top of BG 1, BG 2, BG 3, BG 4
     *      e.g. @ z = 0, sprite renders on top of BG 1 and below BG 2, BG 3, BG 4
     */
    uint8_t lcdc;
    /**
     * The Alpha Mode Word
     *  -Bit 0: BG 1 Display Enable - Setting bit to 1 enables alpha mode on BG 1
     *  -Bits 1-3: BG 1 Alpha Mode - Sets the alpha mode for BG 1
     *      -Mode 0: Normal Mode - Alpha layer renders normally
     *      -Mode 1: Alpha Mode Add - Adds alpha layer colors to current colors
     *      -Mode 2: Alpha Mode Subtract - Subtracts alpha layer colors from current colors
     *      -Mode 3: Alpha Mode Average - Averages alpha layer colors with current colors
     *      -Mode 4: Alpha Mode AND - ANDs the alpha layer colors with current colors
     *      -Mode 5: Alpha Mode OR - ORs the alpha layer colors with current colors
     *      -Mode 6: Alpha Mode XOR - XORs the alpha layer colors with current colors
     *  -Bit 4: BG 2 Display Enable - Setting bit to 1 enables alpha mode on BG 2
     *  -Bits 5-7: BG 2 Alpha Mode - Sets the alpha mode for BG 2
     *  -Bit 8: BG 3 Display Enable - Setting bit to 1 enables alpha mode on BG 3
     *  -Bits 9-11: BG 3 Alpha Mode - Sets the alpha mode for BG 3
     *  -Bit 12: BG 4 Display Enable - Setting bit to 1 enables alpha mode on BG 4
     *  -Bits 13-15: BG 4 Alpha Mode - Sets the alpha mode for BG 4
    */
    uint16_t a_mode;
    /**
     * VRAM Buffer - Stores the tiles for the backgrounds and sprites in a 4bpp format
     * Originally, the VRAM contained 4 banks of 8192 bytes, for a total of 32768 bytes
     * Now, the number of banks is set in the ctor
     * Each bank holds up to 256 tiles in 4bpp format of 32 bytes each
     */
    uint8_t *vram;
    /**
     * OAM Buffer - Stores the data for the current sprites
     * The OAM contains 40 slots for 5 bytes of sprite information, which is as follows:
     *  -Byte 0: Sprite x position, offset by -64 (max sprite width) to allow for off-screen rendering
     *  -Byte 1: Sprite y position, offset by -64 (max sprite height) to allow for off-screen rendering
     *  -Byte 2: Sprite tile position in VRAM bank
     *  -Byte 3: Sprite attribute data:
     *      -Bits 0-2: Palette number, from 0 to 7
     *      -Bits 3-4: VRAM Bank Select, from 0 to 3
     *      -Bit 5: X Flip - Setting bit will flip the sprite horizontally when rendered
     *      -Bit 6: Y Flip - Setting bit will flip the sprite vertically when rendered
     *      -Bit 7: Hide - Setting the bit will make the sprite hidden
     *  - Byte 4: Sprite size data
     *      -Bits 0-1: Sprite height, from 0-3
     *          Options are: 0 = 8px, 1 = 16px, 2 = 32px, 3 = 64px
     *      -Bits 2-3: Sprite width, from 0-3
     *          Options are: 0 = 8px, 1 = 16px, 2 = 32px, 3 = 64px
     *      -Bits 4-5: Sprite mosaic x, from 0-3
     *      -Bits 6-7: Sprite mosaic y, from 0-3
     */
    uint8_t *oam;
    /**
     * Background Tilemap buffers
     * This tilemap contains 1-4 32 x 32 VRAM bank locations of 1 byte each, totaling 512 * num_backgrounds bytes
     * The offset of the screen view is set by bg_scroll_x and bg_scroll_y
     */
    uint8_t *bg_tilemaps;
    /**
     * Background Atributemap Buffers
     * This tilemap contains 4 of 32 x 32 tile attributes of 1 byte each, totaling 4096 bytes
     * Each attribute corresponds to a tile on the bg_tilemap
     * The attribute byte is defined as:
     *      -Bits 0-2: Palette number, from 0 to 7
     *      -Bits 3-4: VRAM Bank Select, from 0 to 3
     *      -Bit 5: X Flip - Setting bit will flip the tile horizontally when rendered
     *      -Bit 6: Y Flip - Setting bit will flip the tile vertically when rendered
     *      -Bit 7: Hide - Setting the bit will make the tile hidden
     */
    uint8_t *bg_attrmaps;
    /**
     * Background Palette Bank
     * The palette bank contains 8 palettes of 16 colors (1 byte) each, totalling 128 bytes
     */
    uint8_t *bg_palette_bank;
    /**
     * Sprite Palette Bank
     * The palette bank contains 8 palettes of 16 colors (1 byte) each, totalling 128 bytes
     * The 0th color in each palette will be replaced by transparency when rendered.
     */
    uint8_t *sprite_palette_bank;
    short *bg_scroll_x; ///> Array of 4 x positions of the screen view into the background tilemaps
    short *bg_scroll_y; ///> Array of 4 y position of the screen view into the background tilemaps
    /**
     * LCD Status Byte
     *  -Bit 0: HBlank Flag - Set to 1 between rendering lines - READ ONLY
     *  -Bit 1: VBlank Flag - Set to 1 between rendering frames (after all lines rendered) - READ ONLY
     *  -Bit 2: Line Compare Flag - Set to 1 when line_y_compare == line_y - READ ONLY
     *  -Bit 3: OAM Flag - Set to 1 after all sprites are rendered - READ ONLY
     *  -Bit 4: HBlank Interrupt Enabled
     *  -Bit 5: VBlank Interrupt Enabled
     *  -Bit 6: Line Compare Interrupt Enabled
     *  -Bit 7: OAM Interrupt Enabled
     */
    uint8_t stat;
    uint8_t line_y; ///> The current line being rendered - READ ONLY
    uint8_t line_y_compare; ///> The line number to compare line_y against
    void (*hblank_interrupt_callback)(GBC_Graphics *); ///> The callback for the HBlank interrupt
    void (*vblank_interrupt_callback)(GBC_Graphics *); ///> The callback for the VBlank interrupt
    void (*line_compare_interrupt_callback)(GBC_Graphics *); ///> The callback for the line compare interrupt
    void (*oam_interrupt_callback)(GBC_Graphics *); ///> The callback for the oam interrupt

    uint8_t num_backgrounds; ///> The number of background layers
    uint8_t screen_x_origin; ///> The start x position of the rendered screen
    uint8_t screen_y_origin; ///> The start y position of the rendered screen
    uint8_t screen_width; ///> The width of the rendered screen
    uint8_t screen_height; ///> The height of the rendered screen
};

/**
 * Creates a GBC Graphics object
 * 
 * @param window The window in which to display the GBC Graphics object
 * @param num_vram_banks The number of vram banks to generate, can be from 1-4
 * @param num_backgrounds The number of background layers to generate, can be from 1-4
 * 
 * @return a pointer to the created GBC Graphics object
 * @note I recommend creating the GBC Graphics object in window_load or an equivalent function
 * @note Aplite is severely limited in RAM, so I recommend using only one vram bank for Aplite and
 * loading from static resources on the fly. If designing for Basalt/Chalk/Diorite, then feel free
 * to use as many vram banks as you'd like
 */
GBC_Graphics *GBC_Graphics_ctor(Window *window, uint8_t num_vram_banks, uint8_t num_backgrounds);

/**
 * Destroys the GBC Graphics display object by freeing any memory it uses
 * 
 * @param self A pointer to the target GBC Graphics object
 */
void GBC_Graphics_destroy(GBC_Graphics *self);

/**
 * Sets the boundaries of the screen to render
 * @note Decreasing the size of the screen will greatly increase FPS
 *
 * @param self A pointer to the target GBC Graphics object
 * @param bounds The boundaries of the screen, GRect(screen x origin, screen y origin, screen width, screen height)
 */
void GBC_Graphics_set_screen_bounds(GBC_Graphics *self, GRect bounds);

/**
 * Sets the x origin of the screen
 *
 * @param self A pointer to the target GBC Graphics object
 * @param new_x The new x origin of the screen
 */
void GBC_Graphics_set_screen_x_origin(GBC_Graphics *self, uint8_t new_x);

/**
 * Sets the y origin of the screen
 *
 * @param self A pointer to the target GBC Graphics object
 * @param new_y The new y origin of the screen
 */
void GBC_Graphics_set_screen_y_origin(GBC_Graphics *self, uint8_t new_y);

/**
 * Sets the width of the screen
 * @note Reducing this parameter will increase FPS
 *
 * @param self A pointer to the target GBC Graphics object
 * @param new_width The new width of the screen
 */
void GBC_Graphics_set_screen_width(GBC_Graphics *self, uint8_t new_width);

/**
 * Sets the height of the screen
 * @note Reducing this parameter will increase FPS
 *
 * @param self A pointer to the target GBC Graphics object
 * @param new_height The new height of the screen
 */
void GBC_Graphics_set_screen_height(GBC_Graphics *self, uint8_t new_height);

/**
 * Gets the boundaries that the screen is rendering within
 *
 * @param self A pointer to the target GBC Graphics object
 *
 * @return The screen's boundaries
 */
GRect GBC_Graphics_get_screen_bounds(GBC_Graphics *self);

/**
 * Gets the x origin of the screen
 *
 * @param self A pointer to the target GBC Graphics object
 *
 * @return The screen's x origin
 */
uint8_t GBC_Graphics_get_screen_x_origin(GBC_Graphics *self);

/**
 * Gets the y origin of the screen
 *
 * @param self A pointer to the target GBC Graphics object
 *
 * @return The screen's y origin
 */
uint8_t GBC_Graphics_get_screen_y_origin(GBC_Graphics *self);

/**
 * Gets the width of the screen
 *
 * @param self A pointer to the target GBC Graphics object
 *
 * @return The screen's width
 */
uint8_t GBC_Graphics_get_screen_width(GBC_Graphics *self);

/**
 * Gets the height of the screen
 *
 * @param self A pointer to the target GBC Graphics object
 *
 * @return The screen's height
 */
uint8_t GBC_Graphics_get_screen_height(GBC_Graphics *self);

/**
 * Moves tiles from one vram bank and position to another
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param src_vram_bank The VRAM bank to draw from
 * @param src_tile_offset The tile offset to swap from
 * @param dest_vram_bank The VRAM bank to edit
 * @param dest_tile_offset The tile offset to swap into
 * @param num_tiles_to_move The number of tiles to move
 * @param swap If the src and dest tiles should be swapped, otherwise just copies from dest
 */
void GBC_Graphics_vram_move_tiles(GBC_Graphics *self, uint8_t src_vram_bank, uint8_t src_tile_offset, 
                             uint8_t dest_vram_bank, uint8_t dest_tile_offset, uint8_t num_tiles_to_move, bool swap);

/**
 * Loads tiles from a tilesheet in storage into vram
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param tilesheet_resource The resource ID of the tilesheet to load from
 * @param tilesheet_tile_offset The tile number offset on the tilesheet to load from
 * @param tiles_to_load The number of tiles to load from the tilesheet
 * @param vram_tile_offset The tile number offset on the VRAM to loading into
 * @param vram_bank_number The VRAM bank to load into
 */
void GBC_Graphics_load_from_tilesheet_into_vram(GBC_Graphics *self, uint32_t tilesheet_resource, uint16_t tilesheet_tile_offset, 
                                            uint16_t tiles_to_load, uint16_t vram_tile_offset, uint8_t vram_bank_number);

/**
 * Returns a pointer to the requested VRAM bank
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param vram_bank_number The VRAM bank to return
 * @return A VRAM bank
 */
uint8_t *GBC_Graphics_get_vram_bank(GBC_Graphics *self, uint8_t vram_bank_number);

/**
 * Sets an arbitrary number of colors of one of the background palettes
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param palette_num The number of the palette to set, from 0 to 7
 * @param num_colors The number of colors being set, from 1 to 16
 * @param ... A va_list containing all of the colors
 */
void GBC_Graphics_set_bg_palette(GBC_Graphics *self, uint8_t palette_num, uint8_t num_colors, ...);

/**
 * Sets the sixteen colors of one of the background palettes from an array
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param palette_num The number of the palette to set, from 0 to 7
 * @param palette_array A pointer to a 16-byte array containing the palette colors
 */
void GBC_Graphics_set_bg_palette_array(GBC_Graphics *self, uint8_t palette_num, uint8_t *palette_array);

/**
 * Sets one color of one of the background palettes
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param palette_num The number of the palette to set, from 0 to 7
 * @param color_num The number of the palette color to set, from 0 to 15
 * @param c The new color
 */
void GBC_Graphics_set_bg_palette_color(GBC_Graphics *self, uint8_t palette_num, uint8_t color_num, uint8_t c);

/**
 * Copies the specified bg palette into a target array
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param palette_num The number of the palette to copy
 * @param target_array The array to copy into
 */
void GBC_Graphics_copy_one_bg_palette(GBC_Graphics *self, uint8_t palette_num, uint8_t *target_array);

/**
 * Copies the bg palettes into a target array
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param target_array The array to copy into
 */
void GBC_Graphics_copy_all_bg_palettes(GBC_Graphics *self, uint8_t *target_array);

/**
 * Sets an arbitrary number of colors of one of the sprite palettes
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param palette_num The number of the palette to set, from 0 to 7
 * @param num_colors The number of colors being set, from 1 to 16
 * @param ... A va_list containing all of the colors
 */
void GBC_Graphics_set_sprite_palette(GBC_Graphics *self, uint8_t palette_num, uint8_t num_colors, ...);

/**
 * Sets the sixteen colors of one of the sprite palettes from an array
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param palette_num The number of the palette to set, from 0 to 7
 * @param palette_array A pointer to a 16-byte array containing the palette colors
 */
void GBC_Graphics_set_sprite_palette_array(GBC_Graphics *self, uint8_t palette_num, uint8_t *palette_array);

/**
 * Sets one color of one of the sprite palettes
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param palette_num The number of the palette to set, from 0 to 7
 * @param color_num The number of the palette color to set, from 0 to 3
 * @param c The new color
 */
void GBC_Graphics_set_sprite_palette_color(GBC_Graphics *self, uint8_t palette_num, uint8_t color_num, uint8_t c);

/**
 * Copies the specified sprite palette into a target array
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param palette_num The number of the palette to copy
 * @param target_array The array to copy into
 */
void GBC_Graphics_copy_one_sprite_palette(GBC_Graphics *self, uint8_t palette_num, uint8_t *target_array);

/**
 * Copies the sprite palettes into a target array
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param target_array The array to copy into
 */
void GBC_Graphics_copy_all_sprite_palettes(GBC_Graphics *self, uint8_t *target_array);

/**
 * Renders the background, window, and sprite layers at the next available opportunity
 * 
 * @param self A pointer to the target GBC Graphics object
 */
void GBC_Graphics_render(GBC_Graphics *self);

/**
 * Sets the LCDC byte
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param new_lcdc The new LCDC byte
 */
void GBC_Graphics_lcdc_set(GBC_Graphics *self, uint8_t new_lcdc);

/**
 * Sets the enable bit of the LCDC byte
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param enabled Should the bit be enabled?
 */
void GBC_Graphics_lcdc_set_enabled(GBC_Graphics *self, bool enabled);

/**
 * Sets the background layer enabled bit of the LCDC byte
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_num The background to set, 0-3
 * @param enabled Should the bit be enabled?
 */
void GBC_Graphics_lcdc_set_bg_layer_enabled(GBC_Graphics *self, uint8_t bg_num, bool enabled);

/**
 * Sets the sprite layer enabled bit of the LCDC byte
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param enabled Should the bit be enabled?
 */
void GBC_Graphics_lcdc_set_sprite_layer_enabled(GBC_Graphics *self, bool enabled);

/**
 * Sets the sprite layer z bits of the LCDC byte
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param layer_z The z position of the sprite layer
 * 
*/
void GBC_Graphics_lcdc_set_sprite_layer_z(GBC_Graphics *self, uint8_t layer_z);

/**
 * Sets the alpha mode enabled bit for a background of the alpha mode word
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_num The background to set, 0-3
 * @param enabled Should the bit be enabled?
 */
void GBC_Graphics_alpha_mode_set_bg_enabled(GBC_Graphics *self, uint8_t bg_num, bool enabled);

/**
 * Sets the alpha mode for a background of the alpha mode word
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_num The background to set, 0-3
 * @param enabled The alpha mode, 0-7
 */
void GBC_Graphics_alpha_mode_set_mode(GBC_Graphics *self, uint8_t bg_num, uint8_t mode);

/**
 * Gets the current line being rendered
 * 
 * @param self A pointer to the target GBC Graphics object
 * 
 * @return The current line being rendered
 */
uint8_t GBC_Graphics_stat_get_current_line(GBC_Graphics *self);

/**
 * Gets the current line to be compared for the interrupt
 * 
 * @param self A pointer to the target GBC Graphics object
 * 
 * @return The current line comparision value
 */
uint8_t GBC_Graphics_stat_get_line_y_compare(GBC_Graphics *self);

/**
 * Checks if the HBlank flag is set
 * 
 * @param self A pointer to the target GBC Graphics object
 * 
 * @return Is the HBlank flag set?
 */
bool GBC_Graphics_stat_check_hblank_flag(GBC_Graphics *self);

/**
 * Checks if the VBlank flag is set
 * 
 * @param self A pointer to the target GBC Graphics object
 * 
 * @return Is the VBlank flag set?
 */
bool GBC_Graphics_stat_check_vblank_flag(GBC_Graphics *self);

/**
 * Checks if the Line Compare flag is set
 * 
 * @param self A pointer to the target GBC Graphics object
 * 
 * @return Is the Line Compare flag set?
 */
bool GBC_Graphics_stat_check_line_comp_flag(GBC_Graphics *self);

/**
 * Checks if the OAM flag is set
 * 
 * @param self A pointer to the target GBC Graphics object
 * 
 * @return Is the OAM flag set?
 */
bool GBC_Graphics_stat_check_oam_flag(GBC_Graphics *self);

/**
 * Sets the STAT byte
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param new_stat The new STAT byte, note that this function will not modify the read only bits
 */
void GBC_Graphics_stat_set(GBC_Graphics *self, uint8_t new_stat);

/**
 * Sets the HBlank interrupt enable bit of STAT
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param enabled Should the HBlank interrupt be enabled?
 */
void GBC_Graphics_stat_set_hblank_interrupt_enabled(GBC_Graphics *self, bool enabled);

/**
 * Sets the VBlank interrupt enable bit of STAT
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param enabled Should the VBlank interrupt be enabled?
 */
void GBC_Graphics_stat_set_vblank_interrupt_enabled(GBC_Graphics *self, bool enabled);

/**
 * Sets the line compare interrupt enable bit of STAT
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param enabled Should the line compare interrupt be enabled?
 */
void GBC_Graphics_stat_set_line_compare_interrupt_enabled(GBC_Graphics *self, bool enabled);

/**
 * Sets the OAM interrupt enable bit of STAT
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param enabled Should the OAM interrupt be enabled?
 */
void GBC_Graphics_stat_set_oam_interrupt_enabled(GBC_Graphics *self, bool enabled);

/**
 * Sets the line to compare for the line compare interrupt
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param new_line_y_comp The new line to compare
 */
void GBC_Graphics_stat_set_line_y_compare(GBC_Graphics *self, uint8_t new_line_y_comp);

/**
 * Sets the callback function for the HBlank interrupt
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param callback The function to call when the HBlank interrupt fires
 */
void GBC_Graphics_set_hblank_interrupt_callback(GBC_Graphics *self, void (*callback)());

/**
 * Sets the callback function for the VBlank interrupt
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param callback The function to call when the VBlank interrupt fires
 */
void GBC_Graphics_set_vblank_interrupt_callback(GBC_Graphics *self, void (*callback)());

/**
 * Sets the callback function for the line compare interrupt
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param callback The function to call when the line compare interrupt fires
 */
void GBC_Graphics_set_line_compare_interrupt_callback(GBC_Graphics *self, void (*callback)());

/**
 * Sets the callback function for the OAM interrupt
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param callback The function to call when the OAM interrupt fires
 */
void GBC_Graphics_set_oam_interrupt_callback(GBC_Graphics *self, void (*callback)());

/** 
 * Helper function for creating an attribute byte
 * 
 * @param palette The palette to use
 * @param vram_bank The VRAM bank to use
 * @param is_x_flipped Should the tile/sprite be flipped horizontally?
 * @param is_y_flipped Should the tile/sprite be flipped vertically?
 * @param is_hidden Should the tile/sprite be hidden?
 * 
 * @return An attribute byte containing the given parameters
 */
uint8_t GBC_Graphics_attr_make(uint8_t palette, uint8_t vram_bank, bool is_x_flipped, bool is_y_flipped, bool is_hidden);

/**
 * Extracts the palette number from an attribute byte
 * 
 * @param attributes An attribute byte
 * 
 * @return The corresponding palette number
 */
uint8_t GBC_Graphics_attr_get_palette_num(uint8_t attributes);

/**
 * Extracts the VRAM bank from an attribute byte
 * 
 * @param attributes An attribute byte
 * 
 * @return The corresponding palette number
 */
uint8_t GBC_Graphics_attr_get_vram_bank(uint8_t attributes);

/**
 * Checks if the attribute byte has a horizontal flip flag
 * 
 * @param attributes An attribute byte
 * 
 * @return true if the attribute has a horizontal flip flag
 */
bool GBC_Graphics_attr_is_x_flipped(uint8_t attributes);

/**
 * Checks if the attribute byte has a vertical flip flag
 * 
 * @param attributes An attribute byte
 * 
 * @return true if the attribute has a vertical flip flag
 */
bool GBC_Graphics_attr_is_y_flipped(uint8_t attributes);

/**
 * Checks if the attribute byte has a hide flag
 * 
 * @param attributes An attribute byte
 * 
 * @return true if the attribute has a hide flag
 */
bool GBC_Graphics_attr_is_hidden(uint8_t attributes);


/**
 * Gets the current x position of the background scroll
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * 
 * @return The current x position of the background scroll
 */
uint8_t GBC_Graphics_bg_get_scroll_x(GBC_Graphics *self, uint8_t bg_layer);

/**
 * Gets the current y position of the background scroll
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * 
 * @return The current y position of the background scroll
 */
uint8_t GBC_Graphics_bg_get_scroll_y(GBC_Graphics *self, uint8_t bg_layer);

/**
 * Gets the tile number at the given position on the background
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The x position on the background, from 0 to 31
 * @param y The y position on the background, from 0 to 31
 * 
 * @return The tile number
 */
uint8_t GBC_Graphics_bg_get_tile(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y);

/** 
 * Gets the attributes of the tile at the given position on the background
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The x position on the background, from 0 to 31
 * @param y The y position on the background, from 0 to 31
 * 
 * @return The tile attributes
 */
uint8_t GBC_Graphics_bg_get_attr(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y);

/**
 * Moves the background scroll by dx and dy
 * X and Y will wrap around if they become < 0 or > MAP_SIZE * GBC_TILE_NUM_BYTES
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param dx The delta x to move the scroll x by
 * @param dy The delta y to move the scroll y by
 */
void GBC_Graphics_bg_move(GBC_Graphics *self, uint8_t bg_layer, short dx, short dy);

/**
 * Sets the background scroll x
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The new scroll x
 */
void GBC_Graphics_bg_set_scroll_x(GBC_Graphics *self, uint8_t bg_layer, uint8_t x);

/**
 * Sets the background scroll y
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param y The new scroll y
 */
void GBC_Graphics_bg_set_scroll_y(GBC_Graphics *self, uint8_t bg_layer, uint8_t y);

/**
 * Sets the background scroll x and scroll y
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The new scroll x
 * @param y The new scroll y
 */
void GBC_Graphics_bg_set_scroll_pos(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y);

/**
 * Sets the tile number at the given position on the background
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The x position on the background, from 0 to 31
 * @param y The y position on the background, from 0 to 31
 * @param tile_number The new tile number
 */
void GBC_Graphics_bg_set_tile(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, uint8_t tile_number);

/**
 * Sets the tile attributes at the given position on the background
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The x position on the background, from 0 to 31
 * @param y The y position on the background, from 0 to 31
 * @param attributes The new attributes
 */
void GBC_Graphics_bg_set_attrs(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, uint8_t attributes);

/**
 * Sets both the tile number and tile attributes at the given position on the background
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The x position on the background, from 0 to 31
 * @param y The y position on the background, from 0 to 31
 * @param tile_number The new tile number
 * @param attributes The new tile number
 */
void GBC_Graphics_bg_set_tile_and_attrs(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, uint8_t tile_number, uint8_t attributes);

/**
 * Sets the palette of the tile at the given position on the background
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The x position on the background, from 0 to 31
 * @param y The y position on the background, from 0 to 31
 * @param palette The new palette, from 0 to 7
 */
void GBC_Graphics_bg_set_tile_palette(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, uint8_t palette);

/**
 * Sets the VRAM bank of the tile at the given position on the background
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The x position on the background, from 0 to 31
 * @param y The y position on the background, from 0 to 31
 * @param vram_bank The new VRAM bank, from 0 to 3
 */
void GBC_Graphics_bg_set_tile_vram_bank(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, uint8_t vram_bank);

/**
 * Sets if the tile at the given position on the background should be flipped horizontally
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The x position on the background, from 0 to 31
 * @param y The y position on the background, from 0 to 31
 * @param flipped If the tile should be flipped horizontally
 */
void GBC_Graphics_bg_set_tile_x_flip(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, bool flipped);

/**
 * Sets if the tile at the given position on the background should be flipped vertically
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The x position on the background, from 0 to 31
 * @param y The y position on the background, from 0 to 31
 * @param flipped If the tile should be flipped vertically
 */
void GBC_Graphics_bg_set_tile_y_flip(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, bool flipped);

/**
 * Sets if the tile at the given position on the background should be hidden
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param x The x position on the background, from 0 to 31
 * @param y The y position on the background, from 0 to 31
 * @param hidden If the tile should be hidden
 */
void GBC_Graphics_bg_set_tile_hidden(GBC_Graphics *self, uint8_t bg_layer, uint8_t x, uint8_t y, bool hidden);

/**
 * Moves a tile from one location to another
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param bg_layer The number of the background layer, from 0 to 3
 * @param src_x The x position of the source tile1
 * @param src_y The y position of the source tile1
 * @param dest_x The x position of the destination tile1
 * @param dest_y The y position of the destination tile1
 * @param swap If the src and dest tiles should be swapped, otherwise just copies from dest
 */
void GBC_Graphics_bg_move_tile(GBC_Graphics *self, uint8_t bg_layer, uint8_t src_x, uint8_t src_y, uint8_t dest_x, uint8_t dest_y, bool swap);

/**
 * Gets the x position of the sprite
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * 
 * @return The sprite's x position
 */
uint8_t GBC_Graphics_oam_get_sprite_x(GBC_Graphics *self, uint8_t sprite_num);

/**
 * Gets the y position of the sprite
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * 
 * @return The sprite's y position
 */
uint8_t GBC_Graphics_oam_get_sprite_y(GBC_Graphics *self, uint8_t sprite_num);

/**
 * Gets the tile of the sprite
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * 
 * @return The sprite's tile
 */
uint8_t GBC_Graphics_oam_get_sprite_tile(GBC_Graphics *self, uint8_t sprite_num);

/**
 * Gets the attributes of the sprite
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * 
 * @return The sprite's attributes
 */
uint8_t GBC_Graphics_oam_get_sprite_attrs(GBC_Graphics *self, uint8_t sprite_num);

/**
 * Gets the width of the sprite in pixels
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * 
 * @return The sprite's width value
 */
uint8_t GBC_Graphics_oam_get_sprite_width(GBC_Graphics *self, uint8_t sprite_num);

/**
 * Gets the height of the sprite in pixels
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * 
 * @return The sprite's height value
 */
uint8_t GBC_Graphics_oam_get_sprite_height(GBC_Graphics *self, uint8_t sprite_num);

/**
 * Gets the mosaic x of the sprite in pixels
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * 
 * @return The sprite's mosaic x in pixels
 */
uint8_t GBC_Graphics_oam_get_sprite_mosaic_x(GBC_Graphics *self, uint8_t sprite_num);

/**
 * Gets the mosaic y of the sprite in pixels
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * 
 * @return The sprite's mosaic y in pixels
 */
uint8_t GBC_Graphics_oam_get_sprite_mosaic_y(GBC_Graphics *self, uint8_t sprite_num);

/**
 * Creates and sets a sprite in OAM with the given values
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param x The sprite's x position
 * @param y The sprite's y position
 * @param tile_position The tile position in VRAM that the sprite will use to render
 * @param attributes The sprite's attributes
 * @param width The sprite's width (see oam description), 0-3
 * @param height The sprite's height (see oam description), 0-3
 * @param mosaic_x The sprite's mosaic x
 * @param mosaix_y The sprite's mosaic y
 */
void GBC_Graphics_oam_set_sprite(GBC_Graphics *self, uint8_t sprite_num, uint8_t x, uint8_t y, uint8_t tile_position, uint8_t attributes, uint8_t width, uint8_t height, uint8_t mosaic_x, uint8_t mosaic_y);

/**
 * Moves a sprite on the OAM by dx and dy
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param dx The delta x to move the sprite by
 * @param dy The delta y to move the sprite by
 */
void GBC_Graphics_oam_move_sprite(GBC_Graphics *self, uint8_t sprite_num, short dx, short dy);

/**
 * Sets the sprite's x position
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param x The x position to move the sprite to
 */
void GBC_Graphics_oam_set_sprite_x(GBC_Graphics *self, uint8_t sprite_num, uint8_t x);

/**
 * Sets the sprite's y position
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param y The y position to move the sprite to
 */
void GBC_Graphics_oam_set_sprite_y(GBC_Graphics *self, uint8_t sprite_num, uint8_t y);

/**
 * Sets the sprite's x and y position
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param x The x position to move the sprite to
 * @param y The y position to move the sprite to
 */
void GBC_Graphics_oam_set_sprite_pos(GBC_Graphics *self, uint8_t sprite_num, uint8_t x, uint8_t y);

/**
 * Sets the tile position in VRAM that the sprite will use to render
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param tile_position The new tile position
 */
void GBC_Graphics_oam_set_sprite_tile(GBC_Graphics *self, uint8_t sprite_num, uint8_t tile_position);

/**
 * Sets the tile position in VRAM that the sprite will use to render
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param tile_position The new tile position
 */
void GBC_Graphics_oam_set_sprite_attrs(GBC_Graphics *self, uint8_t sprite_num, uint8_t attributes);

/**
 * Sets the palette to be used by the sprite
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param palette The number of the palette to use, 0-7
 */
void GBC_Graphics_oam_set_sprite_palette(GBC_Graphics *self, uint8_t sprite_num, uint8_t palette);

/**
 * Sets the VRAM bank to be used by the sprite
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param vram_bank The VRAM bank to use, 0-3
 */
void GBC_Graphics_oam_set_sprite_vram_bank(GBC_Graphics *self, uint8_t sprite_num, uint8_t vram_bank);

/**
 * Sets the sprite's horizontal flip bit
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param flipped Should the sprite be flipped horizontally?
 */
void GBC_Graphics_oam_set_sprite_x_flip(GBC_Graphics *self, uint8_t sprite_num, bool flipped);

/**
 * Sets the sprite's vertical flip bit
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param flipped Should the sprite be flipped vertical?
 */
void GBC_Graphics_oam_set_sprite_y_flip(GBC_Graphics *self, uint8_t sprite_num, bool flipped);
/**
 * Sets the sprite's hide bit
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param flipped Should the sprite be hidden?
 */
void GBC_Graphics_oam_set_sprite_hidden(GBC_Graphics *self, uint8_t sprite_num, bool hidden);

/**
 * Sets the sprite's width (see OAM description)
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param width The width of the sprite (see OAM description)
 */
void GBC_Graphics_oam_set_sprite_width(GBC_Graphics *self, uint8_t sprite_num, uint8_t width);

/**
 * Sets the sprite's height (see OAM description)
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param height The height of the sprite (see OAM description)
 */
void GBC_Graphics_oam_set_sprite_height(GBC_Graphics *self, uint8_t sprite_num, uint8_t height);

/**
 * Sets the sprite's mosaic x (see OAM description)
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param mosaic_x The mosaic x of the sprite (see OAM description)
 */
void GBC_Graphics_oam_set_sprite_mosaic_x(GBC_Graphics *self, uint8_t sprite_num, uint8_t mosaic_x);

/**
 * Sets the sprite's mosaic y (see OAM description)
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num The sprite's position in OAM
 * @param mosaic_y The mosaic y of the sprite (see OAM description)
 */
void GBC_Graphics_oam_set_sprite_mosaic_y(GBC_Graphics *self, uint8_t sprite_num, uint8_t mosaic_y);

/**
 * Moves a sprite from one position in OAM to another
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param source_sprite_num The OAM location to move from
 * @param target_sprite_num The OAM location to move to
 * @param copy Should the sprite be copied (preserve source) or moved (delete source)?
 */
void GBC_Graphics_oam_change_sprite_num(GBC_Graphics *self, uint8_t source_sprite_num, uint8_t target_sprite_num, bool copy);

/**
 * Swaps two sprites in OAM
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num_1 The first sprite to swap
 * @param sprite_num_2 The second sprite to swap
 */
void GBC_Graphics_oam_swap_sprites(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2);

/**
 * Swaps the tiles of two sprites
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num_1 The first sprite to swap
 * @param sprite_num_2 The second sprite to swap
 */
void GBC_Graphics_oam_swap_sprite_tiles(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2);

/**
 * Swaps the attributes of two sprites
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num_1 The first sprite to swap
 * @param sprite_num_2 The second sprite to swap
 */
void GBC_Graphics_oam_swap_sprite_attrs(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2);

/**
 * Swaps the tiles and attributes of two sprites
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param sprite_num_1 The first sprite to swap
 * @param sprite_num_2 The second sprite to swap
 */
void GBC_Graphics_oam_swap_sprite_tiles_and_attrs(GBC_Graphics *self, uint8_t sprite_num_1, uint8_t sprite_num_2);

/**
 * Copies the data in one background tilemap and attrmap to 
 * the target tilemap and attrmap
 * 
 * @param self A pointer to the target GBC Graphics object
 * @param source_bg_layer The number of the source background layer, from 0 to 3
 * @param target_bg_layer The number of the source background layer, from 0 to 3
*/
void GBC_Graphics_copy_background(GBC_Graphics *self, uint8_t source_bg_layer, uint8_t target_bg_layer);