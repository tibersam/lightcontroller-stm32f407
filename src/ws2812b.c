#include "ws2812b.h"

#include "wait.h"

#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#define MIN(x,y) (((x) < (y)) ? (x) : (y))


LED lights[NUMBERLED];
uint16_t ledbuffer[NUMBERLED * NUMBERCOLLOR * NUMBERINCREASE];

#ifndef USEDMA
#ifdef USESPI2
	int enable_send_spi2 = 0;  
#endif
#ifdef USESPI3
	int enable_send_spi3 = 0;
#endif
#endif


int incrementwaittimer;

uint8_t myround (float x);

uint8_t myround (float x)
{
	return (uint8_t) (x + 0.5);
}



void initledmodule(void)
{
#ifndef USEDMA
#ifdef USESPI2
	enable_send_spi2 = 0;  
#endif
#ifdef USESPI3
	enable_send_spi3 = 0;
#endif
#endif
	incrementwaittimer = 0;
	int i = 0;
	setupmoduleclock();
	setupmodulegpio();
#ifdef USESPI1
	setupspi(SPI1);
#endif
#ifdef USESPI4
	setupspi(SPI4);
#endif
#ifdef USESPI2
	setupspi(SPI2);
#endif
#ifdef USESPI3
	setupspi(SPI3);
#endif
	for(i = 0; i < NUMBERLED; i++)
	{
		lights[i].red = 0;
		lights[i].green = 0;
		lights[i].blue = 0;
#if defined(SK2812LED) || defined(SK2812SPI)
		lights[i].white = 0;
		lights[i].current_hue = 0.0;
		lights[i].current_sat = 0.0;
		lights[i].current_int = 0.0;
		lights[i].step_hue = 0.0;
		lights[i].step_sat = 0.0;
		lights[i].step_int = 0.0;
		lights[i].t = 0;
#endif
	}
}

void setupmodulegpio(void)
{
#ifdef USESPI1
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO5|GPIO7);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO4);
	gpio_set_af(GPIOA, GPIO_AF5, GPIO5|GPIO7);//GPIO6 ignored as it is MISO
	gpio_clear(GPIOA, GPIO4);
#endif
#ifdef USESPI2
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO13|GPIO15);
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO12);
	gpio_set_af(GPIOB, GPIO_AF5, GPIO13|GPIO15);//GPIO14 ignored as it is MISO
	gpio_set(GPIOB, GPIO12);
#endif
#ifdef USESPI3
	gpio_mode_setup(GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO10); //clock signal
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO9); //spi enable
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO5); //MOSI
	gpio_set_af(GPIOB, GPIO_AF6, GPIO5); 
	gpio_set_af(GPIOC, GPIO_AF6, GPIO10);//GPIO11 ignored as it is MISO
	gpio_set(GPIOC, GPIO9);
	//
	/*gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO10 | GPIO9);
	gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO5);
	gpio_set(GPIOC, GPIO9 | GPIO10);
	gpio_set(GPIOB, GPIO5);*/
#endif
#ifdef USESPI4
	gpio_mode_setup(GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO5|GPIO6|GPIO2);
	gpio_set_af(GPIOE, GPIO_AF5,GPIO5|GPIO6|GPIO2);
#endif
}

void setupmoduleclock(void)
{
#if defined(USESPI1) || defined(USESPI4)
	rcc_periph_clock_enable(RCC_DMA2);
#endif
#if defined(USESPI2) || defined(USESPI3)
	rcc_periph_clock_enable(RCC_DMA1);
#endif
#ifdef USESPI1
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_SPI1);
#endif
#ifdef USESPI2
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_SPI2);
	#ifdef USEDMA
	nvic_enable_irq(NVIC_DMA1_STREAM4_IRQ);
	#else
	nvic_enable_irq(NVIC_SPI2_IRQ);
	#endif
#endif
#ifdef USESPI3
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_SPI3);
#ifdef USEDMA
	nvic_enable_irq(NVIC_DMA1_STREAM5_IRQ);
#else
	nvic_enable_irq(NVIC_SPI3_IRQ);
#endif
#endif
#ifdef USESPI4
	rcc_periph_clock_enable(RCC_GPIOE);
	rcc_periph_clock_enable(RCC_SPI4);
