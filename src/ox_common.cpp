#include "ox_common.h"

#ifdef STM32F1

// TODO: hide to something
reg32 *const RCC_enr[4] {
  &(RCC->AHBENR),
  &(RCC->APB1ENR),
  &(RCC->APB2ENR),
  0 // for F4...
};

const GPIO_Info gpio_a { GPIOA, RCC_APB2ENR_IOPAEN, RCC_APB2 };
const GPIO_Info gpio_b { GPIOB, RCC_APB2ENR_IOPBEN, RCC_APB2 };
const GPIO_Info gpio_c { GPIOC, RCC_APB2ENR_IOPCEN, RCC_APB2 };
const GPIO_Info gpio_d { GPIOD, RCC_APB2ENR_IOPDEN, RCC_APB2 };
const GPIO_Info gpio_e { GPIOE, RCC_APB2ENR_IOPEEN, RCC_APB2 };

// TODO: separate F1/F*
void devPinConf( GPIO_TypeDef* GPIOx, GPIOMode_TypeDef mode, uint16_t pins )
{
  GPIO_InitTypeDef gp;
  gp.GPIO_Pin = pins;
  gp.GPIO_Speed = GPIO_Speed_50MHz; // TODO: config
  gp.GPIO_Mode = mode;
  GPIO_Init( GPIOx, &gp );
}


#else
 #error "Only STM32F1xx supported now ;-( "

void devPinConf( GPIO_TypeDef* GPIOx, GPIOMode_TypeDef mode, uint16_t pins )
{
  GPIO_InitTypeDef gp;
  gp.GPIO_Pin = pins;
  GPIO_Init( GPIOx, &gp );
}

#endif // STM32F1 ... x

void PinsOut::initHW()
{
  if( RCC_enr[ gpio_info->rcc_reg ] != nullptr ) {
   *( RCC_enr[ gpio_info->rcc_reg ] ) |= gpio_info->rcc_bits;
  }
  devPinConf( gpio, GPIO_Mode_Out_PP, mask );
};

// vim: path=.,/usr/share/stm32lib/inc
