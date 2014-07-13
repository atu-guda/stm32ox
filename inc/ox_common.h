#ifndef _OX_COMMON_H
#define _OX_COMMON_H

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

using reg32 = __IO uint32_t;

constexpr uint8_t PORT_BITS = 16;

// RCC registers for enable devices
constexpr int RCC_BUS_N = 4;
extern reg32 *const RCC_enr[RCC_BUS_N];

void devPinConf( GPIO_TypeDef* GPIOx, GPIOMode_TypeDef mode, uint16_t pins );

#ifdef STM32F1

enum RCC_Bus { // indexes in RCC_enr
  RCC_Bus0 = 0, RCC_AHB  = 0,
  RCC_Bus1 = 1, RCC_APB1 = 1,
  RCC_Bus2 = 2, RCC_APB2 = 2,
  RCC_Bus3 = 3
};

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
   const GPIO_Info* get_gpio_info() const  { return gpio_info; }
  protected:
   const GPIO_Info *gpio_info;
   GPIO_TypeDef *const gpio;
   const uint8_t start, n;
   const uint16_t mask;
};

template<typename DevGPIO>
class OutPins
{
  public:
   OutPins( uint8_t a_start, uint8_t a_n )
     : gpio( (GPIO_TypeDef*)(DevGPIO::base)),
       start( a_start ), n( a_n ),
       mask( ((uint16_t)(0xFFFF) << (PORT_BITS - n)) >> (PORT_BITS - n - start) )
     {};
   uint16_t getMask() const { return mask; }
   void initHW()
   {
      if( RCC_enr[ DevGPIO::rcc_reg ] != nullptr ) {
       *( RCC_enr[ DevGPIO::rcc_reg ] ) |= DevGPIO::rcc_bits;
      }
     devPinConf( gpio, GPIO_Mode_Out_PP, mask );
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



struct PinPlace
{
  GPIO_TypeDef* port;
  uint16_t pin;
};


template<typename DevConf, typename DevMode>
class DevBase {
  public:
   DevBase() {};
   void initHW();
};

template<typename DevConf, typename DevMode>
void DevBase<DevConf,DevMode>::initHW()
{

  // enable by RCC bits
  // std::cout << " RCC enable [" << 0 << "] " << std::hex << RCC_enr[ 0 ]
  //      << " |= " << DevConf::rcc_bits0 << std::endl;
  // std::cout << " RCC enable [" << 1 << "] " << std::hex << RCC_enr[ 1 ]
  //      << " |= " << DevConf::rcc_bits1 << std::endl;
  // std::cout << " RCC enable [" << 2 << "] " << std::hex << RCC_enr[ 2 ]
  //      << " |= " << DevConf::rcc_bits2 << std::endl;
  // std::cout << " RCC enable [" << 3 << "] " << std::hex << RCC_enr[ 3 ]
  //      << " |= " << DevConf::rcc_bits3 << std::endl;
  //* xxx |= bits; + check PTR

  int i = 0;
  for( auto m : DevMode::pins ) {
    if( m ) {
      // std::cout << " pin_" << i << " = " << m << ' '
      //      << std::hex << DevConf::pins[i].port << " : "
      //      << DevConf::pins[i].pin << std::endl;
      devPinConf( DevConf::pins[i].port, (GPIOMode_TypeDef)(m), DevConf::pins[i].pin );
    }
    ++i;
  }
  // std::cout << std::endl;

}


#endif

// vim: path=.,/usr/share/stm32lib/inc
