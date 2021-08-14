#ifndef RGBHSI_H
#define RGBHSI_H
#include <stdint.h>
//Convert from hsi to rgbw
void hsitorgbw(float hue, float saturation, float intensity, uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white);

//Convert from rgb to hsi
void rgbtohsi(uint8_t red, uint8_t green, uint8_t blue, float *hue, float *saturation, float *intensity);

#endif
