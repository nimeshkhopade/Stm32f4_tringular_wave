#include <stm32f4xx.h>

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

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN ; //Enable GPIO A&B Clock
	
	RCC->APB1ENR = 1 << 29; //DAC ON
	
	GPIOA->MODER |= 3 << 8 | 3 << 10; //PA4&PA5 Analog mode
	
	GPIOA->OSPEEDR |= 3 << 8 | 3 << 10; // Very High Speed
	
	/************** DAC Config *****************/
	
	DAC->CR = 1 << 0 | 1 << 2 | 7 << 3 | 2 << 6; //EN,TEN1,TSEL1,WAVE1 
}
int main (void) {
	int count = 0, i = 0;
	board_init();
	while(1) {
		DAC->SWTRIGR = 1 << 0;
		DAC->CR |= count << 8;
		count++;
	}
}
