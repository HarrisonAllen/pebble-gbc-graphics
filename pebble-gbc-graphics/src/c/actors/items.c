#include "items.h"
#include "player.h"
#include "../graphics/background.h"

// contains item_type, x_position, y_position
static uint s_items_data[NUMBER_OF_ITEMS][3];

extern uint8_t sprites_vram_offset;

static uint8_t s_item_sprites[] = {
    BALLOON_SPRITE,
    PLUS_ONE_SPRITE,
    FUEL_SPRITE,
    PLUS_FUEL_SPRITE
};

static uint8_t s_item_palettes[] = {
    BALLOON_PALETTE,
    BALLOON_PALETTE,
    FUEL_PALETTE,
    FUEL_PALETTE
};

static void set_item(GBC_Graphics *graphics, uint8_t item_index) {
    uint8_t item_odds = (rand() % NUMBER_OF_ITEMS);
    s_items_data[item_index][0] = (item_odds < BALLOON_ODDS) ? BALLOON_ID : FUEL_ID;
    s_items_data[item_index][1] = get_player_x() + GBC_Graphics_get_screen_width(graphics) + rand() % (GBC_Graphics_get_screen_width(graphics) * 3);
    s_items_data[item_index][2] = MIN_PLAYER_Y + rand() % (MAX_PLAYER_Y - MIN_PLAYER_Y);
    uint8_t item_attrs = GBC_Graphics_attr_make(s_item_palettes[s_items_data[item_index][0]], 0, false, false, false);
    GBC_Graphics_oam_set_sprite(graphics, ITEM_SPRITE_OFFSET + item_index, 0, 0, sprites_vram_offset + s_item_sprites[s_items_data[item_index][0]], item_attrs);
}

static void setup_item_palettes(GBC_Graphics *graphics) {
#if defined(PBL_COLOR)
    GBC_Graphics_set_sprite_palette(graphics, BALLOON_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorRedARGB8, GColorWhiteARGB8);
    GBC_Graphics_set_sprite_palette(graphics, FUEL_PALETTE, GColorPictonBlueARGB8, GColorBlackARGB8, GColorIslamicGreenARGB8, GColorIslamicGreenARGB8);
#else
    GBC_Graphics_set_sprite_palette(graphics, BALLOON_PALETTE, GBC_WHITE, GBC_BLACK, GBC_GRAY, GBC_WHITE);
    GBC_Graphics_set_sprite_palette(graphics, FUEL_PALETTE, GBC_WHITE, GBC_BLACK, GBC_GRAY, GBC_WHITE);
#endif
}

void items_init(GBC_Graphics *graphics) {
    setup_item_palettes(graphics);
    for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
        set_item(graphics, i);
    }
}

static bool is_item_on_screen(GBC_Graphics *graphics, uint8_t item_index) {
    int item_screen_x = s_items_data[item_index][1] - get_player_x();
    int item_y = s_items_data[item_index][2] - get_player_y();
    return false;
}

static void draw_item_sprites(GBC_Graphics *graphics) {
    for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
        int item_screen_y = s_items_data[i][2] - get_bg_scroll_y(graphics) + SPRITE_OFFSET_Y;
        if (item_screen_y <= 0 || item_screen_y > (GBC_Graphics_get_screen_height(graphics) + TILE_HEIGHT * 2)) {
            GBC_Graphics_oam_hide_sprite(graphics, ITEM_SPRITE_OFFSET + i);
        } else {
            int item_screen_x = s_items_data[i][1] - get_player_x() + get_player_screen_x();
            if (item_screen_x <= 0 || item_screen_x > (GBC_Graphics_get_screen_width(graphics) + TILE_WIDTH)) {
                GBC_Graphics_oam_hide_sprite(graphics, ITEM_SPRITE_OFFSET + i);
            } else {
                GBC_Graphics_oam_set_sprite_pos(graphics, ITEM_SPRITE_OFFSET + i, item_screen_x, item_screen_y);
            }
        }
    }
}

GRect get_item_collision(uint8_t item_id) {
    uint *item = s_items_data[item_id];
    return GRect(item[1], item[2], TILE_WIDTH, TILE_HEIGHT * 2);
}

uint *get_item(uint8_t item_id) {
    return s_items_data[item_id];
}

void handle_collision(GBC_Graphics *graphics, uint8_t item_id) {
    uint *item = s_items_data[item_id];
    switch (item[0]) {
        case BALLOON_ID:
            item[0] = PLUS_ONE_ID;
            GBC_Graphics_oam_set_sprite_tile(graphics, ITEM_SPRITE_OFFSET + item_id, sprites_vram_offset + s_item_sprites[item[0]]);
            break;
        case FUEL_ID:
            item[0] = PLUS_FUEL_ID;
            GBC_Graphics_oam_set_sprite_tile(graphics, ITEM_SPRITE_OFFSET + item_id, sprites_vram_offset + s_item_sprites[item[0]]);
            break;
        default:
            break;
    }
}

void items_step(GBC_Graphics *graphics) {
    for (uint8_t i = 0; i < NUMBER_OF_ITEMS; i++) {
        uint *item= s_items_data[i];
        item[1] -= 1;
        if (item[0] == PLUS_ONE_ID || item[0] == PLUS_FUEL_ID) {
            item[2] -= 1;
        }
        int item_screen_x = item[1] - get_player_x() + get_player_screen_x();
        if (item_screen_x <= 0) {
            set_item(graphics, i);
        }
    }
    draw_item_sprites(graphics);
}