#endif
}
void setupspi(uint32_t spi)
{
	spi_reset(spi);
	spi_set_baudrate_prescaler(spi,5);
	
	spi_set_standard_mode(spi, 1);
	spi_enable_software_slave_management(spi);
	spi_set_nss_high(spi);
	spi_disable_error_interrupt(spi);
	spi_disable_rx_buffer_not_empty_interrupt(spi);
	spi_disable_crc(spi);
	//set baudrate to 800Mhz
	//diff ist sysclock / (ahbprescaler * spiprescaler)
	//	     168000000 / (4*64)
	spi_set_bidirectional_transmit_only_mode(spi);
	spi_send_msb_first(spi);
	spi_disable_tx_buffer_empty_interrupt(spi);
	//  spi_set_next_tx_from_buffer(SPI1);
	//spi_enable_ss_output(spi);
	spi_set_master_mode(spi);
	//spi_enable_tx_buffer_empty_interrupt(spi);
	//spi_enable_tx_dma(spi);
	//SPI_CR1(spi) |= 0x0800;
	spi_enable(spi);
}

void setupdma(uint32_t dma, uint8_t stream, uint32_t channel, uint32_t spi_address, uint32_t offset, uint16_t numberled)
{
	if((numberled + offset-1) < NUMBERLED )
	{
	/*	dma_disable_stream(dma, stream);

		dma_stream_reset(dma, stream);

		dma_set_peripheral_address(dma, stream, spi_address);
		
		dma_set_memory_address(dma, stream, (uint32_t) &ledbuffer[offset * NUMBERCOLLOR * NUMBERINCREASE]);
		dma_set_number_of_data(dma, stream, (uint32_t) numberled * NUMBERCOLLOR * NUMBERINCREASE);
		
		dma_channel_select(dma, stream, channel);
		
		dma_set_dma_flow_control(dma, stream);
		
		dma_set_priority(dma, stream, DMA_SxCR_PL_VERY_HIGH);
		
		dma_set_transfer_mode(dma, stream, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
		
		dma_set_peripheral_burst(dma, stream, DMA_SxCR_PBURST_SINGLE);
		
		dma_set_memory_burst(dma, stream, DMA_SxCR_MBURST_SINGLE);
		
		dma_enable_direct_mode(dma, stream);
		
		dma_set_memory_size(dma, stream, DMA_SxCR_MSIZE_8BIT);
		
		dma_set_peripheral_size(dma, stream, DMA_SxCR_PSIZE_8BIT);
		
		dma_enable_memory_increment_mode(dma, stream);
		//dma_disable_peripheral_increment_mode(dma, stream);	
		dma_enable_transfer_complete_interrupt(dma, stream);
		
		dma_disable_half_transfer_interrupt(dma, stream);*/
  dma_stream_reset(dma, stream);
  dma_set_peripheral_address(dma, stream, spi_address);
  dma_set_memory_address(dma, stream, (uint32_t) &ledbuffer[offset * NUMBERCOLLOR * NUMBERINCREASE]);
  dma_set_number_of_data(dma, stream, (uint16_t) numberled * NUMBERCOLLOR * NUMBERINCREASE);
  dma_channel_select(dma, stream, channel);
  dma_set_dma_flow_control(dma, stream);
  dma_set_priority(dma, stream, DMA_SxCR_PL_VERY_HIGH);
  dma_set_transfer_mode(dma, stream, DMA_SxCR_DIR_MEM_TO_PERIPHERAL);
  //dma_set_read_from_memory(DMA1, DMA_CHANNEL3);
  dma_set_peripheral_burst(dma, stream, DMA_SxCR_PBURST_SINGLE);
  dma_set_memory_burst(dma, stream, DMA_SxCR_MBURST_SINGLE);
  dma_enable_direct_mode(dma, stream);
  dma_set_memory_size(dma, stream, DMA_SxCR_MSIZE_8BIT);
  dma_set_peripheral_size(dma, stream, DMA_SxCR_PSIZE_8BIT);
  dma_enable_memory_increment_mode(dma, stream);
//  dma_enable_stream(dma, stream);
		if(spi_address == SPI1_DR)
		{
			gpio_clear(GPIOA, GPIO4);
		}
		if(spi_address == SPI2_DR)
		{
			gpio_clear(GPIOB, GPIO12);
		}
		if(spi_address == SPI3_DR)
		{
			gpio_clear(GPIOC, GPIO9);
		}
		dma_enable_stream(dma, stream);
	}
}

