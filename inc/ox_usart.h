#ifndef _OX_USART_H
#define _OX_USART_H

// #ifdef STM32F1
// #endif

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>

#include <ox_dev.h>

extern DevConfig USART1Conf1;
extern DevConfig USART1Conf2;
extern DevConfig USART2Conf1;
extern DevConfig USART2Conf2;
extern DevConfig USART3Conf1;
extern DevConfig USART3Conf2;
extern DevConfig UART4Conf1;
extern DevConfig UART5Conf2;


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

extern DevMode UsartModeAsync;
extern DevMode UsartModeHWFC;
//extern DevMode UsartModeHalfDuplex;
//extern DevMode UsartModeIRDA;
//extern DevMode UsartModeSmartCard;
//extern DevMode UsartModeLin;
extern DevMode UsartModeSync;



class Usart : public DevBase {
  public:
   enum {
     CR1_UE  = 0x2000,  //* Enable
     CR1_SBK = 0x0001   //* Break Character
   };
   Usart( const DevConfig *dcfg, const DevMode *dmode, uint32_t baud )
     : DevBase( dcfg, dmode ),
       usart( (USART_TypeDef*)(dcfg->base)),
       uitd{ baud, USART_WordLength_8b, USART_StopBits_1,
             USART_Parity_No, USART_Mode_Rx | USART_Mode_Tx,
             USART_HardwareFlowControl_None }
    {
    }
   void init();
   void deInit();
   void initHW();
   void enable()  { usart->CR1 |= CR1_UE; };
   void disable() { usart->CR1 &= ~CR1_UE; };
   void itConfig( uint16_t it, FunctionalState en );
   USART_TypeDef* getDev() { return usart; };
   USART_InitTypeDef* getInitStruct() { return &uitd; }
   void sendRaw( uint16_t v ) { usart->DR = ( v & (uint16_t)0x01FF); };
   int16_t recvRaw() { return (uint16_t)( usart->DR & (uint16_t)0x01FF ); };
   void sendBrk() { usart->CR1 |= CR1_SBK; };
   bool checkFlag( uint16_t flg );
   void clearFlag( uint16_t flg );
   uint16_t getSR() { return usart->SR; }
   uint16_t getCR1() { return usart->CR1; }
   ITStatus getITStatus( uint16_t it );
   void clearITPendingBit( uint16_t it );

   int sendStrLoop( const char* s );
   int sendBlockLoop( const uint8_t* d, int n );

  protected:
   USART_TypeDef *usart;
   USART_InitTypeDef uitd;
};


#endif
// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include
