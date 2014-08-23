#ifndef _OX_USART_H
#define _OX_USART_H

#if defined STM32F1
 #include <stm32f10x_usart.h>
#elif defined STM32F2
 #include <stm32f2xx_usart.h>
#elif defined STM32F3
 #include <stm32f30x_usart.h>
#elif defined STM32F4
 #include <stm32f4xx_usart.h>
#else
 #error "Undefined MC type"
#endif // STM32Fxxx

#include <ox_dev.h>

extern DevConfig USART1Conf1;
extern DevConfig USART1Conf2;
extern DevConfig USART2Conf1;
extern DevConfig USART2Conf2;
extern DevConfig USART3Conf1;
extern DevConfig USART3Conf2;
extern DevConfig UART4Conf1;
extern DevConfig UART5Conf2;
extern DevConfig USART6Conf1;


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
     CR1_SBK = 0x0001,  //* Break Character
     IBUF_SZ = 128,     //* Input buffer size
     OBUF_SZ = 128      //* Output buffer size
   };
   Usart( const DevConfig *dcfg, const DevMode *dmode, uint32_t baud )
     : DevBase( dcfg, dmode ),
       usart( (USART_TypeDef*)(dcfg->base)),
       uitd{ baud, USART_WordLength_8b, USART_StopBits_1,
             USART_Parity_No, USART_Mode_Rx | USART_Mode_Tx,
             USART_HardwareFlowControl_None } // TODO: from mode
    {
    }
   void init();
   void deInit();
   void initHW();
   void enable()  { usart->CR1 |=  CR1_UE;  };
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
   uint16_t getSRErr() const { return sr_err; }
   ITStatus getITStatus( uint16_t it );
   void clearITPendingBit( uint16_t it );

   int sendStrLoop( const char* s );
   int sendBlockLoop( const char* d, int n );
   int sendStr( const char* s );
   int sendBlock( const char* d, int n );

   void setOnRecv( void (*new_fun)(char) ) { on_recv = new_fun; }
   void handleIRQ();
   void taskIO();

   // debug:
   uint16_t getIbuf_s() const { return ibuf_s; }
   uint16_t getIbuf_e() const { return ibuf_e; }
   uint16_t getObuf_s() const { return obuf_s; }
   uint16_t getObuf_e() const { return obuf_e; }
   bool     getOnTrans() const { return on_transmit; }

  protected:
   USART_TypeDef *usart;
   USART_InitTypeDef uitd;
   void (*on_recv)( char c ) = nullptr;
   uint16_t sr_err = 0;
   uint16_t ibuf_s = 0, ibuf_e = 0, obuf_s = 0, obuf_e = 0;
   bool on_transmit = false;
   char ibuf[IBUF_SZ];
   char obuf[OBUF_SZ];
};

#define STD_USART_IRQ( n, obj ) \
  void USART##n_IRQHandler(void) { obj.handleIRQ(); }



#endif
// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include
