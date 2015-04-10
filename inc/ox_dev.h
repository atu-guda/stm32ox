#ifndef _OX_DEV_H
#define _OX_DEV_H

#include <ox_base.h>


extern reg32 *const RCC_enr[RCC_Bus::RCC_NBUS];

//* Single pin description
struct PinPlace
{
  GPIO_TypeDef* port; //* Port: like GPIOA
  uint16_t pin;       //* Pin Bit.
};

struct DevConfig {
  uint32_t base;               //* Base address: real use in real periph
  uint32_t rcc_bits[RCC_NBUS]; //* Bits to set in RCC_enr to enable device, pins, remap. TODO: + reset bits
  uint32_t remap;              //* Remap flag: AFn on F4
  PinPlace *pins;              //* Describes { port, pin } for given config
  uint8_t  pin_num;            //* size of "pins" array
  uint8_t  irq_num0;           //* NVIC main IRQ number
  uint8_t  irq_num1;           //* NVIC aux  IRQ number (error, ... )
};


struct DevMode {
  uint32_t mode;   //* Abstract mode: may be used at real devices at will
  PinModeNum *pins; //* Required pin config: really index in GPIO_Modes[]
  uint8_t pin_num;
};



class DevBase {
  public:
   DevBase( const DevConfig *dcfg, const DevMode *dmode )
    : cfg(dcfg), mode(dmode) {};
   void initHW();
   void initIRQ( uint8_t prio, uint8_t n = 0 );
  protected:
   const DevConfig *cfg;
   const DevMode   *mode;
};

inline void pin_set( PinPlace *p )
{
  p->port->SET_BIT_REG = p->pin;
}

inline void pin_reset( PinPlace *p )
{
  p->port->RESET_BIT_REG = p->pin;
}

//* AUX class to hold pin value while object lifetime, and revert on death
class PinHold {
  public:
   PinHold( PinPlace *a_pin, bool val, bool a_skip = false );
   ~PinHold();
  protected:
   PinPlace *p;
   bool v, skip;
};

#endif

