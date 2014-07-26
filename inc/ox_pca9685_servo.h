#ifndef _OX_PCA9685_SERVO_H
#define _OX_PCA9685_SERVO_H

#include <ox_i2c.h>

class PCA9685_servo {
  public:
   enum {
     addr_def  = 0x40,
     reg_mode0 = 0x00,
     reg_mode1 = 0x01,
     reg_led0s = 0x06, // 6 = on_low, 7 = on_high, 8 = off_low, 9 = off_high+
     reg_all_s = 0xFA, // FA-FD
     reg_prescale = 0xFE, // 121 = 50Hz 30 - 200 Hz
     bit_onoff = 0x10, // on and off bits on *_high
     mask_high = 0x0F, // value bits in *_high
     cmd_on_inc = 0x20, // to mode0
     cmd_off = 0x10,
     presc_50Hz = 121, // prescale for 50Hz
     presc_100Hz = 60,
     presc_24Hz = 253,
     presc_def = presc_50Hz,
     v_min = 110, // only on 50Hz!
     v_max = 430,
     v_mid = (v_min+v_max)/2,
     v_dlt = (v_max-v_min),
     v_in_max = 1000,
     v_out_mask = 0x0FFF, // 12bit : most used 0x01FF
     n_ch = 16
   };
   PCA9685_servo( DevI2C &d_i2c, uint8_t d_addr = addr_def, uint8_t a_presc = presc_def )
     : i2c(d_i2c), addr(d_addr), presc(a_presc)  {}
   void init();
   void sleep();
   void set( uint8_t ch, int val ); // value is signed!
   void off( uint8_t ch ); // to on - use set!
   void setAll( int val );
   void offAll();
   void setGrp( uint16_t grp, int val );
   void offGrp( uint16_t grp );

   static uint8_t ch2reg( uint8_t ch_idx ) { return reg_led0s + 4*ch_idx + 2; } // 2: reg(off)
   static uint16_t v2t( int v ); // converts value to 'off' time
  protected:
   DevI2C &i2c;
   uint8_t addr;
   uint8_t presc;
};

#endif

