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
  for( wait_count=0; wait_count<maxWait; ++wait_count ) {
    if( spi->SR & flg ) {
      return 1;
    }
    wait_fun();
  }
  return 0;
}


int DevSPI::wait_nflag( uint16_t flg )
{
  for( wait_count=0; wait_count<maxWait; ++wait_count ) {
    if( ! (spi->SR & flg) ) {
      return 1;
    }
    wait_fun();
  }
  return 0;
}

uint16_t DevSPI::send1( uint16_t vs )
{
  n_trans = 0;
  PinHold ph( &( cfg->pins[pinnum_NSS] ), false, ! (cr1_init & CFG::NSS_SOFT ) );
  if( ! wait_TXE() ) {
    return 0;
  }
  spi->DR = vs;
  n_trans = 1;
  return 1;
}


uint16_t DevSPI::recv1( uint16_t *vr )
{
  n_trans = 0;
  PinHold ph( &( cfg->pins[pinnum_NSS] ), false, ! (cr1_init & CFG::NSS_SOFT ) );
  if( ! wait_TXE() ) {  return 0;  }
  spi->DR = 0x55;
  if( ! wait_TXE() ) {  return 0;  }
  if( ! wait_RXNE() ) { return 0;  }
  uint16_t t = spi->DR;
  if( vr ) { *vr = t; };
  return 1;
}


uint16_t DevSPI::send1_recv1( uint16_t vs, uint16_t *vr )
{
  n_trans = 0;
  PinHold ph( &( cfg->pins[pinnum_NSS] ), false, ! (cr1_init & CFG::NSS_SOFT ) );
  if( ! wait_TXE() ) {  return 0;  }
  spi->DR = vs;
  if( ! wait_TXE() ) {  return 0;  }
  if( ! wait_RXNE() ) { return 0;  }
  uint16_t t = spi->DR;
  if( ! wait_TXE() ) {  return 0;  }
  spi->DR = 0x55; // fake
  if( ! wait_RXNE() ) { return 0;  }
  t = spi->DR;
  if( vr ) { *vr = t; };
  return 1;
}


uint16_t DevSPI::send1_recvN_b( uint16_t vs, uint8_t *vr, int nr )
{
  n_trans = 0;
  if( cr1_init & CFG::DS_16b ) { return 0; }

  PinHold ph( &( cfg->pins[pinnum_NSS] ), false, ! (cr1_init & CFG::NSS_SOFT ) );
  if( ! wait_TXE() ) {  return 0;  }
  spi->DR = vs;
  // if( ! wait_TXE() ) {  return 0;  }
  if( ! wait_RXNE() ) { return 0;  }
  uint16_t t = spi->DR; // old (fake)

  for( ; n_trans < nr; ++n_trans ) {
    if( ! wait_TXE() ) {  return 0;  }
    spi->DR = 0x55; // fake
    // if( ! wait_TXE() ) {  return 0;  }
    if( ! wait_RXNE() ) { return 0;  }
    t = spi->DR;
    if( vr ) { *vr++ = t; };
  }
  return n_trans;
}


uint16_t DevSPI::sendM_recvN_b( uint8_t *vs, int ns, uint8_t *vr, int nr )
{
  n_trans = 0;
  if( cr1_init & CFG::DS_16b ) { return 0; }
  if( !vs ) { return 0; }

  uint16_t t;
  PinHold ph( &( cfg->pins[pinnum_NSS] ), false, ! (cr1_init & CFG::NSS_SOFT ) );
  for( int i=0; i<ns; ++i ) {
    if( ! wait_TXE() ) {  return 0;  }
    spi->DR = *vs++;
    if( ! wait_RXNE() ) { return 0;  }
    t = spi->DR; // old (fake)
  }

  for( ; n_trans < nr; ++n_trans ) {
    if( ! wait_TXE() ) {  return 0;  }
    spi->DR = 0x55; // fake
    if( ! wait_TXE() ) {  return 0;  }
    if( ! wait_RXNE() ) { return 0;  }
    t = spi->DR;
    if( vr ) { *vr++ = t; };
  }
  return n_trans;
}

