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
  enable();
  I2C_InitTypeDef itd;
  I2C_StructInit( &itd );
  itd.I2C_ClockSpeed = speed;
  itd.I2C_Mode = mode->mode;
  itd.I2C_OwnAddress1 = own_addr;
  I2C_Init( i2c, &itd );
  asknEnable();
}

void DevI2C::initHW()
{
  DevBase::initHW();
}

//  while( ! I2C_CheckEvent( I2Cn, EVENT ) ) {

#define I2C_WAITFOR( I2Cn, EVENT, res ) \
  wait_count = 0; \
  while( ! checkEv( EVENT ) ) { \
    wait_fun(); \
    ++wait_count; \
    if( wait_count >= maxWait ) { \
      err = res; goto end; \
    } \
  };

#define I2C_WAITNOBUSY( I2Cn ) \
  wait_count = 0;\
  while( I2C_GetFlagStatus( I2Cn, I2C_FLAG_BUSY ) )  { \
    wait_fun(); \
    ++wait_count; \
    if( wait_count >= maxWait ) { \
      err = -10; goto end; \
    } \
  }


int  DevI2C::send( uint8_t addr, uint8_t ds )
{
  // START, addr, sl:ack, cmd, sl:acl, data, sl:ack, STOP
  __label__ end;
  err = 0; n_trans = 0;

  I2C_WAITNOBUSY( i2c );

  genSTART();
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -1 ); // Test on EV5 and clear it

  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Transmitter );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, -2 );

  i2c->DR = ds;
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED, -4 );
  n_trans = 1;

  end: ;
  genSTOP();

  return ( err == 0 ) ? n_trans : err;
}

int  DevI2C::send( uint8_t addr, const uint8_t *ds, int ns )
{
  __label__ end;
  err = 0; n_trans = 0;

  I2C_WAITNOBUSY( i2c );

  genSTART();
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -1 ); // Test on EV5 and clear it

  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Transmitter );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, -2 );

  for( n_trans=0; n_trans<ns; ++n_trans ) {
    i2c->DR = *ds++ ;
    // I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED, -4 );
    I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING, -4 );
  }

  end: ;
  genSTOP();

  return ( err == 0 ) ? n_trans : err;
}

int  DevI2C::send_reg1( uint8_t addr, uint8_t reg,  const uint8_t *ds, int ns )
{
  __label__ end;
  err = 0; n_trans = 0;

  I2C_WAITNOBUSY( i2c );

  genSTART();
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -1 ); // Test on EV5 and clear it

  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Transmitter );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, -2 );

  i2c->DR = reg;
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING, -4 );

  for( n_trans=0; n_trans<ns; ++n_trans ) {
    i2c->DR = *ds++ ;
    I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING, -4 );
  }

  end: ;
  genSTOP();

  return ( err == 0 ) ? n_trans : err;
}

int  DevI2C::send_reg2( uint8_t addr, uint16_t reg, const uint8_t *ds, int ns )
{
  __label__ end;
  err = 0; n_trans = 0;

  I2C_WAITNOBUSY( i2c );

  genSTART();
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -1 ); // Test on EV5 and clear it

  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Transmitter );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, -2 );

  i2c->DR = (uint8_t)(reg>>8);
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING, -4 );
  i2c->DR = (uint8_t)(reg);
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING, -4 );

  for( n_trans=0; n_trans<ns; ++n_trans ) {
    i2c->DR = *ds++ ;
    I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING, -4 );
  }

  end: ;
  genSTOP();

  return ( err == 0 ) ? n_trans : err;
}


int  DevI2C::recv( uint8_t addr )
{
  __label__ end;
  err = 0; n_trans = 0;
  uint8_t v;

  I2C_WAITNOBUSY( i2c );

  genSTART();
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -1 ); // Test on EV5 and clear it

  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Receiver );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, -3 );

  asknDisable();

  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_RECEIVED, -3 );
  v = (uint8_t)( i2c->DR );
  n_trans = 1;

  end: ;
  asknEnable();
  genSTOP();

  return ( err == 0 ) ? v : err;

}

