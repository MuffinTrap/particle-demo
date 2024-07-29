#pragma once
static uint32_t rand_state = 1;

extern "C" {

static uint32_t super_rand(void) {
    uint32_t x = rand_state;
    return rand_state = (x * 0x6255 + 0x3619) & 0xFFFF;
}
/*static uint32_t super_rand_func(uint32_t x) {
    return (x * 0x6255 + 0x3619) & 0xFFFF;
}*/
static inline float super_randf()
{
    return super_rand()/(float)0xFFFF;
}

} // end extern "C"