#ifndef SK2812SPI
uint8_t inflatetwobits(uint8_t data, uint8_t mask1, uint8_t mask0)
{
	uint8_t value = 0x00;
#ifdef WS2812BLED
	uint8_t highbit = 0x0E;
	uint8_t lowbit = 0x08;
#else
#ifdef SK2812LED
	uint8_t highbit = 0x0C;
	uint8_t lowbit = 0x08;  
#endif

#endif

	if((data & mask1) !=0){
		value = highbit << 4;
	}
	else{
		value = lowbit << 4;
	}
	if((data & mask0) !=0){
		value += highbit;
	}
	else{
		value += lowbit;
	}

	return value;
}

void inflatebyte(uint8_t data, int i)
{
	//  uint8_t mask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	uint8_t firstbyte;
	uint8_t secondbyte;
	firstbyte = inflatetwobits(data,0x02,0x01);
	secondbyte = inflatetwobits(data,0x08,0x04);
	ledbuffer[i+1] = (secondbyte << 8) + firstbyte;
	firstbyte = inflatetwobits(data,0x20,0x10);
	secondbyte = inflatetwobits(data,0x80,0x40);
	ledbuffer[i] = (secondbyte << 8) + firstbyte;
}
#endif
void preparebuffer(void)
{
#ifdef WS2812BLED
	int i = 0;
	for(i=0; i < NUMBERLED; i++){
		inflatebyte(lights[i].green,i*NUMBERCOLLOR * NUMBERINCREASE);
		inflatebyte(lights[i].red,i*NUMBERCOLLOR * NUMBERINCREASE+2);
		inflatebyte(lights[i].blue, i* NUMBERCOLLOR* NUMBERINCREASE+4);
	}
#else
#ifdef SK2812LED
	int i = 0;
	for(i = 0; i < NUMBERLED; i++)
	{
		inflatebyte(lights[i].green,i*NUMBERCOLLOR * NUMBERINCREASE);
		inflatebyte(lights[i].red, i* NUMBERCOLLOR * NUMBERINCREASE +2);
		inflatebyte(lights[i].blue, i* NUMBERCOLLOR * NUMBERINCREASE +4);
		inflatebyte(lights[i].white, i* NUMBERCOLLOR * NUMBERINCREASE +6);
	}
	else
#else
#ifdef SK2812SPI
	int i = 0;
	for(i = 0; i < NUMBERLED; i++)
	{
		//ledbuffer[i*(NUMBERCOLLOR)] = (lights[i].red << 8) + lights[i].green;
		//ledbuffer[i*NUMBERCOLLOR+1] = (lights[i].white << 8) + lights[i].blue;
		ledbuffer[i*(NUMBERCOLLOR)*NUMBERINCREASE] = lights[i].green;
		ledbuffer[i*(NUMBERCOLLOR)*NUMBERINCREASE+1] = lights[i].red;
		ledbuffer[i*NUMBERCOLLOR * NUMBERINCREASE+2] = lights[i].blue;
		ledbuffer[i*NUMBERCOLLOR * NUMBERINCREASE+3] = lights[i].white;
	}
#endif
#endif
}

void setLED(int numberled, uint8_t red, uint8_t green, uint8_t blue)
{
	lights[numberled].red = red;
	lights[numberled].green = green;
	lights[numberled].blue = blue;
}
void setLEDred(int numberled, uint8_t red)
{
	lights[numberled].red = red;
}

void setLEDgreen(int numberled, uint8_t green)
{
	lights[numberled].green = green;
}

void setLEDblue(int numberled, uint8_t blue)
{
	lights[numberled].blue = blue;
}



void setup_isr_spi(void);

void setup_isr_spi(void)
{
#ifdef USESPI2
	enable_send_spi2 = 1;
	gpio_clear(GPIOB, GPIO12);
	spi_enable_tx_buffer_empty_interrupt(SPI2);
//	spi_write(SPI2, (uint16_t) ledbuffer[OFFSETSPI2 * NUMBERCOLLOR * NUMBERINCREASE]);
#endif
#ifdef USESPI3
	enable_send_spi3 = 1;
	gpio_clear(GPIOC, GPIO9);
	spi_enable_tx_buffer_empty_interrupt(SPI3);
	//spi_write(SPI3, (uint16_t) ledbuffer[OFFSETSPI3 * NUMBERCOLLOR * NUMBERINCREASE]);
#endif
}


