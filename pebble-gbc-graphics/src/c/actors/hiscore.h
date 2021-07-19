#pragma once

#include <pebble.h>
#include "../pebble-gbc-graphics/pebble-gbc-graphics.h"
#include "../util.h"

#define HI_SCORE_SPRITE_1 26
#define HI_SCORE_SPRITE_2 28
#define HI_SCORE_SPRITE_3 30
#define HI_SCORE_PALETTE 3
#define HI_SCORE_Y_OFFSET_MAX 3
#define HI_SCORE_Y_POS PBL_IF_ROUND_ELSE(62, 93)
#define HI_SCORE_X_POS PBL_IF_ROUND_ELSE(144, 128)
#define HI_SCORE_OFFSET 30

void hiscore_init(GBC_Graphics *graphics);
void hiscore_step(GBC_Graphics *graphics);
void hiscore_hide(GBC_Graphics *graphics);