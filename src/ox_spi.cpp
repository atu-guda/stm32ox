#include <ox_spi.h>

PinModeNum  SPIMode_Duplex_pins[] {
  pinMode_AF_PP, // SCK
  pinMode_AF_PP, // MISO
  pinMode_AF_PP  // MOSI
};

DevMode SPIMode_Duplex_Master = {
  DevSPI::CFG::MASTER, // mode
  ARR_AND_SZ( SPIMode_Duplex_pins )
};

DevMode SPIMode_Duplex_Slave = {
  DevSPI::CFG::SLAVE, // mode
  ARR_AND_SZ( SPIMode_Duplex_pins )
};


PinModeNum  SPIMode_Duplex_NSS_pins[] {
  pinMode_AF_PP, // SCK
  pinMode_AF_PP, // MISO
  pinMode_AF_PP, // MOSI
  pinMode_AF_PP  // NSS
};


DevMode SPIMode_Duplex_Master_NSS = {
  DevSPI::CFG::MASTER, // mode
  ARR_AND_SZ( SPIMode_Duplex_NSS_pins )
};


DevMode SPIMode_Duplex_Slave_NSS = {
  DevSPI::CFG::SLAVE, // mode
  ARR_AND_SZ( SPIMode_Duplex_NSS_pins )
};


PinModeNum  SPIMode_Duplex_NSS_Soft_pins[] {
  pinMode_AF_PP, // SCK
  pinMode_AF_PP, // MISO
  pinMode_AF_PP, // MOSI
  pinMode_Out_PP // NSS
};

DevMode SPIMode_Duplex_Master_NSS_Soft = {
  DevSPI::CFG::MASTER | DevSPI::CFG::NSS_SOFT, // mode
  ARR_AND_SZ( SPIMode_Duplex_NSS_Soft_pins )
};


DevMode SPIMode_Duplex_Slave_NSS_Soft = {
  DevSPI::CFG::SLAVE | DevSPI::CFG::NSS_SOFT, // mode
  ARR_AND_SZ( SPIMode_Duplex_NSS_Soft_pins )
};

// -------------------------------------------------

DevSPI::DevSPI( const DevConfig *dconf, const DevMode *dmode, uint16_t a_cr1_init )
     : DevBase( dconf, dmode ),
       cr1_init( a_cr1_init | dmode->mode ),
       spi( (SPI_TypeDef*)(dconf->base))
{
}

void DevSPI::init()
{
  disable();
  uint16_t t = spi->CR1;
  t &= CR1F::CLEAR_MASK;
  t |= cr1_init;
  spi->CR1 = t;
  spi->I2SCFGR &= (uint16_t)~SPI_I2SCFGR_I2SMOD;
  spi->CRCPR   = crc_poly;
  // TODO: soft nss ctl
  if( cr1_init && CFG::NSS_SOFT ) {
    pin_set( &( cfg->pins[pinnum_NSS] ) );
  }
  err = 0; n_trans = 0;
}

void DevSPI::deInit()
{
  SPI_DeInit( spi );
  if( cr1_init && CFG::NSS_SOFT ) {
    pin_set( &( cfg->pins[pinnum_NSS] ) );
  }
};

int DevSPI::wait_flag( uint16_t flg )
{
  for( int i=0; i<maxWait; ++i ) {
    if( spi->SR & flg ) {
      return 1;
    }
    wait_fun();
  }
  return 0;
}


int DevSPI::wait_nflag( uint16_t flg )
{
  for( int i=0; i<maxWait; ++i ) {
    if( ! (spi->SR & flg) ) {
      return 1;
    }
    wait_fun();
  }
  return 0;
}

uint16_t DevSPI::send_recv1( uint16_t vs )
{
  PinHold( &( cfg->pins[pinnum_NSS] ), false, ! (cr1_init & CFG::NSS_SOFT ) );
  if( ! wait_TXE() ) {
    return 0;
  }
  spi->DR = vs;
  if( ! wait_TXE() ) {
    return 0;
  }
  spi->DR = 0x55;
  if( ! wait_RXNE() ) {
    return 0;
  }
  uint16_t vr = spi->DR;
  return vr;
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
