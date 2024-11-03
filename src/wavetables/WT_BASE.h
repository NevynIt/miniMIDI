#ifndef WT_BASE_H
#define WT_BASE_H

#include <stdint.h>

#define WT_BASE_SAMPLE_RATE 1024
#define WT_BASE_sin (WT_BASE + 7)
#define WT_BASE_sin_SIZE 1024
#define WT_BASE_sqare (WT_BASE + 1031)
#define WT_BASE_sqare_SIZE 1024
#define WT_BASE_saw (WT_BASE + 2055)
#define WT_BASE_saw_SIZE 1024
#define WT_BASE_triangle (WT_BASE + 3079)
#define WT_BASE_triangle_SIZE 1024
#define WT_BASE_noise (WT_BASE + 4103)
#define WT_BASE_noise_SIZE 1024

extern const int16_t WT_BASE[5132];
#endif // WT_BASE_H
