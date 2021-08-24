
#include "rgbhsi.h"
#include <math.h>

//#ifndef M_PI
//#define M_PI            3.14159265358979323846
//#endif

#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
/*
void hsitorgbw(float hue, float saturation, float intensity, uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white)
{
	float r, g, b, w;
	hue = fmod(hue, 360.0f);
	saturation = fmod(saturation, 1.0f);
	intensity = fmod(intensity, 1.0f);
	
	if(hue < 0.0)
	{
		hue = hue + 360.0f;
	}
	hue = hue * (float) M_PI / 180.0f;
	w = 0.0f;

	// If no collor make white
	if(saturation == 0.0f)
	{
		r = intensity;
		g = intensity;
		b = intensity;
	}
	else
	{	
		// if hue is in blue sector (0-120 grad)
		if(hue < 2.0f * ((float) M_PI / 3.0f))
		{
			b = (1.0f - saturation) / 3.0f;
			r = (1.0f + ((saturation * (float) cos(hue)) / ((float) cos( M_PI / 3.0f - hue))))/ 3.0f;
			g = 1.0f - b - r;
		}
		else
		{
			// if hue is in red sector(120-240 grad)
			if ( hue < 4.0f * ((float) M_PI / 3.0f))
			{
				hue = hue - 2.0f * ((float) M_PI / 3.0f);
				r = (1.0f - saturation) / 3.0f;
				g = (1.0f + ((saturation * (float) cos(hue)) / ((float) cos( M_PI / 3.0f - hue)))) / 3.0f;
				b = 1.0f - r - g;
			}
			else
			{
				hue = hue - 4.0f * ((float) M_PI / 3.0f);
				g = (1.0f - saturation) / 3.0f;
				b = (1.0f + ((saturation * (float) cos(hue)) / ((float) cos(M_PI / 3.0f - hue)))) / 3.0f;
				r = 1.0f - g - b;
			}
		}
		if( r < 0.0f)
			r = 0.0f;
		if( g < 0.0f)
			g = 0.0f;
		if( b < 0.0f)
			b = 0.0f;
		r = 3.0f * intensity * r;
		g = 3.0f * intensity * g;
		b = 3.0f * intensity * b;
		if( r > 1.0f)
			r = 1.0f;
		if( g > 1.0f)
			g = 1.0f;
		if( b > 1.0f)
			b = 1.0f;
	}

	*red = (uint8_t) 254.0f * r;
	*green = (uint8_t) 254.0f * g;
	*blue = (uint8_t) 254.0f * b;
	*white = (uint8_t) 254.0f * w;
}*/
/*
void hsitorgbw(float hue, float saturation, float intensity, uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white)
{
	uint8_t r, g, b, w;
	hue = fmod(hue, 360.0f);
	saturation = fmod(saturation, 1.0f);
	intensity = fmod(intensity, 1.0f);
	if(hue < 0.0)
	{
		hue = hue + 360.0f;
	}
	hue = hue * (float) M_PI / 180.0f;
	// if hue is in rg sector
	if(hue < 2.0f * (float) M_PI / 3.0f)
	{
		r = (uint8_t) saturation * 254.0f * intensity / 3.0f * (1.0f + (float) cos(hue) / (float) cos((float) (M_PI/3.0f)-hue));
		g = (uint8_t) saturation * 254.0f * intensity / 3.0f * (1.0f + (1.0f - (float) cos(hue) / (float) cos((float) (M_PI / 3.0f) - hue)));
		b = 0;
	}
	else{
		// if hue is in gb sector
		if(hue < 4.0f * (float) M_PI / 3.0f)
		{
			hue = hue - (2.0f * (float) M_PI / 3.0f);
			g = (uint8_t) saturation * 254.0f * intensity / 3.0f * (1.0f + (float) cos(hue) / (float) cos((float) (M_PI/3.0f)-hue));
			b = (uint8_t) saturation * 254.0f * intensity / 3.0f * (1.0f + (1.0f - (float) cos(hue) / (float) cos((float) (M_PI / 3.0f) - hue)));
			r = 0;
		}
		else
		{
			hue = hue - (4.0f * (float) M_PI / 3.0f);
			b = (uint8_t) saturation * 254.0f * intensity / 3.0f * (1.0f + (float) cos(hue) / (float) cos((float)(M_PI/3.0f)-hue));
			r = (uint8_t) saturation * 254.0f * intensity / 3.0f * (1.0f + (1.0f - (float) cos(hue) / (float) cos((float) (M_PI / 3.0f) + hue)));
			g = 0;
		}
	}
	w = (uint8_t) 254.0f * (1.0f - saturation) * intensity/3.0f;
	*red = r;
	*green = g;
	*blue = b;
	*white = w;
}
*/

