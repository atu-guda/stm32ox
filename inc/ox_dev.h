#ifndef _OX_DEV_H
#define _OX_DEV_H

#include <ox_base.h>

extern reg32 *const RCC_enr[RCC_Bus::RCC_NBUS];

const int max_dev_pins = 8; // May be more? (ETH)

//* Single pin description
struct PinPlace
{
  GPIO_TypeDef* port; //* Port: like GPIOA
  uint16_t pin;       //* Pin Bit.
};

struct DevConfig {
  uint32_t base;               //* Base address: real use in real periph
  uint32_t rcc_bits[RCC_NBUS]; //* Pits to set in RCC_enr to enable device, pins, remap. TODO: + reset bits
  uint32_t remap;              //* Remap flag: may be more of F4?
  PinPlace *pins;              //* Describes { port, pin } for given config
  uint8_t pin_num;             //* size of "pins" array
};


struct DevMode {
  uint32_t mode;   //* Abstract mode: may be used at real devices at will
  uint8_t n_req;   // TODO: remove
  uint8_t pin_num; // TODO: remove
  PinModeNum pins[max_dev_pins]; //* Required pin config: really index in GPIO_Modes[]
};



class DevBase {
  public:
   DevBase( const DevConfig *dcfg, const DevMode *dmode )
    : cfg(dcfg), mode(dmode) {};
   void initHW();
  protected:
   const DevConfig *cfg;
   const DevMode   *mode;
};

#endif

