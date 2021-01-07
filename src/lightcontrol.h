#ifndef LIGHTCONTROL
#define LIGHTCONTROL

#include "ws2812b.h"

void setuplightcontroller(void);
void setstepspeed(void);
void calculatestep(void);
void setrgbvalues(int red, int green, int blue);
void sethsivalues(int hue, int saturation, int intensity);
void setlimitrgb(int red, int green, int blue, int offset, int length, int mode);
int set_stepmode(int step);
void set_waitlength(int wait);

#endif