// https://getreuer.info/posts/colorspace/index.html
// http://en.wikipedia.org/wiki/HSL_and_HSV
void rgbtohsi(uint8_t red, uint8_t green, uint8_t blue, float *hue, float *saturation, float *intensity)
{
	float alpha = 0.0f;
	float beta = 0.0f;

	float r = red / 255.0f;
	float g = green / 255.0f;
	float b = blue / 255.0f;

	*intensity = (r + g + b) / 3.0f;
	
	//alpha = r - g * cos(60°) - b * cos(60°) 
	alpha = 0.5f * (2.0f * r - g - b);
	// beta = g * sin(60°) - b * sin(60°) = (sqrt(3)/2) * (g - b)
	beta = 0.866025403784439f*(g - b);

	if(*intensity > 0)
	{
		*saturation = 1 - (MIN(r,MIN(g,b)))/ *intensity;
		//if(*saturation != 0)
		//{
			*hue = (float) atan2(beta, alpha) * (float) (180.0f / M_PI);
		//}
		if(*hue < 0)
		{
			*hue = *hue + 360.0f;
		}
	}
	else
	{
		*hue = *hue;
		*saturation = *saturation;
	}
}
/*
void rgbtohsi(uint8_t red, uint8_t green, uint8_t blue, float *hue, float *whitness, float *blackness)
{
	float r, g, b, h, w, v, bl, f;
	int i;
	r = red/255.0f;
	g = green/255.0f;
	b = blue/255.0f;

	w = MIN(r, MIN(g,b));
	v = MAX(r, MAX(g,b));
	bl = 1.0f - v;
	if(v == w)
	{
		*hue = *hue;
		*whitness = w;
		*blackness = bl;
		return;
	}
	// findout sector
	f = (r==w)?g-b:((g==w)?b-r:r-g);
	i = (r==w)?3:((g==w)?5:1); 
	h = i - f/(v-w);
	*hue = 60.0f* h; // go to deg representation
	if(*hue > 360.0f)
	{
		*hue = *hue - 360.0f;
	}
	*whitness = w;
	*blackness = bl;
}
*//*
void hwbtorgb(float hue, float whitness, float blackness, float *r, float *g, float *b);
void hwbtorgb(float hue, float whitness, float blackness, float *r, float *g, float *b)
{
	float h, w, bl, v, n, f;
	int i;
	h = hue;
	w = whitness;
	bl = blackness;

	v = 1.0f - bl;
	i = (int) floor(h);
	f = h - i;
	if (i & 1)
		f = 1 - f; // if i is odd
	n = w + f * (v - w); //linear interpolation between w and v
	switch(i)
	{
		case 6:
		case 0: //we are in red, now going to green
			*r = v; 
			*g = n;
			*b = w;
			break;
		case 1: // we are in green, going to red
			*r = n;
			*g = v;
			*b = w;
			break;
		case 2: // we are in green, going to blue
			*r = w;
			*g = v;
			*b = n;
			break;
		case 3: // we are in blue, going to green
			*r = w;
			*g = n;
			*b = v;
			break;
		case 4: // we are in blue going to red
			*r = n;
			*g = w;
			*b = v;
			break;
		case 5: // we are in red, going to blue
			*r = v;
			*g = w;
			*b = n;
			break;
	}
}
*/
/*
void hsitorgbw(float hue, float whitness, float blackness, uint8_t *red, uint8_t *green, uint8_t*blue, uint8_t *white)
{
	float r, g, b, h, w, bl, v, color_level, white_level;
	h = hue / 60.0f; // back to representation from 0-6
	if(h < 0.0f)
	{
		h = h + 6.0f;
	}
	if(h > 6.0f)
	{
		h = h - 6.0f;
	}
	w = 0.0f;
	bl = 0.0f;
	
	// we will now transform color thinking we are at max brightness and full collor, aka no white
	hwbtorgb(h, w, bl, &r, &g, &b);	
	// now get the real whiteness and blackness
	w = whitness;// (whitness<0.0f)?0.0f:(whitness>1.0f)?1.0f:whitness;
	bl = blackness;//(blackness<0.0f)?0.0f:(blackness>1.0f)? 1.0f:blackness;
	if(w + bl >= 1.0f)
	{
		//w = 1.0f - bl; // if w + b > 1, reduce w to match the condition
		w = w/(w+b);
		b = b/(w+b);
	}
	// inverse blackness, to get light brighness( blackness is 0 at full brighness)
	v = 1.0f - bl;
	// calculate real led levels:
	// 	      rgbw max  brighness   procentual color of this level
	color_level = 255.0f * v * (1-(w/v));
	white_level = 255.0f * v * (w/v);

	*red = (uint8_t) r * color_level;
	*green = (uint8_t) g * color_level;
	*blue = (uint8_t) b * color_level;
	*white = (uint8_t) w * white_level;

}
*/
/*
void hsitorgbw(float hue, float saturation, float intensity, uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white)
{
	float r, g, b, w;
	hue = hue - 360.0f * floor(hue/360.0f);

	if(hue < 120)
	{
		b =  (1.0f - saturation);
		r =  (1.0f + saturation*cos(hue*(M_PI/180.0f))/cos((60.0f - hue)*(M_PI/180.0f)));
		g =  (1.0f - r - b) * saturation;
		w = (1 - saturation);
	}
	else if( hue < 240)
	{
		hue = hue - 120.0f;
		r = (1.0f - saturation);
		g = (1.0f + saturation*cos(hue*(M_PI/180.0f))/cos((60.0f - hue)*(M_PI/180.0f)));
		b = (1.0f - r - g) * saturation;
		w = (1.0f - saturation);

	}
	else
	{
		hue = hue - 240.0f;
		g = (1.0f - saturation);
		b = (1.0f + saturation*cos(hue*(M_PI/180.0f))/cos((60.0f - hue)*(M_PI/180.0f)));
		r = (1.0f - g - b) * saturation;
		w = (1.0f - saturation);
	}
	r = 3.0f * intensity * r;
	g = 3.0f * intensity * g;
	b = 3.0f * intensity * b;
	w = 3.0f * intensity * w;
	r = r>0.0f?(r<1.0f?r:1.0f):0.0f;
	g = g>0.0f?(g<1.0f?g:1.0f):0.0f;
	b = b>0.0f?(b<1.0f?b:1.0f):0.0f;
	w = w>0.0f?(w<1.0f?w:1.0f):0.0f;
	*red = (uint8_t) 255.0f * r;
	*green = (uint8_t) 255.0f * g;
	*blue = (uint8_t) 255.0f * b;
	*white = (uint8_t) 255.0f * w;	
}*/
void hsitorgbw(float hue, float saturation, float intensity, uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white)
{
	uint8_t r, g, b, w;
	float cos_h, cos_1047_h;

	hue = fmod(hue,360);
	if(hue < 0)
	{
		hue = hue + 360;
	}
	hue = 3.14159f * hue/(float)180;
	saturation = saturation>0?(saturation<1?saturation:1):0;
	intensity = intensity>0?(intensity<1?intensity:1):0;

	if(hue < 2.09439f){
		cos_h = cos(hue);
		cos_1047_h = cos(1.047196667f - hue);
		r = saturation * 255.0f * (intensity/3.0f )*(1+cos_h/cos_1047_h);
		g = saturation * 255.0f * (intensity/3.0f )*(1+(1-cos_h/cos_1047_h));
		b = 0;
		w = 255.0f * intensity *(1 - saturation);
	}else if( hue < 4.188787f){
		hue = hue - 2.09439f;
		cos_h = cos(hue);
		cos_1047_h = cos(1.047196667f - hue);
		r = 0;
		g = saturation * 255.0f * (intensity/3.0f)*(1+cos_h/cos_1047_h);
		b = saturation * 255.0f * (intensity/3.0f)*(1+(1-cos_h/cos_1047_h));
		w = 255.0f * intensity *(1 - saturation);
	}else{
		hue = hue - 4.188787f;
		cos_h = cos(hue);
		cos_1047_h = cos(1.047196667f - hue);
		r = saturation * 255.0f * (intensity/3.0f)*(1+(1-cos_h/cos_1047_h));
		g = 0;
		b = saturation * 255.0f * (intensity/3.0f)*(1+cos_h/cos_1047_h);
		w = 255.0f * intensity *(1 - saturation);
	}
	r = r >0.0f?(r<255.0f?r:255.0f):0.0f;
	g = g >0.0f?(g<255.0f?g:255.0f):0.0f;
	b = b >0.0f?(b<255.0f?b:255.0f):0.0f;
	*red = (uint8_t)r;
	*green = (uint8_t)g;
	*blue = (uint8_t)b;
	*white = (uint8_t)w;
}
