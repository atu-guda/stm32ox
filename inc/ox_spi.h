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
extern DevMode SPIMode_Duplex_Master_NSS_Soft;
extern DevMode SPIMode_Duplex_Slave;
extern DevMode SPIMode_Duplex_Slave_NSS;
extern DevMode SPIMode_Duplex_Slave_NSS_Soft;


class DevSPI : public DevBase {
  public:
   enum PinNums {
     pinnum_SCK = 0, pinnum_MISO = 1, pinnum_MOSI = 2, pinnum_NSS = 3
   };
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
     BIDIMODE  = 0x8000,            //* Bidirectional data mode enable
     CLEAR_MASK= 0x3040             //* drop all, except MSTR, CRC*
   };
   enum CFG {
     // config names:
     L2_DUPLEX = 0x0000,            //* 2 lines duplex
     L2_RX     = 0x0400,            //* 2 lines RX only
     L1_RX     = 0x8000,            //* 1 line  RX only
     L1_TX     = 0xC000,            //* 1 line  TX only
     MASTER    = 0x0104,            //* Master: MSTR | SSI (auto by dev->mode)
     SLAVE     = 0x0000,            //* Slave
     DS_8b     = 0x0000,            //* Datasize: 8 bit
     DS_16b    = 0x0800,            //* Datasize: 16 bit = DFF
     CPOL_LOW  = 0x0000,            //* Polarity: active low
     CPOL_HIGH = 0x0002,            //* Polarity: active high = CPOL
     CHPAL_1E  = 0x0000,            //* Phase: first edge
     CHPAL_2E  = 0x0001,            //* Phase: second edge = CHPA
     NSS_HARD  = 0x0000,            //* Automatic NSS control
     NSS_SOFT  = 0x0200,            //* Software NSS control = SSM
     MSB_FIRST = 0x0000,            //* Most significant first
     LSB_FIRST = 0x0080,            //* Less significant first = LSBFIRST
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
     RXNE       = 0x01,              //* Receive buffer Not Empty
     TXE        = 0x02,              //* Transmit buffer Empty
     CHSIDE     = 0x04,              //* Channel side
     UDR        = 0x08,              //* Underrun flag
     CRCERR     = 0x10,              //* CRC Error flag
     MODF       = 0x20,              //* Mode fault
     OVR        = 0x40,              //* Overrun flag
     BSY        = 0x80               //* Busy flag

   };

   DevSPI( const DevConfig *dconf, const DevMode *dmode, uint16_t a_cr1_init );
   void init();
   void deInit();
   SPI_TypeDef* getDev() { return spi; };
   void enable()     { spi->CR1 |= (uint16_t)( CR1F::SPE); };
   void disable()    { spi->CR1 &= (uint16_t)(~CR1F::SPE); };
   void set_crc_poly( uint16_t v ) { crc_poly = v; }
   uint16_t getCR1() { return spi->CR1; }
   uint16_t getSR()  { return spi->SR; }
   uint16_t getData() { return spi->DR; }
   void     setData( uint16_t d ) { spi->DR = d; }

   int get_wait_count() const { return wait_count; };
   int wait_flag( uint16_t flg );
   int wait_nflag( uint16_t flg );
   int wait_TXE() { return wait_flag( SRF::TXE ); }
   int wait_RXNE() { return wait_flag( SRF::RXNE ); }
   int wait_N_BSY() { return wait_nflag( SRF::BSY ); }

   uint16_t send1( uint16_t vs ); // all returns number of tranferred in last direction
   uint16_t recv1( uint16_t *vr );
   uint16_t send1_recv1( uint16_t vs, uint16_t *vr );
   uint16_t send1_recvN_b( uint16_t vs, uint8_t *vr, int nr );
   uint16_t sendM_recvN_b( uint8_t *vs, int ns, uint8_t *vr, int nr );

  protected:
   uint16_t cr1_init;
   SPI_TypeDef *spi;
   uint16_t crc_poly = 7;
   int maxWait = 5000;
   void (*wait_fun)() = taskYieldFun;
   int wait_count = 0;
   int n_trans = 0;
   int err = 0;

};



#endif
