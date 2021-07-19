#include "util.h"

uint8_t base_vram_offset, cloud_vram_offset, fuel_vram_offset, road_vram_offset, sprites_vram_offset, text_vram_offset;

const uint8_t ease_in_out_percentages[] = {
  0,
  2,
  4,
  8,
  16,
  26,
  38,
  50,
  62,
  74,
  84,
  92,
  96,
  98,
  100
};

const uint8_t ease_out_percentages[] = {
  0,
  2,
  4,
  6,
  8,
  12,
  16,
  24,
  32,
  42,
  52,
  64,
  76,
  88,
  100
};

const uint8_t ease_out_slow_percentages[] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  8,
  10,
  12,
  14,
  16,
  20,
  24,
  28,
  32,
  37,
  42,
  47,
  52,
  58,
  64,
  70,
  76,
  82,
  88,
  94,
  100
};

static int8_t s_dir_to_point[] = {
  // x, y
  0, 0, // D_NONE
  0, -1, // D_UP
  -1, 0, // D_LEFT
  0, 1, // D_DOWN
  1, 0, // D_RIGHT
  0, 0, // D_END
};

int clamp_int(int min, int val, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

uint8_t clamp_uint8_t(uint8_t min, uint8_t val, uint8_t max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

uint8_t clamp_int8_t(int8_t min, int8_t val, int8_t max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

int8_t *dir_to_point(Direction dir) {
    return &s_dir_to_point[dir * 2];
}

bool grects_overlap(GRect rect1, GRect rect2) {
  // Check if rect1 is right of rect2 or rect2 is right of rect1
  if ((rect1.origin.x > rect2.origin.x + rect2.size.w) || (rect2.origin.x > rect1.origin.x + rect1.size.w)) {
    return false;
  }

  // Check if rect1 is below rect2 or rect2 is below rect1
  if ((rect1.origin.y > rect2.origin.y + rect2.size.h) || (rect2.origin.y > rect1.origin.y + rect1.size.h)) {
    return false;
  }

  // Otherwise they overlap!
  return true;
}

// Load the tilesheets
void load_tilesheets(GBC_Graphics *graphics) {
  ResHandle handle = resource_get_handle(RESOURCE_ID_DATA_BASE_TILESHEET);
  size_t res_size = resource_size(handle);
  uint16_t tiles_to_load = res_size / TILE_SIZE; // Can change this if you only want to load a certain number of tiles

  uint8_t tilesheet_start_offset = 0; // This is the tile on the tilesheet we want to start loading from
  uint8_t vram_start_offset = 0; // This is the tile in the VRAM we want to start loading into
  uint8_t vram_bank = 0; // The VRAM bank we want to store the tiles into
  GBC_Graphics_load_from_tilesheet_into_vram(graphics, RESOURCE_ID_DATA_BASE_TILESHEET, tilesheet_start_offset, 
                                             tiles_to_load, vram_start_offset, vram_bank);
  vram_start_offset += tiles_to_load;
  cloud_vram_offset = vram_start_offset;
  handle = resource_get_handle(RESOURCE_ID_DATA_CLOUD_TILESHEET);
  res_size = resource_size(handle);
  tiles_to_load = res_size / TILE_SIZE;
  GBC_Graphics_load_from_tilesheet_into_vram(graphics, RESOURCE_ID_DATA_CLOUD_TILESHEET, tilesheet_start_offset, 
                                             tiles_to_load, vram_start_offset, vram_bank);

  vram_start_offset += tiles_to_load;
  fuel_vram_offset = vram_start_offset;
  handle = resource_get_handle(RESOURCE_ID_DATA_FUEL_BAR_TILESHEET);
  res_size = resource_size(handle);
  tiles_to_load = res_size / TILE_SIZE;
  GBC_Graphics_load_from_tilesheet_into_vram(graphics, RESOURCE_ID_DATA_FUEL_BAR_TILESHEET, tilesheet_start_offset, 
                                             tiles_to_load, vram_start_offset, vram_bank);
                                             
  vram_start_offset += tiles_to_load;
  road_vram_offset = vram_start_offset;
  handle = resource_get_handle(RESOURCE_ID_DATA_ROAD_TILESHEET);
  res_size = resource_size(handle);
  tiles_to_load = res_size / TILE_SIZE;
  GBC_Graphics_load_from_tilesheet_into_vram(graphics, RESOURCE_ID_DATA_ROAD_TILESHEET, tilesheet_start_offset, 
                                             tiles_to_load, vram_start_offset, vram_bank);

  vram_start_offset += tiles_to_load;
  sprites_vram_offset = vram_start_offset;
  handle = resource_get_handle(RESOURCE_ID_DATA_SPRITES_TILESHEET);
  res_size = resource_size(handle);
  tiles_to_load = res_size / TILE_SIZE;
  GBC_Graphics_load_from_tilesheet_into_vram(graphics, RESOURCE_ID_DATA_SPRITES_TILESHEET, tilesheet_start_offset, 
                                             tiles_to_load, vram_start_offset, vram_bank);

  vram_start_offset += tiles_to_load;
  text_vram_offset = vram_start_offset;
  handle = resource_get_handle(RESOURCE_ID_DATA_TEXT_TILESHEET);
  res_size = resource_size(handle);
  tiles_to_load = res_size / TILE_SIZE;
  GBC_Graphics_load_from_tilesheet_into_vram(graphics, RESOURCE_ID_DATA_TEXT_TILESHEET, tilesheet_start_offset, 
                                             tiles_to_load, vram_start_offset, vram_bank);

  APP_LOG(APP_LOG_LEVEL_INFO, "Using %d/%d tiles in VRAM", vram_start_offset + tiles_to_load, UINT8_MAX+1);
}

int lerp(int start, int end, int percent) {
  return start + (percent * (end - start) / 100);
}