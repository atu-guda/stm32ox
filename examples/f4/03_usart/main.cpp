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
void task_string_send( void *prm UNUSED );

}

Usart us2( &USART2Conf1, &UsartModeAsync, 115200 );
char global_cr = ' ';
void on_received_char( char c );

int main(void)
{
  leds.initHW();

  NVIC_SetPriorityGrouping( NVIC_PriorityGroup_4 );

  leds.write( 0x0F );  delay_bad_ms( 200 );
  leds.write( 0x0A );  delay_bad_ms( 200 );
  leds.reset( 0x0F );  delay_bad_ms( 200 );

  xTaskCreate( task_leds, "leds", 2*def_stksz, 0, 1, 0 );
  xTaskCreate( task_usart2_send, "send", 2*def_stksz, 0, 1, 0 );
  xTaskCreate( task_usart2_recv, "recv", 2*def_stksz, 0, 1, 0 );
  xTaskCreate( task_string_send, "ss",     def_stksz, 0, 1, 0 );

  us2.initIRQ( configKERNEL_INTERRUPT_PRIORITY, 0 );
  us2.initHW();
  us2.init();
  us2.itConfig( USART_IT_RXNE, ENABLE );
  us2.setOnRecv( on_received_char );
  us2.enable();

  vTaskStartScheduler();
  die4led( 0xFF );

  return 0;
}

void task_leds( void *prm UNUSED )
{
  while(1) {
    leds.toggle( BIT1 );
    delay_ms( 500 );
  }
  vTaskDelete(0);
}

void task_string_send( void *prm UNUSED )
{
  char s[] = "U2< ; > \r\n";
  uint8_t i = ' ';
  while(1) {
    s[3] = i;
    s[5] = global_cr;
    us2.sendStr( s );
    delay_ms( 2000 );
    ++i;
    if( i > 127 ) {
       i = ' ';
    }
  }
  vTaskDelete(0);
}

STD_USART2_SEND_TASK( us2 );
STD_USART2_RECV_TASK( us2 );
STD_USART2_IRQ( us2 );

void on_received_char( char c )
{
  global_cr = c;
}

void _exit( int rc UNUSED )
{
}


// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc

