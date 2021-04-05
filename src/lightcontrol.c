#include "lightcontrol.h"

extern LED lights[NUMBERLED];
int stepmode;
int waitlength;
extern int incrementwaittimer;


void setuplightcontroller(void)
{
	stepmode = 1;
	waitlength = 2;
}

int set_stepmode(int step)
{
	if( (step >= 0) && (step <= 2))
	{
		stepmode = step;
		return 0;
	}
	else
	{
		stepmode = 0;
		return -1;
	}
}

int get_stepmode(void)
{
	return stepmode;
}

void set_waitlength(int wait)
{
	waitlength = wait;
}

int get_waitlength(void)
{
	return waitlength;
}

void get_hsi(float *hue, float *sat, float *intens)
{
	*hue = lights[60].current_hue;
	*sat = lights[60].current_sat;
	*intens = lights[60].current_int;
}

void setcycelhsi(float hue, float sat, float intens, float interval)
{
  int i = 0;
  float value1;
  float value2;
  float target_hue = hue;
  float target_sat = sat;
  float target_int = intens;
  //rgbtohsi((uint8_t)red,(uint8_t) green,(uint8_t) blue, &target_hue, &target_sat, &target_int );
   for(i = 0; i < NUMBERLED; i++)
   {
     value1 = target_hue - lights[i].current_hue;
     if(value1 >= 180.0f )
     {
       value2 = (value1 - 360.0f);
     }
     else
     {
       if(value1 <= -180.0f)
       {
         value2 = 360.0f + value1;
       }
       else
       {
         value2 = value1;
       }
     }
     lights[i].step_hue = value2/interval;
     lights[i].step_sat = ((target_sat - lights[i].current_sat)/interval);
     lights[i].step_int = ((target_int - lights[i].current_int)/interval);
   }
   setstepspeed();
}

void setrgbvalues(int red, int green, int blue)
{
  int i = 0;
  float value1;
  float value2;
  float target_hue;
  float target_sat;
  float target_int;
  rgbtohsi((uint8_t)red,(uint8_t) green,(uint8_t) blue, &target_hue, &target_sat, &target_int );
   for(i = 0; i < NUMBERLED; i++)
   {
     value1 = target_hue - lights[i].current_hue;
     if(value1 >= 180.0f )
     {
       value2 = (value1 - 360.0f);
     }
     else
     {
       if(value1 <= -180.0f)
       {
         value2 = 360.0f + value1;
       }
       else
       {
         value2 = value1;
       }
     }
     lights[i].step_hue = value2/255.0f;
     lights[i].step_sat = ((target_sat - lights[i].current_sat)/255.0f);
     lights[i].step_int = ((target_int - lights[i].current_int)/255.0f);
   }
   setstepspeed();
}

void setstepspeed(void)
{
  int i = 0;
  switch(stepmode)
  {
    case 0:
      for(i = 0; i < NUMBERLED; i++)
      {
        lights[i].t = 0;
      }
      break;
    case 1:
      for(i = 0; i < 61; i++)
      {
        lights[((60-i)%NUMBERLED)].t = (-1)* i*(waitlength);
      }
      for(i = 0; i < 92; i++)
      {
        lights[i+OFFSETSPI3].t = (-1) * (i+61)*(waitlength);
      }
      for(i = 0; i < 279; i++)
      {
        lights[(61+i)%NUMBERLED].t = (-1)* i*(waitlength);
      }
      break;
    case 2:
      for(i = 0; i < 61; i++)
      {
        lights[((60-i)%NUMBERLED)].t = (-1)* ((61-i)+92 + 124)*(waitlength);
      }
      for(i = 0; i < 92; i++)
      {
        lights[i+OFFSETSPI3].t = (-1) * (92-i + 124)*(waitlength);
      }
      for(i = 0; i < 279; i++)
      {
        lights[(61+i)%NUMBERLED].t = (-1)* (278-i)*(waitlength);
      }
      break;
    default:

    break;
  }
}


void sethsivalues(int hue, int saturation, int intensity)
{
  int i = 0;
  for(i = 0; i < NUMBERLED; i++)
  {
    lights[i].step_hue = (lights[i].current_hue -(float) hue)/254.0f;
    lights[i].step_sat = (lights[i].current_sat -(((float)(saturation % 254))/254.0f))/254.0f;
    lights[i].step_int = (lights[i].current_int -(((float)(intensity % 254))/254.0f)/254.0f);
  }
  setstepspeed();
}

void setlimitrgb(int red, int green, int blue, int offset, int length, int mode)
{
  int i = 0;
  float target_hue;
  float target_sat;
  float target_int;
  float value1;
  float value2;

  rgbtohsi((uint8_t)red,(uint8_t) green,(uint8_t) blue, &target_hue, &target_sat, &target_int );
   for(i = 0; i < length; i++)
   {
     value1 = target_hue - lights[i+offset].current_hue;
     if(value1 >= 180.0f )
     {
       value2 = (value1 - 360.0f);
     }
     else
     {
       if(value1 <= -180.0f)
       {
         value2 = 360.0f + value1;
       }
       else
       {
         value2 = value1;
       }
     }
     lights[i+offset].step_hue = value2/255.0f;
     lights[i+offset].step_sat = ((target_sat - lights[i+offset].current_sat)/255.0f);
     lights[i+offset].step_int = ((target_int - lights[i+offset].current_int)/255.0f);
   }

  switch(mode)
  {
    case 0:
    for(i = 0; i < length; i++)
    {
      lights[i+offset].t = 0;
    }
    break;
    case 1:
    for(i = 0; i < length; i++)
    {
      lights[i+offset].t = (-1) * i* waitlength;
    }
    break;
    case 2:
    for(i = 0; i < length; i++)
    {
      lights[i+offset].t = (-1) * (length - i)* waitlength;
    }
    break;
    default:
    for(i = 0; i < length; i++)
    {
      lights[i+offset].t = 0;
    }
    break;
  }
}

void calculatestep(void)
{
  int i = 0;
  float hack = 0.0;
  for(i = 0; i < NUMBERLED; i++)
  {
    if(lights[i].t >= 0)
    {
       if(lights[i].t < 256)
       {
         hack = fmod(lights[i].step_hue + lights[i].current_hue,360);
         if(hack < 0.0f)
         {
           hack = hack + 360.0f;
         }
         lights[i].current_hue = hack;
         lights[i].current_int += lights[i].step_int;
         lights[i].current_sat += lights[i].step_sat;
         hsitorgb(lights[i].current_hue, lights[i].current_sat, lights[i].current_int,&(lights[i].red),&(lights[i].green), &(lights[i].blue), &(lights[i].white));
         incrementwaittimer = 1200000000;
	}
    }
    if(lights[i].t < 270)
    {
    lights[i].t += 1;
    }
  }
}
