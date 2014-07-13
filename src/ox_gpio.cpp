#include <ox_gpio.h>

#ifdef STM32F1


const GPIO_Info gpio_a { GPIOA, RCC_APB2ENR_IOPAEN, RCC_APB2 };
const GPIO_Info gpio_b { GPIOB, RCC_APB2ENR_IOPBEN, RCC_APB2 };
const GPIO_Info gpio_c { GPIOC, RCC_APB2ENR_IOPCEN, RCC_APB2 };
const GPIO_Info gpio_d { GPIOD, RCC_APB2ENR_IOPDEN, RCC_APB2 };
const GPIO_Info gpio_e { GPIOE, RCC_APB2ENR_IOPEEN, RCC_APB2 };

#endif // STM32F1 ... x

void PinsOut::initHW()
{
  if( RCC_enr[ gpio_info->rcc_reg ] != nullptr ) {
   *( RCC_enr[ gpio_info->rcc_reg ] ) |= gpio_info->rcc_bits;
  }
  devPinConf( gpio, GPIO_Mode_Out_PP, mask );
};

// vim: path=.,/usr/share/stm32lib/inc
