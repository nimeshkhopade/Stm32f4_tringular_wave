#include <stm32f4xx.h>

const uint16_t Sine12bit[128] = {
                                      2048, 2145, 2242, 2339, 2435, 2530, 2624, 2717, 2808, 2897,
                                      2984, 3069, 3151, 3230, 3307, 3381, 3451, 3518, 3581, 3640,
                                      3696, 3748, 3795, 3838, 3877, 3911, 3941, 3966, 3986, 4002,
                                      4013, 4019, 4020, 4016, 4008, 3995, 3977, 3954, 3926, 3894,
                                      3858, 3817, 3772, 3722, 3669, 3611, 3550, 3485, 3416, 3344,
                                      3269, 3191, 3110, 3027, 2941, 2853, 2763, 2671, 2578, 2483,
                                      2387, 2291, 2194, 2096, 1999, 1901, 1804, 1708, 1612, 1517,
                                      1424, 1332, 1242, 1154, 1068, 985, 904, 826, 751, 679,
                                      610, 545, 484, 426, 373, 323, 278, 237, 201, 169,
                                      141, 118, 100, 87, 79, 75, 76, 82, 93, 109,
                                      129, 154, 184, 218, 257, 300, 347, 399, 455, 514,
                                      577, 644, 714, 788, 865, 944, 1026, 1111, 1198, 1287,
                                      1378, 1471, 1565, 1660, 1756, 1853, 1950, 2047 
};  

void delay(){
	int j = 0, i = 0;
	for(i = 0;i < 2500; i++)
	{
		for(j = 0; j < 500; j++){}
	}
}

void board_init() {
		
	/* Flash settings (see RM0090 rev9, p80) */

	FLASH->ACR =
            FLASH_ACR_LATENCY_5WS               // 6 CPU cycle wait 
          | FLASH_ACR_PRFTEN                    // enable prefetch 
          | FLASH_ACR_ICEN                      // instruction cache enable *
          | FLASH_ACR_DCEN;                     // data cache enable 
	
	
	/********* CLOCK GENERATION **********/
	
	RCC->CFGR = RCC_CFGR_PPRE2_DIV2           // APB2 prescaler 
								| RCC_CFGR_PPRE1_DIV4;          // APB1 prescaler 
				
	
	RCC->CR = RCC_CR_HSEON;         /* Enable external oscillator */

	/* Wait for locked external oscillator */
	while((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY);
	
	/* PLL config */
	RCC->PLLCFGR =
          RCC_PLLCFGR_PLLSRC_HSE                /* PLL source */
        | (4 << 0)                              /* PLL input division */
        | (168 << 6)                            /* PLL multiplication */
        | (0 << 16)                             /* PLL sys clock division */
        | (7 << 24);                            /* PLL usb clock division =48MHz */
	
	RCC->CR |= RCC_CR_PLLON;		/* Enable PLL */

	while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);	/* Wait for locked PLL */
	
	RCC->CFGR &= ~RCC_CFGR_SW; 		/* select system clock */			/* clear */
	
	RCC->CFGR |= RCC_CFGR_SW_PLL | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2;   /* SYSCLK is PLL */
	
	while((RCC->CFGR & RCC_CFGR_SW_PLL) != RCC_CFGR_SW_PLL);		/* Wait for SYSCLK to be PLL */
	/************ /CLOCK GENERATION ************/

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOAEN ; //Enable GPIO A&B Clock
	
	RCC->APB1ENR = 1 << 29; //DAC ON
	
	//RCC->APB2ENR = 1 << 8; //ADC1ON
	
	//GPIOB->AFR[0] |= 4 << 24; //Configure PB6 to alternate function -> I2C(SCL)
	
	//GPIOB->AFR[1] |= 4 << 4; //Configure PB9 to alternate function -> I2C(SDA)
	
	//GPIOB->MODER |= 2 << 12 | 2 << 18; //PB6 | PB9
	
	GPIOA->MODER |= 3 << 8 | 3 << 10; //PA4&PA5 Analog mode
	
	GPIOA->OSPEEDR |= 3 << 8 | 3 << 10; // Very High Speed
	
	//GPIOB->OTYPER = 1 << 6 | 1 << 9; // Open Drain
	
	//GPIOB->OSPEEDR |= 1 << 12 | 1 << 18; // Medium Speed
	
	//GPIOB->PUPDR |= 1 << 12 | 1 << 18; // PULLUP VERY IMPORTANT
	
	/*************** I2C Config *******************/
	
/*	I2C1->CR2 |= 16 << 0; //16MHz peri clk
	
	I2C1->CCR |= 0x0050; // for 100KHz SM mode
	
	I2C1->TRISE = 0x0011; //17 (1000ns/62.5ns = 16 + 1)
	
	I2C1->CR1 |= 1 << 0; // Enable peripheral */
	
	/************ //I2C config ******************/
	
	/************** DAC Config *****************/
	
	DAC->CR = 1 << 0 | 1 << 2 | 7 << 3 | 2 << 6; //EN,TEN1,TSEL1,WAVE1 
}
int main (void) {
	int count = 0, i = 0;
	const uint32_t sample[10] = {2048,3251,3995,3996,3253,2051,847,101,98,839};
	board_init();
	while(1) {
		for(i = 0; i < 128; i++) {
			DAC->SWTRIGR = 1 << 0;
			DAC->DHR12R1 = Sine12bit[i];
		}
		//DAC->CR |= count << 8;
		//count++;
		
		/*if(count >= 50) {
			count = 0;
		}*/
		//delay();
	}
}
