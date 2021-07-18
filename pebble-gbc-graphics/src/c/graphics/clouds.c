#include "clouds.h"

// 94 tiles!
static const uint8_t cloud_data[] = {
// offset, width, height
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
    // 90, 1, 1,
    // 91, 1, 1,
    // 92, 1, 1,
    // 93, 1, 1,
};

static uint8_t s_cloud_box_dims[] = {8, 8};

// Palettes can be stored in arrays, and loaded!
static uint8_t cloud_palette[] = {
#if defined(PBL_COLOR)
    GColorPictonBlueARGB8, GColorPictonBlueARGB8, GColorCelesteARGB8, GColorWhiteARGB8
#else
    GBC_WHITE, GBC_WHITE, GBC_BLACK, GBC_BLACK
#endif
};

static uint8_t get_num_clouds() {
    return sizeof(cloud_data) / 3;
}

static void load_cloud_palette(GBC_Graphics *graphics, uint8_t palette_num) {
    // Here I copy a palette from an array
    GBC_Graphics_set_bg_palette_array(graphics, palette_num, cloud_palette);
}

static void draw_cloud_at_location(GBC_Graphics *graphics, uint8_t cloud_id, uint8_t x_pos, uint8_t y_pos, uint8_t cloud_vram_start) {
    const uint8_t *data = &cloud_data[cloud_id * 3];
    uint8_t vram_offset = data[0];
    uint8_t cloud_width = data[1];
    uint8_t cloud_height = data[2];
    uint8_t cloud_index = 0;
    uint8_t cloud_attrs = GBC_Graphics_attr_make(0, 0, false, false, false);

    for (uint8_t y = 0; y < cloud_height; y++) {
        for (uint8_t x = 0; x < cloud_width; x++) {
            GBC_Graphics_bg_set_tile_and_attrs(graphics, (x_pos + x) % TILEMAP_WIDTH, (y_pos + y) % TILEMAP_HEIGHT, vram_offset + cloud_vram_start + cloud_index, cloud_attrs);
            cloud_index++;
        }
    }
}

static void draw_cloud(GBC_Graphics *graphics, uint8_t cloud_id, uint8_t x_pos, uint8_t y_pos, uint8_t cloud_vram_start, uint8_t cloud_attrs) {
    const uint8_t *data = &cloud_data[cloud_id * 3];
    uint8_t vram_offset = data[0];
    uint8_t cloud_width = data[1];
    uint8_t cloud_height = data[2];

    uint8_t cloud_index = 0;
    for (uint8_t y = 0; y < cloud_height; y++) {
        for (uint8_t x = 0; x < cloud_width; x++) {
            GBC_Graphics_bg_set_tile_and_attrs(graphics, (x_pos + x) % TILEMAP_WIDTH, (y_pos + y) % TILEMAP_HEIGHT, vram_offset + cloud_vram_start + cloud_index, cloud_attrs);
            cloud_index++;
        }
    }
}

void draw_clouds(GBC_Graphics *graphics, uint8_t cloud_vram_start) {
    load_cloud_palette(graphics, CLOUD_PALETTE);
    uint8_t sky_attrs = GBC_Graphics_attr_make(0, 0, false, false, false);
    
    for (uint8_t x = 0; x < TILEMAP_WIDTH; x++) {
        for (uint8_t y = 0; y < TILEMAP_HEIGHT; y++) {
            GBC_Graphics_bg_set_tile_and_attrs(graphics, x, y, CLOUD_SKY_TILE, sky_attrs);
        }
    }

    uint8_t num_clouds_x = TILEMAP_WIDTH / s_cloud_box_dims[0];
    uint8_t num_clouds_y = TILEMAP_HEIGHT / s_cloud_box_dims[1];

    for (uint8_t y = 0; y < num_clouds_y; y++) {
        for (uint8_t x = 0; x < num_clouds_x; x++) {
            uint8_t cloud_id = rand() % get_num_clouds();
            const uint8_t *data = &cloud_data[cloud_id * 3];
            uint8_t vram_offset = data[0];
            uint8_t cloud_width = data[1];
            uint8_t cloud_height = data[2];
            uint8_t empty_x_tiles = s_cloud_box_dims[0] - cloud_width;
            uint8_t empty_y_tiles = s_cloud_box_dims[1] - cloud_height;

            uint8_t x_offset = rand()%empty_x_tiles;
            uint8_t y_offset = rand()%empty_y_tiles;
            bool should_cloud_be_foreground = rand()%2 == 0;
            uint8_t cloud_attrs = GBC_Graphics_attr_make(0, 0, false, false, should_cloud_be_foreground);

            uint8_t x_pos = (x) * s_cloud_box_dims[0] + x_offset;
            uint8_t y_pos = (y) * s_cloud_box_dims[1] + y_offset;
            draw_cloud(graphics, cloud_id, x_pos, y_pos, cloud_vram_start, cloud_attrs);
        }
    }
}