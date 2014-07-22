#include <ox_mpu6050.h>

uint16_t MPU6050::getReg( uint8_t reg )
{
  uint32_t v;
  i2c.recv_reg1( addr, reg, (uint8_t*)(&v), 2 );
  v = __REV16( v ); // swap bytes in 16-bits
  return (uint16_t)(v);
}

void MPU6050::getRegs( uint8_t reg1, uint8_t n, uint16_t *data )
{
  i2c.recv_reg1( addr, reg1, (uint8_t*)(data), 2*n );
  for( uint8_t i=0; i<n; ++i ) { // swap
    uint32_t v = data[i];
    v = __REV16( v );
    data[i] = (uint16_t)(v);
  }
}

void MPU6050::init()
{
  setAccScale( accs_2g );
  setGyroScale( gyros_250 );
  wake( pll_gyro_x );
}
