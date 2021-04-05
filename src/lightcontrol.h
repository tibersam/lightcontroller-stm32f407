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
int get_waitlength(void);
int get_stepmode(void);
void set_waitlength(int wait);
void get_hsi(float *hue, float *sat, float *intens);
void setcycelhsi(float hue, float sat, float intens, float interval);

#endif
