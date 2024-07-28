// sync data
#include "../rocket/track.h"
struct track_key effect_active_keys[] = {{ 0, 3.000000, KEY_STEP}, };
const struct sync_track effect_active = { "effect_active", effect_active_keys,1};
struct track_key scene_rotX_keys[] = {{ 0, 15.000000, KEY_STEP}, };
const struct sync_track scene_rotX = { "scene_rotX", scene_rotX_keys,1};
struct track_key scene_rotY_keys[] = {{ 0, 0.000000, KEY_LINEAR}, { 23, 180.000000, KEY_STEP}, { 27, 50.000000, KEY_STEP}, };
const struct sync_track scene_rotY = { "scene_rotY", scene_rotY_keys,3};
struct track_key scene_rotZ_keys[] = {{ 0, 0.000000, KEY_STEP}, };
const struct sync_track scene_rotZ = { "scene_rotZ", scene_rotZ_keys,1};
struct track_key scene_X_keys[] = {{ 0, 0.000000, KEY_STEP}, };
const struct sync_track scene_X = { "scene_X", scene_X_keys,1};
struct track_key scene_Y_keys[] = {{ 0, -0.600000, KEY_STEP}, };
const struct sync_track scene_Y = { "scene_Y", scene_Y_keys,1};
struct track_key scene_Z_keys[] = {{ 0, -1.400000, KEY_STEP}, };
const struct sync_track scene_Z = { "scene_Z", scene_Z_keys,1};
struct track_key radar_dots_keys[] = {{ 0, 0.000000, KEY_STEP}, };
const struct sync_track radar_dots = { "radar_dots", radar_dots_keys,1};
struct track_key radar_seed_keys[] = {};
const struct sync_track radar_seed = { "radar_seed", radar_seed_keys,0};
struct track_key radar_spread_keys[] = {};
const struct sync_track radar_spread = { "radar_spread", radar_spread_keys,0};
struct track_key radar_rarity_keys[] = {};
const struct sync_track radar_rarity = { "radar_rarity", radar_rarity_keys,0};
struct track_key tuner_pos_keys[] = {};
const struct sync_track tuner_pos = { "tuner_pos", tuner_pos_keys,0};
struct track_key tuner_names_keys[] = {};
const struct sync_track tuner_names = { "tuner_names", tuner_names_keys,0};
