#include <ox_pca9685_servo.h>


void PCA9685_servo::init()
{
  sleep(); // prescale can only be set while sleep
  uint8_t s = presc;
  i2c.send_reg1( addr, reg_prescale, &s, 1 );
  s = cmd_on_inc;
  i2c.send_reg1( addr, reg_mode0, &s, 1 );
}

void PCA9685_servo::sleep()
{
  uint8_t s = cmd_off;
  i2c.send_reg1( addr, reg_mode0, &s, 1 );
}

void PCA9685_servo::set( uint8_t ch, int val )
{
  uint16_t s = v2t( val );
  uint8_t reg = ch2reg( ch );
  i2c.send_reg1( addr, reg, (uint8_t*)&s, 2 );
}

void PCA9685_servo::off( uint8_t ch )
{
  uint8_t reg = ch2reg( ch ) + 1;
  uint8_t s = bit_onoff;
  i2c.send_reg1( addr, reg, &s, 1 );
}

void PCA9685_servo::setAll( int val )
{
  uint16_t s = v2t( val );
  uint8_t reg = reg_all_s + 2;
  i2c.send_reg1( addr, reg, (uint8_t*)&s, 2 );
}

void PCA9685_servo::offAll()
{
  uint8_t reg = reg_all_s + 3;
  uint8_t s = bit_onoff;
  i2c.send_reg1( addr, reg, &s, 1 );
}


void PCA9685_servo::setGrp( uint16_t grp, int val )
{
  uint16_t chm = 1;
  for( uint8_t ch = 0; chm; chm <<= 1, ++ch ) {
    if( grp & chm ) {
      set( ch, val );
    }
  }
}

void PCA9685_servo::offGrp( uint16_t grp )
{
  uint16_t chm = 1;
  for( uint8_t ch = 0; chm; chm <<= 1, ++ch ) {
    if( grp & chm ) {
      off( ch );
    }
  }

}


uint16_t PCA9685_servo::v2t( int v )
{
  uint16_t r = v_mid + v_dlt * v / ( 2 * v_in_max );
  r &= v_out_mask;
  return r;
}

