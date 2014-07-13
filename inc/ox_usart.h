#ifndef _OX_USART_H
#define _OX_USART_H

// #ifdef STM32F1
// #endif

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>

#include "ox_common.h"

enum UsartModes {
  modeNone = 0,
  modeAsync,
  modeHWFC,
  modeHalfDuplex,
  modeIRDA,
  modeLin,
  modeSmartCard,
  modeSync
};


struct UsartModeBase {
  enum {
    Pin_TX = 0, Pin_RX, Pin_CTS, Pin_RTS, Pin_CK, Pin_NUM
  };
  enum { Req_n = 0 };
  const static int pins[Pin_NUM];
};
const int UsartModeBase::pins[Pin_NUM] = {
    0, // TX
    0, // RX
    0, // CK
    0, // CTS
    0  // RCK
};
// debug on PC host only
const char *const usart_pin_name[] = { "TX", "RX", "CK", "CTS", "RTS",  "?NUM", "??", "???" };


struct UsartMode2Wire : public UsartModeBase {
  enum { Req_n = 2 };
  const static int pins[Pin_NUM];
};
const int UsartMode2Wire::pins[Pin_NUM] = {
    GPIO_Mode_AF_PP, // TX
    GPIO_Mode_IPU,   // RX
    0,               // CK
    0,               // CTS
    0                // RTS
};


struct UsartMode4Wire : public UsartModeBase {
  enum { Req_n = 4 };
  const static int pins[Pin_NUM];
};
const int UsartMode4Wire::pins[] = {
    GPIO_Mode_AF_PP, // TX
    GPIO_Mode_IPU,   // RX
    0,               // CK
    GPIO_Mode_IPU,   // CTS
    GPIO_Mode_AF_PP  // RTS
};


struct UsartModeAsync : public UsartMode2Wire {
  enum {
    mode = modeAsync
  };
};

struct UsartModeHWFC : public UsartMode4Wire {
  enum {
    mode = modeHWFC
  };
};

struct UsartModeHalfDuplex : public UsartModeBase {
  enum { Req_n = 1 };
  enum {
    mode = modeHalfDuplex
  };
  const static int pins[Pin_NUM];
};
const int UsartModeHalfDuplex::pins[] = {
    GPIO_Mode_AF_PP, // TX
    0,               // RX
    0,               // CK
    0,               // CTS
    0                // RTS
};

struct UsartModeIRDA : public UsartModeAsync {
  enum {
    mode = modeIRDA
  };
};

struct UsartModeLin : public UsartModeAsync {
  enum {
    mode = modeLin
  };
};

struct UsartModeSmartCard : public UsartModeHalfDuplex {
  enum {
    mode = modeSmartCard
  };
};

struct UsartModeSyn : public UsartModeBase {
  enum {
    mode = modeSync
  };
  enum { Req_n = 3 };
  const static int pins[Pin_NUM];
};
const int UsartModeSyn::pins[] = {
    GPIO_Mode_AF_PP, // TX
    GPIO_Mode_IPU,   // RX
    GPIO_Mode_AF_PP, // CK
    0,               // CTS
    0                // RTS
};


template<typename UsartConf, typename UsartMode>
class Usart : public DevBase<UsartConf,UsartMode> {
  public:
   Usart( uint32_t baud )
     : baudRate(baud),
       usart( (USART_TypeDef*)(UsartConf::base))
    {
      static_assert( UsartConf::canMode( UsartMode::Req_n ), "Unsupported USART mode" );
    }
   void init() {};
   void deInit() {};
   void initHW();
   USART_TypeDef* getDev() { return usart; };
   void send( uint16_t v ) { USART_SendData( usart, v); };
   int16_t recv() { return USART_ReceiveData( usart ); };
  protected:
   uint32_t baudRate;
   uint16_t wordLength = USART_WordLength_8b;
   uint16_t stopBits = USART_StopBits_1;
   uint16_t parity = USART_Parity_No;
   uint16_t mode =  USART_Mode_Rx | USART_Mode_Tx; // TODO: to config
   USART_TypeDef *usart;
};

template<typename UsartConf, typename UsartMode>
void Usart<UsartConf, UsartMode>::initHW()
{
  // std::cout << "USART mode " << UsartMode::mode << std::endl;
  DevBase<UsartConf, UsartMode>::initHW();

}


#endif
