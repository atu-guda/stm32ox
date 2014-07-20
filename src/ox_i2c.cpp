#include <ox_i2c.h>


DevMode I2CModeI2C_Master = {
  I2C_Mode_I2C, // mode
  2,            // n_req
  3,            // pin_num
  { pinMode_AF_OD, // CLK
    pinMode_AF_OD, // SDA
    pinMode_NONE,  // no SMBA
    pinMode_NONE, pinMode_NONE,
    pinMode_NONE, pinMode_NONE, pinMode_NONE
  }
};

DevMode I2CModeSMB_Master = {
  I2C_Mode_SMBusHost, // mode
  3,            // n_req
  3,            // pin_num
  { pinMode_AF_OD, // CLK
    pinMode_AF_OD, // SDA
    pinMode_AF_OD, // no SMBA
    pinMode_NONE, pinMode_NONE,
    pinMode_NONE, pinMode_NONE, pinMode_NONE
  }
};

void DevI2C::init()
{
  disable();
  enable();
  I2C_InitTypeDef itd;
  I2C_StructInit( &itd );
  itd.I2C_ClockSpeed = speed;
  itd.I2C_Mode = mode->mode;
  itd.I2C_OwnAddress1 = own_addr;
  I2C_Init( i2c, &itd );
  asknEnable();
  err = 0; n_trans = 0;
}

// I2C_WAITFOR( I2C_EVENT_MASTER_BYTE_TRANSMITTED, -4 );

void DevI2C::initHW()
{
  DevBase::initHW();
}

int DevI2C::waitForEv( uint32_t ev, int errcode )
{
  wait_count = 0;
  while( ! checkEv( ev ) ) {
    wait_fun();
    ++wait_count;
    if( wait_count >= maxWait ) {
      err = errcode;
      return 0;
    }
  }
  return 1;
}

int DevI2C::waitNoBusy()
{
  wait_count = 0;
  while( I2C_GetFlagStatus( i2c, I2C_FLAG_BUSY ) )  {
    wait_fun();
    ++wait_count;
    if( wait_count >= maxWait ) {
      err = -10;
      return 0;
    }
  }
  return 1;
}

int DevI2C::prep( uint8_t addr, bool is_transmit, bool noWait )
{
  err = 0; n_trans = 0;
  if( !noWait && ! waitNoBusy() ) {
    return 0;
  }

  genSTART();
  if( ! waitForEv( I2C_EVENT_MASTER_MODE_SELECT, -1 ) ) { // Test on EV5 and clear it
    return 0;
  }

  addr <<= 1; // place for !w/r bit
  uint32_t ev;

  if( is_transmit )  {
    ev = I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED;
  } else {
    addr |= 0x01;  // Set the address bit0 for read
    ev = I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED;
  }
  i2c->DR = addr;

  return waitForEv( ev, -2 );
}

int DevI2C::ping( uint8_t addr )
{
  int rc = prep( addr, true );
  genSTOP();
  return rc;

}

int DevI2C::send_pure( const uint8_t *ds, int ns )
{
  for( int i=0; i<ns; ++i ) {
    i2c->DR = *ds;
    ds++;
    if( ! waitForEv( I2C_EVENT_MASTER_BYTE_TRANSMITTING, -4 ) ) {
      return 0;
    }
    ++n_trans;
  }
  return 1;
}


int  DevI2C::send( uint8_t addr, uint8_t ds )
{
  return send( addr, &ds, 1 );
}

int  DevI2C::send( uint8_t addr, const uint8_t *ds, int ns )
{
  if( prep( addr, true ) ) {
    send_pure( ds, ns );
  }
  genSTOP();
  return ( err == 0 ) ? n_trans : err;
}

int  DevI2C::send_reg_n( uint8_t addr, uint32_t reg, uint8_t reglen,  const uint8_t *ds, int ns )
{
  uint32_t reg_rev = __REV( reg );
  const uint8_t *aptr = (const uint8_t*)(&reg_rev);

  if( reglen > 4 ) {
    reglen = 4;
  }

  aptr += 4 - reglen;  // set offset to start byte

  if( prep( addr, true ) ) {
    if( send_pure( aptr, reglen ) ) {
      send_pure( ds, ns );
    }
  }

  genSTOP();
  return ( err == 0 ) ? n_trans : err;
}


int  DevI2C::send_reg1( uint8_t addr, uint8_t reg,  const uint8_t *ds, int ns )
{
  return send_reg_n( addr, reg, 1, ds, ns );
}

int  DevI2C::send_reg2( uint8_t addr, uint16_t reg, const uint8_t *ds, int ns )
{
  return send_reg_n( addr, reg, 2, ds, ns );
}


int  DevI2C::recv( uint8_t addr )
{
  uint8_t v;
  int r = recv( addr, &v, 1 );

  return ( r > 0 ) ? v : err;

}

int  DevI2C::recv_pure( uint8_t *dd, int nd )
{
  if( nd < 2 ) {
    asknDisable(); // single
  }

  for( n_trans=0; n_trans<nd; ++n_trans ) {
    if( ! waitForEv( I2C_EVENT_MASTER_BYTE_RECEIVED, -3 ) ) {
      return 0;
    }
   *dd++ = (uint8_t)( i2c->DR );
   if( n_trans >=nd-2 ) {
     asknDisable(); // <-------- last?
   }
  }
  return 1;
}

int  DevI2C::recv( uint8_t addr, uint8_t *dd, int nd )
{
  if( prep( addr, false ) ) {
    recv_pure( dd, nd );
  }

  asknEnable();
  genSTOP();

  return ( err == 0 ) ? n_trans : err;
}

int  DevI2C::recv_reg_n( uint8_t addr, int32_t reg, uint8_t reglen,  uint8_t *dd, int nd )
{
  uint32_t reg_rev = __REV( reg );
  const uint8_t *aptr = (const uint8_t*)(&reg_rev);

  if( reglen > 4 ) {
    reglen = 4;
  }

  aptr += 4 - reglen;  // set offset to start byte

  return send_recv( addr, aptr, reglen, dd, nd );
}

int  DevI2C::recv_reg1( uint8_t addr, int8_t reg,  uint8_t *dd, int nd )
{
  return recv_reg_n( addr, reg, 1, dd, nd );
}

int  DevI2C::recv_reg2( uint8_t addr, int16_t reg, uint8_t *dd, int nd )
{
  return recv_reg_n( addr, reg, 2, dd, nd );
}

int  DevI2C::send_recv( uint8_t addr, const uint8_t *ds, int ns, uint8_t *dd, int nd )
{
  if( prep( addr, true ) ) { // start as transmitter
    if( send_pure( ds, ns ) ) {
      if( prep( addr, false, true ) ) { // restart
        recv_pure( dd, nd );
      } else {
        err = -20;
      }
    }
  }

  genSTOP();
  asknEnable();
  return ( err == 0 ) ? n_trans : err;
}