void sendbuffer(void)
{
	incrementwaittimer += 1;


	if(incrementwaittimer > 500)
	{
		incrementwaittimer = 0;
#ifdef USEDMA
	#ifdef USESPI1
		setupdma(DMA2, DMA_STREAM3, DMA_SxCR_CHSEL_3 , SPI1_DR, OFFSETSPI1 , (uint16_t)NUMBERLEDSPI1);
	#endif
	#ifdef USESPI4
		setupdma(DMA2, DMA_STREAM4, DMA_SxCR_CHSEL_5 , SPI4_DR, OFFSETSPI4 , (uint16_t)NUMBERLEDSPI4);
	#endif
	#ifdef USESPI2
		setupdma(DMA1, DMA_STREAM4, DMA_SxCR_CHSEL_0, SPI2_DR, OFFSETSPI2, (uint16_t) NUMBERLEDSPI2);
	#endif
	#ifdef USESPI3
		setupdma(DMA1, DMA_STREAM5, DMA_SxCR_CHSEL_0, SPI3_DR, OFFSETSPI3, (uint16_t) NUMBERLEDSPI3);
	#endif
#else
		setup_isr_spi();
#endif
		
		/*for(i = 0; i < NUMBERLEDSPI4*NUMBERCOLLOR * NUMBERINCREASE; i++)
		  {
		  spi_send(SPI4, ledbuffer[i+8]);
		  }*/
#endif
		/*int i = 0;
		  for(i = 0; i < NUMBERLED*NUMBERCOLLOR * NUMBERINCREASE; i++)
		  {
		  spi_send(SPI1, ledbuffer[i]);
		  }*/
	}
}
#if defined(SK2812LED) || defined(SK2812SPI)
void setLEDrgbhsi(int numberled, uint8_t red, uint8_t green, uint8_t blue)
{
	float hue, saturation, intensity;
	rgbtohsi(red, green, blue, &hue, &saturation, &intensity);
	hsitorgb(hue, saturation, intensity,&(lights[numberled].red),&(lights[numberled].green), &(lights[numberled].blue), &(lights[numberled].white));
}

void setLEDwhite(int numberled, uint8_t white)
{
	lights[numberled].white = white;
}

void rgbtohsi(uint8_t red, uint8_t green, uint8_t blue,float * hue, float *saturation, float *intensity)
{
	float max, min, I, H, S, r, g, b, c;
	// normalice the rgb values
	r = red / 255.0;
	g = green / 255.0;
	b = blue / 255.0;
	// Calculate C
	max = MAX(r, MAX(g,b));
	min = MIN(r, MIN(g,b));
	c = max-min;
	// Calculate Intensity
	I = (r + g + b) / 3.0;
	//Caculate Hue and Saturation
	if(c == 0)
	{
		H = 0.0;
		S = 0.0;
	}
	else
	{
		if(r == max)
		{
			H = 60 * fmod(((g-b)/c)+6,6);
		}
		if(g == max)
		{
			H = 60 * (((b-r)/c)+2);
		}
		if(b == max)
		{
			H = 60 * (((r-g)/c)+4);
		}

		S = 1 - (min / I);
	}
	*hue = H;
	*saturation = S;
	*intensity = I;
}
void hsitorgb(float hue, float saturation, float intensity, uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white)
{
	uint8_t r, g, b, w;
	float cos_h, cos_1047_h;

	hue = fmod(hue,360);
	hue = 3.14159 * hue/(float)180;
	//saturation = saturation>0?(saturation<1?saturation:1):0;
	//intensity = intensity>0?(intensity<1?intensity:1):0;

	if(hue < 2.09439){
		cos_h = cos(hue);
		cos_1047_h = cos(1.047196667 - hue);
		r = saturation * 255 * intensity / 3.0*(1+cos_h/cos_1047_h);
		g = saturation * 255 * intensity / 3.0*(1+(1-cos_h/cos_1047_h));
		b = 0;
		w = 255 * intensity/3*(1 - saturation);
	}else if( hue < 4.188787){
		hue = hue - 2.09439;
		cos_h = cos(hue);
		cos_1047_h = cos(1.047196667 - hue);
		r = 0;
		g = saturation * 255 * intensity / 3.0*(1+cos_h/cos_1047_h);
		b = saturation * 255 * intensity / 3.0*(1+(1-cos_h/cos_1047_h));
		w = 255 * intensity/3*(1 - saturation);
	}else{
		hue = hue - 4.188787;
		cos_h = cos(hue);
		cos_1047_h = cos(1.047196667 - hue);
		r = saturation * 255 * intensity / 3.0*(1+(1-cos_h/cos_1047_h));
		g = 0;
		b = saturation * 255 * intensity / 3.0*(1+cos_h/cos_1047_h);
		w = 255 * intensity/3*(1 - saturation);
	}
	*red = (uint8_t)r;
	*green = (uint8_t)g;
	*blue = (uint8_t)b;
	*white = (uint8_t)w;
}

