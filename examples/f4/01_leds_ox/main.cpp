#include <ox_gpio.h>
#include <board_stm32f407_atu_x2.h>

BOARD_DEFINE_LEDS;
PinsOut po1( &gpio_d, 12, 4 );


int main(void)
{
  leds.initHW();
  po1.initHW();

  int i=0;
  while (1)
  {
    leds.write( ~i );
    po1.write( i );
    ++i;
    i &= 0x0F;
    delay_bad_ms( 1000 );
  }
  return 0;
}

void _exit( int rc UNUSED )
{
}


// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc

