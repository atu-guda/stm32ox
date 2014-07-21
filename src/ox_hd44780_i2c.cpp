#include <ox_hd44780_i2c.h>

void HD44780_i2c::init_4b( bool is_2row )
{
  delay_ms( 40 );
  strobe( lcd_8b_mode );  delay_ms( 4 );
  strobe( lcd_8b_mode );  delay_ms( 4 );
  strobe( lcd_8b_mode );  delay_ms( 4 );
  strobe( lcd_4b_mode | ( is_2row ? lcd_mode_2row : 0 ) );  delay_ms( 4 );
  cls();
  on();
}

void HD44780_i2c::wr4( uint8_t v, bool is_data )
{
  uint8_t b = ( is_data ) ? lcd_bit_rs : 0;
  b |= led_state;
  uint8_t v1 = ( v & 0xF0 );
  uint8_t d = v1 | lcd_bit_e | b;
  i2c.send( addr, d );
  delay_mcs( 1 );
  d = v1 | led_state | b;
  i2c.send( addr, d );
  delay_mcs( 1 );
  v1 = v << 4;
  d = v1 | lcd_bit_e | b;
  i2c.send( addr, d );
  delay_mcs( 1 );
  d = v1 | b;
  i2c.send( addr, d );
}

void HD44780_i2c::strobe( uint8_t v )
{
  uint8_t d = v | lcd_bit_e | led_state;
  i2c.send( addr, d );
  delay_mcs( 2 );
  d = v | led_state;
  i2c.send( addr, d );
}

void HD44780_i2c::puts( const char *s )
{
  if( !s ) {
    return;
  }
  for( ; *s; ++s ) {
    putc( *s );
  };
}

void HD44780_i2c::gotoxy( uint8_t x, uint8_t y )
{
  uint8_t ofs = y * cpl + x;
  ofs |= 0x80; // cmd
  cmd( ofs );

}