void testfunction(float hue, float saturation, float intensity, uint8_t *red, uint8_t *green, uint8_t *blue, uint8_t *white)
{
	float H, Z, C, X, M;


	M = (intensity * (1-saturation));
	H = hue / 60.0;
	Z = fmod(H,2)-1;
	if(Z <= 0)
		Z = 1- Z;
	else
		Z = 1 + Z;
	C = (3*intensity*saturation)/(1+Z);
	X = C * Z;

	if(H == 0)
	{
		*red =(uint8_t) 0;
		*green = (uint8_t) 0;
		*blue = (uint8_t) 0;
	}
	if((0<H)&&(H<=1)){
		*red =(uint8_t) 255.0* (C+M) ;
		*green = (uint8_t) 255.0* (X+M);
		*blue = (uint8_t) 0;
	}
	if((1<H)&&(H<=2)){
		*red =(uint8_t)255.0* (X+M);
		*green = (uint8_t)255.0* (C+M);
		*blue = (uint8_t) 0;
	}
	if((2<H)&&(H<=3)){
		*red =(uint8_t) 0;
		*green = (uint8_t)255.0* (C+M);
		*blue = (uint8_t)255.0* (X+M);
	}
	if((3<H)&&(H<=4)){
		*red =(uint8_t) 0;
		*green = (uint8_t)255.0* (X+M);
		*blue = (uint8_t)255.0* (C+M);
	}
	if((4<H)&&(H<=5)){
		*red =(uint8_t)255.0* (X+M);
		*green = (uint8_t) 0;
		*blue = (uint8_t)255.0* (C+M);
	}
	if((5<H)&&(H<=6)){
		*red =(uint8_t)255.0* (C+M);
		*green = (uint8_t) 0;
		*blue = (uint8_t)255.0* (X+M);
	}

	*white =(uint8_t) 255.0*M;
}
#endif

//SPI1
void dma2_stream3_isr(void)
{
	if(dma_get_interrupt_flag(DMA2, DMA_STREAM3, DMA_TCIF))
	{
		dma_clear_interrupt_flags(DMA2, DMA_STREAM3, DMA_TCIF);
		gpio_clear(GPIOA, GPIO4);
	}
}

//SPI3
void dma1_stream5_isr(void)
{
	if(dma_get_interrupt_flag(DMA1, DMA_STREAM5, DMA_TCIF))
	{
		dma_clear_interrupt_flags(DMA1, DMA_STREAM5, DMA_TCIF);
		gpio_clear(GPIOC, GPIO9);
	}
}

//SPI2
void dma1_stream4_isr(void)
{
	if(dma_get_interrupt_flag(DMA1, DMA_STREAM4, DMA_TCIF))
	{
		dma_clear_interrupt_flags(DMA1, DMA_STREAM4, DMA_TCIF);
		gpio_set(GPIOB, GPIO12);
	}
	else
	{
		dma_clear_interrupt_flags(DMA1, DMA_STREAM4, DMA_TCIF | DMA_HTIF | DMA_TEIF );
	}
}

//SPI2 isr
#ifndef USEDMA
#ifdef USESPI2
void spi2_isr(void)
{
	static int i = 0;
	if (enable_send_spi2 != 0)
	{
		i = 0;
		enable_send_spi2 = 0;
	}
	if( i <= NUMBERLEDSPI2 * NUMBERCOLLOR * NUMBERINCREASE)
	{
		spi_write(SPI2, (uint16_t) ledbuffer[ OFFSETSPI2 * NUMBERCOLLOR * NUMBERINCREASE + i]);
		i++;
	}
	else
	{
		spi_disable_tx_buffer_empty_interrupt(SPI2);
		gpio_set(GPIOB, GPIO12);
		spi_write(SPI2, 0);
	}
}
#endif
#endif


//SPI3 isr
#ifndef USEDMA
#ifdef USESPI3
void spi3_isr(void)
{
	static int i = 0;
	if (enable_send_spi3 != 0)
	{
		i = 0;
		enable_send_spi3 = 0;
	}
	if( i <= NUMBERLEDSPI3 * NUMBERCOLLOR * NUMBERINCREASE)
	{
		spi_write(SPI3, (uint16_t) ledbuffer[ OFFSETSPI3 * NUMBERINCREASE * NUMBERCOLLOR + i]);
		i++;
	}
	else
	{
		spi_disable_tx_buffer_empty_interrupt(SPI3);
		gpio_set(GPIOC, GPIO9);
		spi_write(SPI3, 0);
	}
}
#endif
#endif
