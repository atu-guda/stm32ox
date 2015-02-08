#ifndef _OX_SPI_H
#define _OX_SPI_H

#if defined STM32F1
 #include <stm32f10x_spi.h>
#elif defined STM32F2
 #include <stm32f2xx_spi.h>
#elif defined STM32F3
 #include <stm32f30x_spi.h>
#elif defined STM32F4
 #include <stm32f4xx_spi.h>
#else
 #error "Undefined MC type"
#endif // STM32Fxxx

// #include <stm32f10x_gpio.h>

#include <ox_dev.h>

extern DevConfig SPI1Conf1;
extern DevConfig SPI1Conf2;
extern DevConfig SPI2Conf1;
extern DevConfig SPI2Conf2;
extern DevConfig SPI3Conf1;
extern DevConfig SPI3Conf2;

extern DevMode SPIMode_Duplex_Master;
extern DevMode SPIMode_Duplex_Master_NSS;
extern DevMode SPIMode_Duplex_Slave;
extern DevMode SPIMode_Duplex_Slave_NSS;


class DevSPI : public DevBase {
  public:
   enum CR1F {
     CHPA      = 0x0001,            //* Clock Phase
     CPOL      = 0x0002,            //* Clock Polarity
     MSTR      = 0x0004,            //* Master Selection
     BRP_b0    = 0x0008,            //* BaudRate_Prescaler (BRP) Bit 0
     BRP_b1    = 0x0010,            //* BRP Bit 1
     BRP_b2    = 0x0020,            //* BRP Bit 2
     SPE       = 0x0040,            //* SPI Enable
     LSBFIRST  = 0x0080,            //* Frame Format
     SSI       = 0x0100,            //* Internal slave select
     SSM       = 0x0200,            //* Software slave management
     RXONLY    = 0x0400,            //* Receive only
     DFF       = 0x0800,            //* Data Frame Format
     CRCNEXT   = 0x1000,            //* Transmit CRC next
     CRCEN     = 0x2000,            //* Hardware CRC calculation enable
     BIDIOE    = 0x4000,            //* Output enable in bidirectional mode
     BIDIMODE  = 0x8000             //* Bidirectional data mode enable
   };
   enum CFG {
     // config names:
     L2_DUPLEX = 0x0000,            //* 2 lines duplex
     L2_RX     = 0x0400,            //* 2 lines RX only
     L1_RX     = 0x8000,            //* 1 line  RX only
     L1_TX     = 0xC000,            //* 1 line  TX only
     MASTER    = 0x0104,            //* Master: MSTR | SSI
     SLAVE     = 0x0000,            //* Slave
     DS_8b     = 0x0000,            //* Datasize: 8 bit
     DS_16b    = 0x0800,            //* Datasize: 16 bit = DFF
     BRP_2     = 0x0000,            //* BR Bits:
     BRP_4     = 0x0008,            //*
     BRP_8     = 0x0010,            //*
     BRP_16    = 0x0018,            //*
     BRP_32    = 0x0020,            //*
     BRP_64    = 0x0028,            //*
     BRP_128   = 0x0030,            //*
     BRP_256   = 0x0038             //*
   };

   enum CR2F {
     RXDMAEN   = 0x01,              //* Rx Buffer DMA Enable
     TXDMAEN   = 0x02,              //* Tx Buffer DMA Enable
     SSOE      = 0x04,              //* SS Output Enable
     ERRIE     = 0x20,              //* Error Interrupt Enable
     RXNEIE    = 0x40,              //* RX buffer Not Empty Interrupt Enable
     TXEIE     = 0x80               //* Tx buffer Empty Interrupt Enable
   };

   enum SRF {
     SR_RXNE       = 0x01,              //* Receive buffer Not Empty
     SR_TXE        = 0x02,              //* Transmit buffer Empty
     SR_CHSIDE     = 0x04,              //* Channel side
     SR_UDR        = 0x08,              //* Underrun flag
     SR_CRCERR     = 0x10,              //* CRC Error flag
     SR_MODF       = 0x20,              //* Mode fault
     SR_OVR        = 0x40,              //* Overrun flag
     SR_BSY        = 0x80               //* Busy flag

   };
   DevSPI( const DevConfig *dconf, const DevMode *dmode,
       uint16_t a_cr1_init )
     : DevBase( dconf, dmode ),
       cr1_init( a_cr1_init ),
       spi( (SPI_TypeDef*)(dconf->base))
    {
    }
   void init();
   void deInit() { SPI_DeInit( spi ); };
   SPI_TypeDef* getDev() { return spi; };
   void enable()     { spi->CR1 |= (uint16_t)( CR1F::SPE); };
   void disable()    { spi->CR1 &= (uint16_t)(~CR1F::SPE); };
   // int send_pure( const uint8_t *ds, int ns ); // return 0=Err 1=Ok
   // // returns: >0 = N of send/recv bytes, <0 - error
   // int  send(  uint8_t ds );
   // int  send(  const uint8_t *ds, int ns );
   // int  send_reg_n(  uint32_t reg, uint8_t reglen,  const uint8_t *ds, int ns );
   // int  send_reg1(  uint8_t reg,  const uint8_t *ds, int ns );
   // int  send_reg2(  uint16_t reg, const uint8_t *ds, int ns );
   // int  recv_pure( uint8_t *dd, int nd );
   // int  recv( uint8_t addr );
   // int  recv(  uint8_t *dd, int nd );
   // int  recv_reg_n(  int32_t reg, uint8_t reglen,  uint8_t *dd, int nd );
   // int  recv_reg1(  int8_t reg,  uint8_t *dd, int nd );
   // int  recv_reg2(  int16_t reg, uint8_t *dd, int nd );
   // int  send_recv(  const uint8_t *ds, int ns, uint8_t *dd, int nd );
   // void setMaxWait( uint32_t mv ) { maxWait = mv; }
   // int getWaitCount() const { return wait_count; }
   // uint16_t getNTrans() const { return n_trans; };
   // uint16_t getErr() const { return err; };
   // int waitForEv( uint32_t ev, int errcode ); // return 0 - failed to wait, != 0 - Ok
   // int waitNoBusy(); // same

  protected:
   uint16_t cr1_init;
   SPI_TypeDef *spi;
   int maxWait = 5000;
   void (*wait_fun)() = taskYieldFun;
   int wait_count = 0;
   int n_trans = 0;
   int err = 0;

};



#endif
