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
  &(RCC->APB1ENR),
  &(RCC->APB2ENR),
  &(RCC->AHB1ENR),
  &(RCC->AHB2ENR),
  &(RCC->AHB3ENR)
};
#elif defined(STM32F3)
reg32 *const RCC_enr[RCC_Bus::RCC_NBUS] {
  &(RCC->APB1ENR),
  &(RCC->APB2ENR),
  &(RCC->AHBENR),
  nullptr,
  nullptr
};
#elif defined(STM32F4)
reg32 *const RCC_enr[RCC_Bus::RCC_NBUS] {
  &(RCC->APB1ENR),
  &(RCC->APB2ENR),
  &(RCC->AHB1ENR),
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

  // remap
  #ifdef STM32F1
  if( cfg->remap ) {
    AFIO->MAPR |= cfg->remap;
  }
  #else
  uint8_t afn = (uint8_t)(cfg->remap);
  #endif

  // pins config
  for( int i=0; i<mode->pin_num; ++i ) {
    if( mode->pins[i] != pinMode_NONE
        && cfg->pins[i].port != nullptr
        && cfg->pins[i].pin != 0
    ) {
      devPinsConf( cfg->pins[i].port, mode->pins[i], cfg->pins[i].pin );
      #ifndef STM32F1
      if( afn ) {
        uint8_t np = numFirstBit( cfg->pins[i].pin );
        if( np < PORT_BITS ) {
          GPIO_PinAFConfig( cfg->pins[i].port, np, afn );
        }
      }
      #endif
    }
  }
}

void DevBase::initIRQ( uint8_t prio, uint8_t n  )
{
  NVIC_InitTypeDef nv;
  uint8_t ch = ( n == 1 ) ? cfg->irq_num1 : cfg->irq_num0;
  if( ch == 0 ) {
    return;
  }
  nv.NVIC_IRQChannel = ch;
  nv.NVIC_IRQChannelPreemptionPriority = prio;
  nv.NVIC_IRQChannelSubPriority = 0;
  nv.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &nv );
}

// ----------------------- PinHold --------------------------------

PinHold::PinHold( PinPlace *a_pin, bool val, bool a_skip )
  : p( a_pin ), v( val ), skip( a_skip )
{
  if( !skip ) {
    if( v ) {
      pin_set( p );
    } else {
      pin_reset( p );
    }
  }
}

PinHold::~PinHold()
{
  if( !skip ) {
    if( !v ) {
      pin_reset( p );
    } else {
      pin_set( p );
    }
  }
}
