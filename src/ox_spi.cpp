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

#define NSS_HOLDER PinHold ph( &( cfg->pins[pinnum_NSS] ), inv_nss, ! (cr1_init & CFG::NSS_SOFT ) )

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
  nss_off();
  err = 0; n_trans = 0;
}

void DevSPI::deInit()
{
  SPI_DeInit( spi );
  nss_off();
};

void DevSPI::nss_off()
{
  if( ! ( cr1_init & CFG::NSS_SOFT  ) ) {
    return;
  }
  if( inv_nss ) {
    pin_reset( &( cfg->pins[pinnum_NSS] ) );
  } else {
    pin_set( &( cfg->pins[pinnum_NSS] ) );
  }
}

void DevSPI::nss_on()
{
  if( ! ( cr1_init & CFG::NSS_SOFT  ) ) {
    return;
  }
  if( inv_nss ) {
    pin_set( &( cfg->pins[pinnum_NSS] ) );
  } else {
    pin_reset( &( cfg->pins[pinnum_NSS] ) );
  }
}

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

int DevSPI::send1x( uint16_t vs )
{
  n_trans = 0;
  NSS_HOLDER;
  if( ! wait_TXE() ) {
    return 0;
  }
  spi->DR = vs;
  n_trans = 1;
  delay_bad_ms(10);
  return 1;
}


int DevSPI::recv1( uint16_t *vr )
{
  n_trans = 0;
  NSS_HOLDER;
  if( ! wait_TXE() ) {  return 0;  }
  spi->DR = 0x55;
  if( ! wait_TXE() ) {  return 0;  }
  if( ! wait_RXNE() ) { return 0;  }
  uint16_t t = spi->DR;
  if( vr ) { *vr = t; };
  return 1;
}


int DevSPI::send1_recv1( uint16_t vs, uint16_t *vr )
{
  n_trans = 0;
  NSS_HOLDER;
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


int DevSPI::send1_recvN_b( uint16_t vs, uint8_t *vr, int nr )
{
  n_trans = 0;
  if( cr1_init & CFG::DS_16b ) { return 0; }

  NSS_HOLDER;
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


int DevSPI::sendM_recvN_b( const uint8_t *vs, int ns, uint8_t *vr, int nr )
{
  n_trans = 0;
  if( cr1_init & CFG::DS_16b ) { return 0; }
  if( !vs ) { return 0; }

  uint16_t t;
  NSS_HOLDER;

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

int DevSPI::sendM_sendN_b( const uint8_t *vs0, int ns0, const uint8_t *vs1, int ns1 )
{
  n_trans = 0;
  if( cr1_init & CFG::DS_16b ) { return 0; }
  if( !vs0 || ( ns1 !=0 && ! vs1 ) ) { return 0; }

  uint16_t t;
  NSS_HOLDER;

  for( int i=0; i<ns0; ++i ) {
    if( ! wait_TXE() ) {  return 0;  }
    spi->DR = *vs0++;
    if( ! wait_RXNE() ) { return 0;  }
    t = spi->DR; // old (fake)
    ++n_trans;
  }

  for( int i=0; i<ns1; ++i ) {
    if( ! wait_TXE() ) {  return 0;  }
    spi->DR = *vs1++;
    if( ! wait_RXNE() ) { return 0;  }
    t = spi->DR; // old (fake)
    ++n_trans;
  }
  ++t; // a-la use

  return n_trans;
}



int DevSPI::duplexN_b( const uint8_t *vs, int ns, uint8_t *vr )
{
  n_trans = 0;
  if( cr1_init & CFG::DS_16b ) { return 0; }
  if( !vs || ! vr ) { return 0; }

  NSS_HOLDER;

  for( int i=0; i<=ns; ++i ) { // <= SIC! one more byte
    if( ! wait_TXE() ) {  return 0;  }
    uint8_t sb = 0;
    if( i != ns ) {
      sb = *vs++;
    }
    spi->DR = sb;
    if( ! wait_RXNE() ) { return 0;  }
    uint8_t rb = spi->DR;
    if( i !=0 ) {
      *vr++ = rb;
    }
    ++n_trans;
  }


  return n_trans;
}




