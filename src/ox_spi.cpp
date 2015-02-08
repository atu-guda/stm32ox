#include <ox_spi.h>

PinModeNum  SPIMode_Duplex_pins[] {
  pinMode_AF_PP, // SCK
  pinMode_AF_PP, // MISO
  pinMode_AF_PP  // MOSI
};

DevMode SPIMode_Duplex_Master = {
  SPI_Mode_Master, // mode
  ARR_AND_SZ( SPIMode_Duplex_pins )
};

DevMode SPIMode_Duplex_Slave = {
  SPI_Mode_Slave, // mode
  ARR_AND_SZ( SPIMode_Duplex_pins )
};


PinModeNum  SPIMode_Duplex_NSS_pins[] {
  pinMode_AF_PP, // SCK
  pinMode_AF_PP, // MISO
  pinMode_AF_PP, // MOSI
  pinMode_AF_PP  // NSS
};


DevMode SPIMode_Duplex_Master_NSS = {
  SPI_Mode_Master, // mode
  ARR_AND_SZ( SPIMode_Duplex_NSS_pins )
};


DevMode SPIMode_Duplex_Slave_NSS = {
  SPI_Mode_Slave, // mode
  ARR_AND_SZ( SPIMode_Duplex_NSS_pins )
};




void DevSPI::init()
{
  disable();
  enable();
  uint16_t t = spi->CR1;
  err = 0; n_trans = 0;
}


// int DevSPI::waitForEv( uint32_t ev, int errcode )
// {
//   wait_count = 0;
//   while( ! checkEv( ev ) ) {
//     wait_fun();
//     ++wait_count;
//     if( wait_count >= maxWait ) {
//       err = errcode;
//       return 0;
//     }
//   }
//   return 1;
// }
//
// int DevSPI::waitNoBusy()
// {
//   wait_count = 0;
//   while( SPI_GetFlagStatus( i2c, SPI_FLAG_BUSY ) )  {
//     wait_fun();
//     ++wait_count;
//     if( wait_count >= maxWait ) {
//       err = -10;
//       return 0;
//     }
//   }
//   return 1;
// }
//
// int DevSPI::prep(  bool is_transmit, bool noWait )
// {
//   err = 0; n_trans = 0;
//   if( !noWait && ! waitNoBusy() ) {
//     return 0;
//   }
//
//   genSTART();
//   if( ! waitForEv( SPI_EVENT_MASTER_MODE_SELECT, -1 ) ) { // Test on EV5 and clear it
//     return 0;
//   }
//
//   addr <<= 1; // place for !w/r bit
//   uint32_t ev;
//
//   if( is_transmit )  {
//     ev = SPI_EVENT_MASTER_TRANSMITTER_MODE_SELECTED;
//   } else {
//     addr |= 0x01;  // Set the address bit0 for read
//     ev = SPI_EVENT_MASTER_RECEIVER_MODE_SELECTED;
//   }
//   i2c->DR = addr;
//
//   return waitForEv( ev, -2 );
// }
//
//
// int DevSPI::send_pure( const uint8_t *ds, int ns )
// {
//   for( int i=0; i<ns; ++i ) {
//     i2c->DR = *ds;
//     ds++;
//     if( ! waitForEv( SPI_EVENT_MASTER_BYTE_TRANSMITTING, -4 ) ) {
//       return 0;
//     }
//     ++n_trans;
//   }
//   return 1;
// }
//
//
// int  DevSPI::send(  uint8_t ds )
// {
//   return send( addr, &ds, 1 );
// }
//
// int  DevSPI::send(  const uint8_t *ds, int ns )
// {
//   if( prep( addr, true ) ) {
//     send_pure( ds, ns );
//   }
//   genSTOP();
//   return ( err == 0 ) ? n_trans : err;
// }
//
// int  DevSPI::send_reg_n(  uint32_t reg, uint8_t reglen,  const uint8_t *ds, int ns )
// {
//   uint32_t reg_rev = __REV( reg );
//   const uint8_t *aptr = (const uint8_t*)(&reg_rev);
//
//   if( reglen > 4 ) {
//     reglen = 4;
//   }
//
//   aptr += 4 - reglen;  // set offset to start byte
//
//   if( prep( addr, true ) ) {
//     if( send_pure( aptr, reglen ) ) {
//       send_pure( ds, ns );
//     }
//   }
//
//   genSTOP();
//   return ( err == 0 ) ? n_trans : err;
// }
//
//
// int  DevSPI::send_reg1(  uint8_t reg,  const uint8_t *ds, int ns )
// {
//   return send_reg_n( addr, reg, 1, ds, ns );
// }
//
// int  DevSPI::send_reg2(  uint16_t reg, const uint8_t *ds, int ns )
// {
//   return send_reg_n( addr, reg, 2, ds, ns );
// }
//
//
// int  DevSPI::recv( uint8_t addr )
// {
//   uint8_t v;
//   int r = recv( addr, &v, 1 );
//
//   return ( r > 0 ) ? v : err;
//
// }
//
// int  DevSPI::recv_pure( uint8_t *dd, int nd )
// {
//   if( nd < 2 ) {
//     asknDisable(); // single
//   }
//
//   for( n_trans=0; n_trans<nd; ++n_trans ) {
//     if( ! waitForEv( SPI_EVENT_MASTER_BYTE_RECEIVED, -3 ) ) {
//       return 0;
//     }
//    *dd++ = (uint8_t)( i2c->DR );
//    if( n_trans >=nd-2 ) {
//      asknDisable(); // <-------- last?
//    }
//   }
//   return 1;
// }
//
// int  DevSPI::recv(  uint8_t *dd, int nd )
// {
//   if( prep( addr, false ) ) {
//     recv_pure( dd, nd );
//   }
//
//   asknEnable();
//   genSTOP();
//
//   return ( err == 0 ) ? n_trans : err;
// }
//
// int  DevSPI::recv_reg_n(  int32_t reg, uint8_t reglen,  uint8_t *dd, int nd )
// {
//   uint32_t reg_rev = __REV( reg );
//   const uint8_t *aptr = (const uint8_t*)(&reg_rev);
//
//   if( reglen > 4 ) {
//     reglen = 4;
//   }
//
//   aptr += 4 - reglen;  // set offset to start byte
//
//   return send_recv( addr, aptr, reglen, dd, nd );
// }
//
// int  DevSPI::recv_reg1(  int8_t reg,  uint8_t *dd, int nd )
// {
//   return recv_reg_n( addr, reg, 1, dd, nd );
// }
//
// int  DevSPI::recv_reg2(  int16_t reg, uint8_t *dd, int nd )
// {
//   return recv_reg_n( addr, reg, 2, dd, nd );
// }
//
// int  DevSPI::send_recv(  const uint8_t *ds, int ns, uint8_t *dd, int nd )
// {
//   if( prep( addr, true ) ) { // start as transmitter
//     if( send_pure( ds, ns ) ) {
//       if( prep( addr, false, true ) ) { // restart
//         recv_pure( dd, nd );
//       } else {
//         err = -20;
//       }
//     }
//   }
//
//   genSTOP();
//   asknEnable();
//   return ( err == 0 ) ? n_trans : err;
// }
//
