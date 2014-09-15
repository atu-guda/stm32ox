#include <ox_gpio.h>
#include <board_stm32f103rb_ek.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

BOARD_DEFINE_LEDS;
const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

extern "C" {

void task_leds( void *prm UNUSED );

}


int main(void)
{
  leds.initHW();
  NVIC_SetPriorityGrouping( NVIC_PriorityGroup_4 );

  leds.write( 0x03 );  delay_bad_ms( 200 );

  xTaskCreate( task_leds, "leds", 2*def_stksz, 0, 1, 0 );

  leds.write( 0x02 );  delay_bad_ms( 200 );

  vTaskStartScheduler();
  die4led( 0xFF );

  return 0;
}

void task_leds( void *prm UNUSED )
{
  int i=0;
  while(1)
  {
    leds.write( i );
    ++i;
    i &= 0x0F;
    delay_ms( 1000 );
  }
}

void _exit( int rc UNUSED )
{
}


// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc

