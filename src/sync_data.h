// sync data
#include "../rocket/track.h"
struct track_key clear_r_keys[] = {{ 0, 1.000000, KEY_STEP}, { 8, 0.700000, KEY_STEP}, { 17, 0.500000, KEY_STEP}, { 24, 0.450000, KEY_STEP}, { 40, 0.300000, KEY_STEP}, { 53, 0.200000, KEY_STEP}, { 69, 0.100000, KEY_STEP}, { 91, 0.900000, KEY_STEP}, };
const struct sync_track clear_r = { "clear_r", clear_r_keys,8};
struct track_key clear_g_keys[] = {};
const struct sync_track clear_g = { "clear_g", clear_g_keys,0};
struct track_key clear_b_keys[] = {};
const struct sync_track clear_b = { "clear_b", clear_b_keys,0};
