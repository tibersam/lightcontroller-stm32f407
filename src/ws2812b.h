/*
 * WS2812B SPI driver works arround 3,2 MHZ
 * sk2812spi needs arround 800 KHZ
 * ref: https://stm32f4-discovery.net/2014/04/library-05-spi-for-stm32f4xx/
 */
#ifndef WS2812BHEADAER
#define WS2812BHEADAER

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>
#include <math.h>

#define NUMBERLED 432

//#define USEDMA

//#define USESPI1
#ifdef USESPI1
	#define OFFSETSPI1 0
	#define NUMBERLEDSPI1 339
#endif

#define USESPI2
#ifdef USESPI2
	#define OFFSETSPI2 0
	#define NUMBERLEDSPI2 340
#endif

#define USESPI3
#ifdef USESPI3
	#define OFFSETSPI3 340
	#define NUMBERLEDSPI3 92
#endif

//#define USESPI4
#ifdef USESPI4
	#define OFFSETSPI4 339
	#define NUMBERLEDSPI4 92
#endif

//#define WS2812BLED
//#define SK2812LED
#define SK2812SPI
#ifdef WS2812BLED
	#define NUMBERCOLLOR 3
	#define NUMBERINCREASE 2
#else
	#ifdef SK2812LED
		#define NUMBERCOLLOR 4
		#define NUMBERINCREASE 2
	#else
		#ifdef SK2812SPI
			#define NUMBERCOLLOR 4
			#define NUMBERINCREASE 1
		#endif
	#endif
#endif
#if NUMBERCOLLOR == 3
typedef struct{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
}LED;
#elif NUMBERCOLLOR == 4
typedef struct{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t white;
	float current_hue;
	float current_sat;
	float current_int;
	float step_hue;
	float step_sat;
	float step_int;
	int t;
}LED;
#endif

void initledmodule(void);
void setupmoduleclock(void);
void setupmodulegpio(void);
void setupspi(uint32_t spi, uint32_t rstspi);
void setupdma(uint32_t dma, uint8_t stream, uint32_t channel, uint32_t spi_address, uint32_t offset, uint16_t numberled);
uint8_t inflatetwobits(uint8_t data, uint8_t mask1, uint8_t mask0);
void inflatebyte(uint8_t data, int i);
void preparebuffer(void);
void force_update_led(void);
void sendbuffer(void);
void setLED(int numberled, uint8_t red, uint8_t green, uint8_t blue);
void getLED(int numberled, uint8_t *red, uint8_t *green, uint8_t *blue);
void setLEDred(int numberled, uint8_t red);
uint8_t getLEDred(int numberled);
void setLEDgreen(int numberled, uint8_t green);
uint8_t getLEDgreen(int numberled);
void setLEDblue(int numberled, uint8_t blue);
uint8_t getLEDblue(int numberled);
#if defined(SK2812LED) || defined(SK2812SPI)
void setLEDrgbw(int numberled, uint8_t red, uint8_t green, uint8_t blue, uint8_t white);
void setLEDwhite(int numberled, uint8_t white);
uint8_t getLEDwhite(int numberled);
void getrgbwLED(int numberled, uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white);
void setLEDrgbhsi(int numberled, uint8_t red, uint8_t green, uint8_t blue);
/*
void rgbtohsi(uint8_t red, uint8_t green, uint8_t blue,float * hue, float *saturation, float *intensity);
void hsitorgb(float hue, float saturation, float intensity, uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white);
*/
void testfunction(float hue, float saturation, float intensity, uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white);

#endif

#endif
