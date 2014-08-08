#include "stm32f10x_conf.h"
#include <errno.h>

#ifdef USE_FREERTOS
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <stdlib.h>
#endif

#include <ox_dev.h>




#if defined(STM32F1)
reg32 *const RCC_enr[RCC_Bus::RCC_NBUS] {
  &(RCC->APB1ENR),
  &(RCC->APB2ENR),
  &(RCC->AHBENR),
  nullptr,
  nullptr
};
#elif defined(STM32F2)
reg32 *const RCC_enr[RCC_Bus::RCC_NBUS] {
  &(RCC->AHB1ENR),
  &(RCC->APB1ENR),
  &(RCC->APB2ENR),
  &(RCC->AHB2ENR),
  &(RCC->AHB3ENR)
};
#elif defined(STM32F3)
reg32 *const RCC_enr[RCC_Bus::RCC_NBUS] {
  &(RCC->AHBENR),
  &(RCC->APB1ENR),
  &(RCC->APB2ENR),
  nullptr,
  nullptr
};
#elif defined(STM32F4)
reg32 *const RCC_enr[RCC_Bus::RCC_NBUS] {
  &(RCC->AHB1ENR),
  &(RCC->APB1ENR),
  &(RCC->APB2ENR),
  &(RCC->AHB2ENR),
  &(RCC->AHB3ENR)
};

#else
 #error "Unsupported MCU detected ;-( "
#endif // STM32Fxxx



void DevBase::initHW()
{
  // RCC enable bits part
  for( int i=0; i<RCC_NBUS; ++i ) {
    if( ! RCC_enr[i] ) {
      continue;
    }
    if( cfg->rcc_bits[i] ) {
      *RCC_enr[i] |=  cfg->rcc_bits[i];
    }
  }

  // TODO: remap, irq

  // pins config
  for( int i=0; i<max_dev_pins; ++i ) {
    if( mode->pins[i] == pinMode_NONE
        || cfg->pins[i].port == nullptr
        || cfg->pins[i].pin == 0
    ) {
      continue;
    }
    devPinsConf( cfg->pins[i].port, mode->pins[i], cfg->pins[i].pin );
  }
}

