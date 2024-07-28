// sync data
#include "../rocket/track.h"
struct track_key radar_dots_keys[] = {{ 0, 0.000000, KEY_STEP}, { 1, 1.000000, KEY_STEP}, { 2, 2.000000, KEY_STEP}, { 3, 3.000000, KEY_STEP}, { 4, 4.000000, KEY_STEP}, { 5, 5.000000, KEY_STEP}, { 6, 6.000000, KEY_STEP}, { 7, 7.000000, KEY_LINEAR}, { 104, 20.000000, KEY_LINEAR}, };
const struct sync_track radar_dots = { "radar_dots", radar_dots_keys,9};
struct track_key radar_seed_keys[] = {{ 0, 100.000000, KEY_STEP}, };
const struct sync_track radar_seed = { "radar_seed", radar_seed_keys,1};
struct track_key radar_spread_keys[] = {{ 0, 0.500000, KEY_STEP}, };
const struct sync_track radar_spread = { "radar_spread", radar_spread_keys,1};
struct track_key radar_rarity_keys[] = {{ 0, 4.000000, KEY_STEP}, };
const struct sync_track radar_rarity = { "radar_rarity", radar_rarity_keys,1};
struct track_key tuner_pos_keys[] = {{ 0, 0.000000, KEY_STEP}, { 8, 0.000000, KEY_LINEAR}, { 29, 1.000000, KEY_LINEAR}, { 38, 1.700000, KEY_STEP}, };
const struct sync_track tuner_pos = { "tuner_pos", tuner_pos_keys,4};
struct track_key tuner_names_keys[] = {{ 0, 0.000000, KEY_STEP}, { 8, 0.000000, KEY_STEP}, { 15, 0.000000, KEY_STEP}, { 16, 1.000000, KEY_STEP}, };
const struct sync_track tuner_names = { "tuner_names", tuner_names_keys,4};
struct track_key credits_names_keys[] = {{ 42, 0.000000, KEY_STEP}, { 45, 1.000000, KEY_STEP}, { 48, 2.000000, KEY_STEP}, { 51, 3.000000, KEY_STEP}, { 55, 4.000000, KEY_STEP}, { 58, 5.000000, KEY_STEP}, };
const struct sync_track credits_names = { "credits_names", credits_names_keys,6};