int  DevI2C::recv( uint8_t addr, uint8_t *dd, int nd )
{
  __label__ end;
  err = 0; n_trans = 0;

  I2C_WAITNOBUSY( i2c );

  genSTART();
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -1 ); // Test on EV5 and clear it

  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Receiver );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, -3 );


  for( n_trans=0; n_trans<nd; ++n_trans ) {
   I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_RECEIVED, -3 );
   *dd++ = (uint8_t)( i2c->DR );
   if( n_trans >=nd-2 ) {
     asknDisable(); // <-------- last?
   }
  }

  end: ;
  asknEnable();
  genSTOP();

  return ( err == 0 ) ? n_trans : err;

}

int  DevI2C::recv_reg1( uint8_t addr, int8_t reg,  uint8_t *dd, int nd )
{
  __label__ end;
  err = 0; n_trans = 0;

  I2C_WAITNOBUSY( i2c );

  genSTART();
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -1 ); // Test on EV5 and clear it

  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Transmitter );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, -2 );

  i2c->DR = reg;
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING, -4 );

  genSTART(); // restart
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -7 ); // Test on EV5 and clear it


  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Receiver );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, -3 );

  for( n_trans=0; n_trans<nd; ++n_trans ) {
   I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_RECEIVED, -3 );
   *dd++ = (uint8_t)( i2c->DR );
   if( n_trans >=nd-2 ) {
     asknDisable(); // <-------- last?
   }
  }

  end: ;
  asknEnable();
  genSTOP();

  return ( err == 0 ) ? n_trans : err;
}

int  DevI2C::recv_reg2( uint8_t addr, int16_t reg, uint8_t *dd, int nd )
{
  __label__ end;
  err = 0; n_trans = 0;

  I2C_WAITNOBUSY( i2c );

  genSTART();
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -1 ); // Test on EV5 and clear it

  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Transmitter );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, -2 );

  i2c->DR = (uint8_t)(reg>>8);
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING, -4 );
  i2c->DR = (uint8_t)(reg);
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING, -4 );

  genSTART(); // restart
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -7 ); // Test on EV5 and clear it


  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Receiver );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, -3 );

  for( n_trans=0; n_trans<nd; ++n_trans ) {
   I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_RECEIVED, -3 );
   *dd++ = (uint8_t)( i2c->DR );
   if( n_trans >=nd-2 ) {
     asknDisable(); // <-------- last?
   }
  }

  end: ;
  asknEnable();
  genSTOP();

  return ( err == 0 ) ? n_trans : err;
}

int  DevI2C::send_recv( uint8_t addr, const uint8_t *ds, int ns, uint8_t *dd, int nd )
{
  __label__ end;
  err = 0; n_trans = 0;

  I2C_WAITNOBUSY( i2c );

  genSTART();
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -1 ); // Test on EV5 and clear it

  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Transmitter );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED, -2 );

  for( n_trans=0; n_trans<ns; ++n_trans ) {
    i2c->DR = *ds++;
    // I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED, -4 );
    I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTING, -4 );
  }

  genSTART(); // restart
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_MODE_SELECT, -7 ); // Test on EV5 and clear it


  I2C_Send7bitAddress( i2c, addr<<1, I2C_Direction_Receiver );
  I2C_WAITFOR( i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED, -3 );

  for( n_trans=0; n_trans<nd; ++n_trans ) {
   I2C_WAITFOR( i2c, I2C_EVENT_MASTER_BYTE_RECEIVED, -3 );
   *dd++ = (uint8_t)( i2c->DR );
   if( n_trans >=nd-2 ) {
     asknDisable(); // <-------- last?
   }
  }

  end: ;
  asknEnable();
  genSTOP();

  return ( err == 0 ) ? n_trans : err;
}

