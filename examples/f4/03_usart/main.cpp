#include <ox_gpio.h>
#include <ox_usart.h>
#include <board_stm32f407_atu_x2.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

BOARD_DEFINE_LEDS;
const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

extern "C" {

void task_leds( void *prm UNUSED );

}

Usart us2( &USART2Conf1, &UsartModeAsync, 115200 );

int main(void)
{
  leds.initHW();
  us2.initHW();

  us2.init();
  us2.enable();

  NVIC_SetPriorityGrouping( NVIC_PriorityGroup_4 );

  leds.write( 0x0F );  delay_bad_ms( 200 );
  leds.write( 0x0A );  delay_bad_ms( 200 );
  leds.reset( 0x0F );  delay_bad_ms( 200 );

  xTaskCreate( task_leds, "leds", 2*def_stksz, 0, 1, 0 );

  vTaskStartScheduler();
  die4led( 0xFF );

  return 0;
}

void task_leds( void *prm UNUSED )
{
  char s[] = "U2< ; > ";
  char se[] = " \r\n";
  char sts[10], crs[10];
  uint8_t i = ' ', ic = ' ';
  uint16_t st, cr;
  while(1) {
    st = us2.getSR();
    cr = us2.getCR1();
    s[3] = i;
    if( st & USART_FLAG_RXNE ) {
      ic = us2.recvRaw();
      s[5] = ic;
    }
    word2hex( st, sts );
    word2hex( cr, crs );
    leds.toggle( BIT1 );
    us2.sendStrLoop( s );
    us2.sendStrLoop( sts );
    us2.sendStrLoop( "  " );
    us2.sendStrLoop( crs );
    us2.sendStrLoop( se );
    delay_ms( 500 );
    ++i;
    if( i > 127 ) {
      i = ' ';
    }
  }
}

void _exit( int rc UNUSED )
{
}

