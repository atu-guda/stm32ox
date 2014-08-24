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

#include <FreeRTOS.h>
#include <queue.h>

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
             USART_HardwareFlowControl_None }, // TODO: from mode
       ibuf( xQueueCreate( IBUF_SZ, 1 ) ),
       obuf( xQueueCreate( OBUF_SZ, 1 ) )
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

   //* set callback function, called on every char
   void setOnRecv( void (*new_fun)(char) ) { on_recv = new_fun; }
   //* wait and receive char, make sense, only if "on_recv" is not set.
   int  getChar( int wait_tick = 500 );
   void handleIRQ();
   void task_recv(); // to be called from task (optional)
   void task_send(); // to be called from task

   // debug:
   bool     getOnTrans() const { return on_transmit; }

  protected:
   USART_TypeDef *usart;
   USART_InitTypeDef uitd;
   void (*on_recv)( char c ) = nullptr;
   uint16_t sr_err = 0;
   bool on_transmit = false;
   QueueHandle_t ibuf = nullptr;
   QueueHandle_t obuf = nullptr;
   int wait_tx = 500; // TODO: adjust
   int wait_rx = 500;
};

// common declarations
extern "C" {
  void USART1_IRQHandler(void);
  void USART2_IRQHandler(void);
  void USART3_IRQHandler(void);
  void USART4_IRQHandler(void);
  void USART5_IRQHandler(void);
  void USART6_IRQHandler(void);
  void task_usart1_send( void *prm UNUSED );
  void task_usart1_recv( void *prm UNUSED );
  void task_usart2_send( void *prm UNUSED );
  void task_usart2_recv( void *prm UNUSED );
  void task_usart3_send( void *prm UNUSED );
  void task_usart3_recv( void *prm UNUSED );
  void task_usart4_send( void *prm UNUSED );
  void task_usart4_recv( void *prm UNUSED );
  void task_usart5_send( void *prm UNUSED );
  void task_usart5_recv( void *prm UNUSED );
  void task_usart6_send( void *prm UNUSED );
  void task_usart7_recv( void *prm UNUSED );
}

#define STD_USART1_IRQ( obj ) void USART1_IRQHandler(void) { obj.handleIRQ(); }
#define STD_USART2_IRQ( obj ) void USART2_IRQHandler(void) { obj.handleIRQ(); }
#define STD_USART3_IRQ( obj ) void USART3_IRQHandler(void) { obj.handleIRQ(); }
#define STD_USART4_IRQ( obj ) void USART4_IRQHandler(void) { obj.handleIRQ(); }
#define STD_USART5_IRQ( obj ) void USART5_IRQHandler(void) { obj.handleIRQ(); }
#define STD_USART6_IRQ( obj ) void USART6_IRQHandler(void) { obj.handleIRQ(); }

#define STD_USART1_SEND_TASK( obj ) \
  void task_usart1_send( void *prm UNUSED ) { while(1) {  obj.task_send(); }  vTaskDelete(0); }
#define STD_USART2_SEND_TASK( obj ) \
  void task_usart2_send( void *prm UNUSED ) { while(1) {  obj.task_send(); }  vTaskDelete(0); }
#define STD_USART3_SEND_TASK( obj ) \
  void task_usart3_send( void *prm UNUSED ) { while(1) {  obj.task_send(); }  vTaskDelete(0); }
#define STD_USART4_SEND_TASK( obj ) \
  void task_usart4_send( void *prm UNUSED ) { while(1) {  obj.task_send(); }  vTaskDelete(0); }
#define STD_USART5_SEND_TASK( obj ) \
  void task_usart5_send( void *prm UNUSED ) { while(1) {  obj.task_send(); }  vTaskDelete(0); }
#define STD_USART6_SEND_TASK( obj ) \
  void task_usart6_send( void *prm UNUSED ) { while(1) {  obj.task_send(); }  vTaskDelete(0); }


#define STD_USART1_RECV_TASK( obj ) \
  void task_usart1_recv( void *prm UNUSED ) { while(1) {  obj.task_recv(); }  vTaskDelete(0); }
#define STD_USART2_RECV_TASK( obj ) \
  void task_usart2_recv( void *prm UNUSED ) { while(1) {  obj.task_recv(); }  vTaskDelete(0); }
#define STD_USART3_RECV_TASK( obj ) \
  void task_usart3_recv( void *prm UNUSED ) { while(1) {  obj.task_recv(); }  vTaskDelete(0); }
#define STD_USART4_RECV_TASK( obj ) \
  void task_usart4_recv( void *prm UNUSED ) { while(1) {  obj.task_recv(); }  vTaskDelete(0); }
#define STD_USART5_RECV_TASK( obj ) \
  void task_usart5_recv( void *prm UNUSED ) { while(1) {  obj.task_recv(); }  vTaskDelete(0); }
#define STD_USART6_RECV_TASK( obj ) \
  void task_usart6_recv( void *prm UNUSED ) { while(1) {  obj.task_recv(); }  vTaskDelete(0); }

#endif
// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include
