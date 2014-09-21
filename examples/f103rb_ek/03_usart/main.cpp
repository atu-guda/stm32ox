#include <ox_gpio.h>
#include <ox_usart.h>
#include <board_stm32f103rb_ek.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

BOARD_DEFINE_LEDS;
const int def_stksz = configMINIMAL_STACK_SIZE;

extern "C" {

void task_leds( void *prm UNUSED );
void task_string_send( void *prm UNUSED );

};

Usart usa( &USART1Conf1, &UsartModeAsync, 115200 );
char global_cr = ' ';
void on_received_char( char c );

int main(void)
{
  leds.initHW();

  NVIC_SetPriorityGrouping( NVIC_PriorityGroup_4 );

  leds.write( 0x00 );  delay_bad_ms( 500 );
  leds.write( 0x02 );  delay_bad_ms( 200 );

  xTaskCreate( task_leds,        "leds", def_stksz, 0, 1, 0 );
  xTaskCreate( task_usart1_send, "send", def_stksz, 0, 1, 0 );
  xTaskCreate( task_usart1_recv, "recv", def_stksz, 0, 1, 0 );
  xTaskCreate( task_string_send, "ss",   def_stksz, 0, 1, 0 );

  // leds.write( 0x03 );  delay_bad_ms( 500 );

  usa.initIRQ( configKERNEL_INTERRUPT_PRIORITY, 0 );
  usa.initHW();
  usa.init();
  usa.itConfig( USART_IT_RXNE, ENABLE );
  usa.setOnRecv( on_received_char );
  usa.enable();

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
  char s[] = "U1< ; > \r\n";
  uint8_t i = ' ';
  while(1) {
    leds.toggle( BIT0 );
    s[3] = i;
    s[5] = global_cr;
    usa.sendStr( s );
    delay_ms( 200 );
    ++i;
    if( i > 127 ) {
       i = ' ';
    }
  }
  vTaskDelete(0);
}

STD_USART1_SEND_TASK( usa );
STD_USART1_RECV_TASK( usa );
STD_USART1_IRQ( usa );

void on_received_char( char c )
{
  global_cr = c;
  // leds.toggle( BIT1 );
}

void _exit( int rc UNUSED )
{
}


// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc

