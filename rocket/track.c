#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>

#ifdef __cplusplus
    extern "C" {
#endif
#ifndef M_PI
#define M_PI 3.141926
#endif

#include "sync.h"
#include "track.h"
#include "base.h"

static double key_linear(const struct track_key k[2], double row)
{
    double t = (row - k[0].row) / (k[1].row - k[0].row);
    return k[0].value + (k[1].value - k[0].value) * t;
}

static double key_smooth(const struct track_key k[2], double row)
{
    double t = (row - k[0].row) / (k[1].row - k[0].row);
    t = t * t * (3 - 2 * t);
    return k[0].value + (k[1].value - k[0].value) * t;
}

static double key_ramp(const struct track_key k[2], double row)
{
    double t = (row - k[0].row) / (k[1].row - k[0].row);
    t = pow(t, 2.0);
    return k[0].value + (k[1].value - k[0].value) * t;
}

double sync_get_val(const struct sync_track *t, double row)
{
    int idx, irow;

    /* If we have no keys at all, return a constant 0 */
    if (!t->num_keys)
        return 0.0f;

    irow = (int)floor(row);
    idx = key_idx_floor(t, irow);

    /* at the edges, return the first/last value */
    if (idx < 0)
        return t->keys[0].value;
    if (idx > (int)t->num_keys - 2)
        return t->keys[t->num_keys - 1].value;

    /* interpolate according to key-type */
    switch (t->keys[idx].type) {
    case KEY_STEP:
        return t->keys[idx].value;
    case KEY_LINEAR:
        return key_linear(t->keys + idx, row);
    case KEY_SMOOTH:
        return key_smooth(t->keys + idx, row);
    case KEY_RAMP:
        return key_ramp(t->keys + idx, row);
    default:
        assert(0);
        return 0.0f;
    }
}

int sync_find_key(const struct sync_track *t, int row)
{
    int lo = 0, hi = t->num_keys;

    /* binary search, t->keys is sorted by row */
    while (lo < hi) {
        int mi = (lo + hi) / 2;
        assert(mi != hi);

        if (t->keys[mi].row < row)
            lo = mi + 1;
        else if (t->keys[mi].row > row)
            hi = mi;
        else
            return mi; /* exact hit */
    }
    assert(lo == hi);

    /* return first key after row, negated and biased (to allow -0) */
    return -lo - 1;
}

#ifndef SYNC_PLAYER
int sync_set_key(struct sync_track *t, const struct track_key *k)
{
    int idx = sync_find_key(t, k->row);
    if (idx < 0) {
        /* no exact hit, we need to allocate a new key */
        struct track_key *tmp;
        idx = -idx - 1;
        tmp = (struct track_key *)realloc(t->keys, sizeof(struct track_key) * (t->num_keys + 1));
        if (!tmp)
            return -1;
        t->num_keys++;
        t->keys = tmp;
        memmove(t->keys + idx + 1, t->keys + idx,
            sizeof(struct track_key) * (t->num_keys - idx - 1));
    }
    t->keys[idx] = *k;
    return 0;
}

void start_save_sync(const char *filename) {
    FILE *file = fopen(filename, "w");
    fprintf(file, "// sync data\n");
    fprintf(file, "#include \"../rocket/track.h\"\n");
    fclose(file);
}
void save_sync(const struct sync_track *t, const char *filename) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(file, "struct track_key %s_keys[] = {", t->name);
    for (int i = 0; i < t->num_keys; i++) {
        int row = t->keys[i].row;
        float value = t->keys[i].value;
        enum key_type type = t->keys[i].type;

        fprintf(file, "{ %d, %f, %d}, ", row, value, type);
    }
    fprintf(file, "};\n");

    fprintf(file, "const struct sync_track %s = { \"%s\", ", t->name, t->name);
    fprintf(file, "%s_keys", t->name);
    fprintf(file, ",%d};\n", t->num_keys);
    fclose(file);
}
int sync_del_key(struct sync_track *t, int pos)
{
    struct track_key *tmp;
    int idx = sync_find_key(t, pos);
    assert(idx >= 0);
    memmove(t->keys + idx, t->keys + idx + 1,
        sizeof(struct track_key) * (t->num_keys - idx - 1));
    assert(t->keys);
    tmp = (struct track_key *)realloc(t->keys, sizeof(struct track_key) *
        (t->num_keys - 1));
    if (t->num_keys != 1 && !tmp)
        return -1;
    t->num_keys--;
    t->keys = tmp;
    return 0;
}
#endif

#ifdef __cplusplus
	}
#endif
