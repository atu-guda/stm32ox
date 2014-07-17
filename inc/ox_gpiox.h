#ifndef _OX_GPIOX_H
#define _OX_GPIOX_H

// #ifdef STM32F1
// #endif


#if defined STM32F1
 #include <stm32f10x.h>
 #include <stm32f10x_gpio.h>
#elif defined STMF2
 #include <stm32f20x.h>
 #include <stm32f20x_gpio.h>
#elif defined STMF3
 #include <stm32f30x.h>
 #include <stm32f30x_gpio.h>
#elif defined STMF4
 #include <stm32f40x.h>
 #include <stm32f40x_gpio.h>
#else
 #error "Undefined MC type"
#endif // STM32Fxxx

#include <ox_base.h>



#ifdef STM32F1

// GPIO is a special devices: not fixed pin roles

struct DevGPIOA {
  enum {
    base = GPIOA_BASE,
    rcc_reg  = RCC_APB2,
    rcc_bit  = RCC_APB2ENR_IOPAEN
  };
};

struct DevGPIOB {
  enum {
    base = GPIOB_BASE,
    rcc_reg  = RCC_APB2,
    rcc_bit  = RCC_APB2ENR_IOPBEN
  };
};

struct DevGPIOC {
  enum {
    base = GPIOC_BASE,
    rcc_reg  = RCC_APB2,
    rcc_bit  = RCC_APB2ENR_IOPCEN
  };
};

struct DevGPIOD {
  enum {
    base = GPIOD_BASE,
    rcc_reg  = RCC_APB2,
    rcc_bit  = RCC_APB2ENR_IOPDEN
  };
};

struct DevGPIOE {
  enum {
    base = GPIOE_BASE,
    rcc_reg  = RCC_APB2,
    rcc_bit  = RCC_APB2ENR_IOPEEN
  };
};

#define DEFINED_PinsOutX
template<typename DevGPIO>
class PinsOutX
{
  public:
   PinsOutX( uint8_t a_start, uint8_t a_n )
     : gpio( (GPIO_TypeDef*)(DevGPIO::base)),
       start( a_start ), n( a_n ),
       mask( ((uint16_t)(0xFFFF) << (PORT_BITS - n)) >> (PORT_BITS - n - start) )
     {};
   uint16_t getMask() const { return mask; }
   void initHW()
   {
      if( RCC_enr[ DevGPIO::rcc_reg ] != nullptr ) {
       *( RCC_enr[ DevGPIO::rcc_reg ] ) |= DevGPIO::rcc_bit;
      }
     devPinsConf( gpio, pinMode_Out_PP, mask );
   };
   GPIO_TypeDef* dev() { return gpio; }
   uint16_t mv( uint16_t v ) const
   {
     return ((v << start) & mask );
   }
   void write( uint16_t v )  // set to given, drop old
   {
     gpio->ODR = mv( v ) | ( gpio->ODR & (~mask) );
   }
   void set( uint16_t v )   // get given to '1' (OR)
   {
     gpio->BSRR = mv( v );
   }
   void reset( uint16_t v ) // AND~
   {
     gpio->BRR = mv( v );
   }
   void toggle( uint16_t v ) // XOR
   {
     gpio->ODR ^= mv( v );
   }
  protected:
   GPIO_TypeDef *const gpio;
   const uint8_t start, n;
   const uint16_t mask;
};

#endif // STM32F1
// TODO: F2-4
//

extern PinsOutX<DevGPIOC> leds; // BUG: TODO: how to define not binding to DevGPIOC?


// template<typename DevConf, typename DevMode>
// void DevBase<DevConf,DevMode>::initHW()
// {
//
//   int i = 0;
//   for( auto m : DevMode::pins ) {
//     if( m ) {
//       devPinConf( DevConf::pins[i].port, (GPIOMode_TypeDef)(m), DevConf::pins[i].pin );
//     }
//     ++i;
//   }
//
// }

#endif

// vim: path=.,/usr/share/stm32lib/inc
