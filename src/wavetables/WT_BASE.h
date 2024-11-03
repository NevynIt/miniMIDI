#ifndef WT_BASE_H
#define WT_BASE_H

#include <stdint.h>

#define WT_BASE_SAMPLE_RATE 1024
#define WT_BASE_sin (WT_BASE + 12)
#define WT_BASE_sin_SIZE 1024
#define WT_BASE_sqare (WT_BASE + 1036)
#define WT_BASE_sqare_SIZE 1024
#define WT_BASE_saw (WT_BASE + 2060)
#define WT_BASE_saw_SIZE 1024
#define WT_BASE_triangle (WT_BASE + 3084)
#define WT_BASE_triangle_SIZE 1024
#define WT_BASE_noise (WT_BASE + 4108)
#define WT_BASE_noise_SIZE 1024

extern const int16_t WT_BASE[5132];
#endif // WT_BASE_H
