#ifndef _OX_HD44780_I2C_H
#define _OX_HD44780_I2C_H

#include <ox_i2c.h>

class HD44780_i2c {
  public:
    enum {
      lcd_8b_mode   = 0x30,
      lcd_4b_mode   = 0x20,
      lcd_mode_2row = 0x08,
      lcd_cmd_cls   = 0x01,
      lcd_cmd_home  = 0x02,
      lcd_cmd_onoff = 0x08,
      lcd_cmd_bit_on= 0x04,
      lcd_cmd_cur_on= 0x02,
      lcd_cmd_blk_on= 0x01,
      lcd_bit_rs    = 0x01,
      lcd_bit_rw    = 0x02, // unused, set to 0 always
      lcd_bit_e     = 0x04,
      lcd_bit_led   = 0x08,
      lcd_def_addr  = 0x27
    };
    HD44780_i2c( DevI2C &i2c_d, uint8_t d_addr = lcd_def_addr )
     : i2c( i2c_d ), addr( d_addr ) {};
    void init_4b( bool is_2row = true );
    void wr4( uint8_t v, bool is_data );
    void strobe( uint8_t v );
    void putc( char c ) { wr4( c, true ); }
    void cmd( uint8_t cmd ) { wr4( cmd, false ); }
    void puts( const char *s );
    void gotoxy( uint8_t x, uint8_t y );
    void cls()   { cmd( lcd_cmd_cls );  delay_ms( 4 ); }
    void home()  { cmd( lcd_cmd_home ); delay_ms( 2 ); }
    void on()       { mod |=  lcd_cmd_bit_on; cmd( mod | lcd_cmd_onoff ); }
    void off()      { mod &= ~lcd_cmd_bit_on; cmd( mod | lcd_cmd_onoff ); }
    void curs_on()  { mod |=  lcd_cmd_cur_on; cmd( mod | lcd_cmd_onoff ); }
    void curs_off() { mod &= ~lcd_cmd_cur_on; cmd( mod | lcd_cmd_onoff ); }
    void led_on()   { led_state = lcd_bit_led; cmd( mod | lcd_cmd_onoff ); }
    void led_off()  { led_state = 0;           cmd( mod | lcd_cmd_onoff ); }

  protected:
   DevI2C &i2c;
   uint8_t addr;
   uint8_t mod = 0;
   uint8_t led_state = lcd_bit_led;
   const uint8_t cpl = 40; // 40 char/linr
};


#endif
