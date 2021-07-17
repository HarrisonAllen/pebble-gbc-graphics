#include "util.h"

uint8_t base_vram_offset, cloud_vram_offset, fuel_vram_offset, road_vram_offset, sprites_vram_offset, text_vram_offset;

uint8_t clamp_uint8_t(uint8_t min, uint8_t val, uint8_t max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
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
}