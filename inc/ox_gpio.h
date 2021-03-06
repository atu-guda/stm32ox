#ifndef _OX_GPIO_H
#define _OX_GPIO_H

#include <ox_dev.h>

#if defined STM32F1
 #include <stm32f10x_gpio.h>
#elif defined STM32F2
 #include <stm32f2xx_gpio.h>
#elif defined STM32F3
 #include <stm32f30x_gpio.h>
#elif defined STM32F4
 #include <stm32f4xx_gpio.h>
#else
 #error "Undefined MC type"
#endif // STM32Fxxx



struct GPIO_Info {
  GPIO_TypeDef *gpio;
  uint32_t rcc_bits;
  uint16_t rcc_reg;
};

extern const GPIO_Info gpio_a;
extern const GPIO_Info gpio_b;
extern const GPIO_Info gpio_c;
extern const GPIO_Info gpio_d;
extern const GPIO_Info gpio_e;

class PinsOut
{
  public:
   PinsOut( const GPIO_Info *gi, uint8_t a_start, uint8_t a_n )
     : gpio_info( gi ),
       gpio( gi->gpio ),
       start( a_start ), n( a_n ),
       mask( ((uint16_t)(0xFFFF) << (PORT_BITS - n)) >> (PORT_BITS - n - start) )
     {};
   uint16_t getMask() const { return mask; }
   void initHW();
   const GPIO_TypeDef* dev() { return gpio; }
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
     gpio->SET_BIT_REG = mv( v );
   }
   void reset( uint16_t v ) // AND~
   {
     gpio->RESET_BIT_REG = mv( v );
   }
   void toggle( uint16_t v ) // XOR
   {
     gpio->ODR ^= mv( v );
   }
   const GPIO_Info* get_gpio_info() const  { return gpio_info; }
  protected:
   const GPIO_Info *gpio_info;
   GPIO_TypeDef *const gpio;
   const uint8_t start, n;
   const uint16_t mask;
};
extern PinsOut leds;
void die4led( uint16_t n );


#endif

// vim: path=.,/usr/share/stm32lib/inc
