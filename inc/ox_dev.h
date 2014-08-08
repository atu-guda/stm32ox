#ifndef _OX_DEV_H
#define _OX_DEV_H

#include <ox_base.h>

extern reg32 *const RCC_enr[RCC_Bus::RCC_NBUS];

const int max_dev_pins = 8; // May be more? (ETH)

struct PinPlace
{
  GPIO_TypeDef* port;
  uint16_t pin;
};

struct DevConfig {
  uint32_t base;
  uint32_t rcc_bits[RCC_NBUS];
  uint32_t remap;
  PinPlace pins[max_dev_pins];
};


struct DevMode {
  uint32_t mode;
  uint8_t n_req;
  uint8_t pin_num;
  PinModeNum pins[max_dev_pins];
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

