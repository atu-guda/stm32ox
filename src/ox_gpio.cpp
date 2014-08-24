#include <ox_gpio.h>

#ifdef STM32F1

const GPIO_Info gpio_a { GPIOA, RCC_APB2ENR_IOPAEN, RCC_APB2 };
const GPIO_Info gpio_b { GPIOB, RCC_APB2ENR_IOPBEN, RCC_APB2 };
const GPIO_Info gpio_c { GPIOC, RCC_APB2ENR_IOPCEN, RCC_APB2 };
const GPIO_Info gpio_d { GPIOD, RCC_APB2ENR_IOPDEN, RCC_APB2 };
const GPIO_Info gpio_e { GPIOE, RCC_APB2ENR_IOPEEN, RCC_APB2 };
const GPIO_Info gpio_f { GPIOF, RCC_APB2ENR_IOPFEN, RCC_APB2 };

#endif // STM32F1 ... x

#ifdef STM32F4

const GPIO_Info gpio_a { GPIOA, RCC_AHB1ENR_GPIOAEN, RCC_AHB1 };
const GPIO_Info gpio_b { GPIOB, RCC_AHB1ENR_GPIOBEN, RCC_AHB1 };
const GPIO_Info gpio_c { GPIOC, RCC_AHB1ENR_GPIOCEN, RCC_AHB1 };
const GPIO_Info gpio_d { GPIOD, RCC_AHB1ENR_GPIODEN, RCC_AHB1 };
const GPIO_Info gpio_e { GPIOE, RCC_AHB1ENR_GPIOEEN, RCC_AHB1 };
const GPIO_Info gpio_f { GPIOF, RCC_AHB1ENR_GPIOFEN, RCC_AHB1 };
const GPIO_Info gpio_g { GPIOG, RCC_AHB1ENR_GPIOGEN, RCC_AHB1 };

#endif // STM32F1 ... x

void PinsOut::initHW()
{
  if( RCC_enr[ gpio_info->rcc_reg ] != nullptr ) {
   *( RCC_enr[ gpio_info->rcc_reg ] ) |= gpio_info->rcc_bits;
  }
  devPinsConf( gpio, pinMode_Out_PP, mask );
};

// vim: path=.,/usr/share/stm32lib/inc
