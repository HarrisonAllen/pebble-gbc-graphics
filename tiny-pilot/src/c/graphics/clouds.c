#include "clouds.h"

/**
 * Data for each cloud, stored as:
 *  [0] Tilesheet offset
 *  [1] Width in tiles
 *  [2] Height in tiles
 */
static const uint8_t cloud_data[] = {
    0, 4, 2,
    8, 4, 2,
    16, 7, 2,
    30, 5, 2,
    40, 4, 2,
    48, 4, 1,
    52, 3, 1,
    55, 4, 1,
    59, 5, 1,
    64, 5, 1,
    69, 6, 1,
    75, 3, 1,
    78, 2, 1,
    80, 4, 1,
    84, 2, 1,
    86, 2, 1,
    88, 2, 1,
    // There are more clouds on the tilesheet, but they are too small and I don't want to use them
    // 90, 1, 1,
    // 91, 1, 1,
    // 92, 1, 1,
    // 93, 1, 1,
};

// The dimensions of a cloud box, which is the region in which a cloud is generated
static uint8_t s_cloud_box_dims[] = {8, 8};

/**
 * The cloud palette is stored in array rather than being explicitly set in code
 * 
 * The colors are:
 *   1. Sky color
 *   2. Sky color (for tiles that need to have background priority)
 *   3. Cloud detail color
 *   4. Cloud fill color
 */
static uint8_t cloud_palette[] = {
#if defined(PBL_COLOR)
    GColorPictonBlueARGB8, GColorPictonBlueARGB8, GColorCelesteARGB8, GColorWhiteARGB8
#else
    GBC_COLOR_WHITE, GBC_COLOR_WHITE, GBC_COLOR_BLACK, GBC_COLOR_BLACK
#endif
};

/**
 * Gets the number of available clouds to choose from
 * 
 * @return The number of clouds
 */
static uint8_t get_num_clouds() {
    return sizeof(cloud_data) / 3;
}

/**
 * Loads in the cloud palette from an array
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param palette_num The palette number to load into
 */
static void load_cloud_palette(GBC_Graphics *graphics, uint8_t palette_num) {
    GBC_Graphics_set_bg_palette_array(graphics, palette_num, cloud_palette);
}

/**
 * Draws a cloud at the given location
 * 
 * @param graphics The GBC_Graphics object for rendering
 * @param cloud_id The id of the cloud to draw
 * @param x_pos The x position of the cloud in tiles
 * @param y_pos The y position of the cloud in tiles
 * @param cloud_vram_start The vram offset of the cloud tilesheet
 * @param cloud_attrs The attributes for the cloud to use
 */
static void draw_cloud(GBC_Graphics *graphics, uint8_t cloud_id, uint8_t x_pos, uint8_t y_pos, uint8_t cloud_vram_start, uint8_t cloud_attrs) {
    const uint8_t *data = &cloud_data[cloud_id * 3];
    uint8_t vram_offset = data[0];
    uint8_t cloud_width = data[1];
    uint8_t cloud_height = data[2];

    uint8_t cloud_index = 0;
    for (uint8_t y = 0; y < cloud_height; y++) {
        for (uint8_t x = 0; x < cloud_width; x++) {
            GBC_Graphics_bg_set_tile_and_attrs(graphics, (x_pos + x) % GBC_TILEMAP_WIDTH, (y_pos + y) % GBC_TILEMAP_HEIGHT, vram_offset + cloud_vram_start + cloud_index, cloud_attrs);
            cloud_index++;
        }
    }
}

void draw_clouds(GBC_Graphics *graphics, uint8_t cloud_vram_start) {
    // Load the palette from an array
    load_cloud_palette(graphics, CLOUD_PALETTE);

    // Fill the background with the sky
    uint8_t sky_attrs = GBC_Graphics_attr_make(0, 0, false, false, false);
    for (uint8_t x = 0; x < GBC_TILEMAP_WIDTH; x++) {
        for (uint8_t y = 0; y < GBC_TILEMAP_HEIGHT; y++) {
            GBC_Graphics_bg_set_tile_and_attrs(graphics, x, y, CLOUD_SKY_TILE, sky_attrs);
        }
    }

    // Now draw the clouds, doing the cloud boxes spreads them out and prevents consistency
    uint8_t num_clouds_x = GBC_TILEMAP_WIDTH / s_cloud_box_dims[0];
    uint8_t num_clouds_y = GBC_TILEMAP_HEIGHT / s_cloud_box_dims[1];
    for (uint8_t y = 0; y < num_clouds_y; y++) {
        for (uint8_t x = 0; x < num_clouds_x; x++) {
            // Pull a random cloud
            uint8_t cloud_id = rand() % get_num_clouds();
            const uint8_t *data = &cloud_data[cloud_id * 3];

            // Place the cloud randomly within the cloud box
            uint8_t cloud_width = data[1];
            uint8_t cloud_height = data[2];
            uint8_t empty_x_tiles = s_cloud_box_dims[0] - cloud_width;
            uint8_t empty_y_tiles = s_cloud_box_dims[1] - cloud_height;
            uint8_t x_offset = rand()%empty_x_tiles;
            uint8_t y_offset = rand()%empty_y_tiles;

            // Make some of the clouds foreground objects for an extra layer of depth
            bool should_cloud_be_foreground = rand()%2 == 0;
            uint8_t cloud_attrs = GBC_Graphics_attr_make(0, 0, false, false, should_cloud_be_foreground);

            // And now draw the cloud
            uint8_t x_pos = (x) * s_cloud_box_dims[0] + x_offset;
            uint8_t y_pos = (y) * s_cloud_box_dims[1] + y_offset;
            draw_cloud(graphics, cloud_id, x_pos, y_pos, cloud_vram_start, cloud_attrs);
        }
    }
}