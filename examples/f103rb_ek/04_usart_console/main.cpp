#include <cstring>
#include <cstdlib>

#include <ox_gpio.h>
#include <ox_usart.h>
#include <ox_console.h>
#include <ox_debug1.h>
#include <ox_smallrl.h>
#include <ox_smallrl_q.h>
#include <board_stm32f103rb_ek.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

using namespace std;
using namespace SMLRL;

BOARD_DEFINE_LEDS;
const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

// SmallRL storage and config
int smallrl_print( const char *s, int l );
int smallrl_exec( const char *s, int l );
void smallrl_sigint(void);
QueueHandle_t smallrl_cmd_queue;

// SmallRL srl( smallrl_print, smallrl_exec );
SmallRL srl( smallrl_print, exec_queue );

// --- local commands;
int cmd_test0( int argc, const char * const * argv );
CmdInfo CMDINFO_TEST0 { "test0", 'T', cmd_test0, " - test something 0"  };

int idle_flag = 0;

const CmdInfo* global_cmds[] = {
  DEBUG_CMDS,

  &CMDINFO_TEST0,
  nullptr
};



extern "C" {

void task_main( void *prm UNUSED );
void task_leds( void *prm UNUSED );

}

Usart usa( &USART1Conf1, &UsartModeAsync, 115200 );
void on_received_char( char c );

int main(void)
{
  leds.initHW();

  NVIC_SetPriorityGrouping( NVIC_PriorityGroup_4 );

  leds.write( 0x00 );  delay_bad_ms( 500 );
  leds.write( 0x02 );  delay_bad_ms( 200 );

  global_smallrl = &srl;
  SMALLRL_INIT_QUEUE;


  //           fun         name    stack_sz   prm prty *handle
  xTaskCreate( task_usart1_send, "send", def_stksz, 0, 1, 0 );
  xTaskCreate( task_usart1_recv, "recv", def_stksz, 0, 1, 0 );
  xTaskCreate( task_main, "main", def_stksz, 0, 1, 0 );
  leds.write( 0x00 );  delay_bad_ms( 500 );
  xTaskCreate( task_smallrl_cmd, "smallrl_cmd", def_stksz, 0, 1, 0 );
  xTaskCreate( task_leds,        "leds", 2*def_stksz, 0, 1, 0 );

  // leds.write( 0x03 );  delay_bad_ms( 500 );

  usa.initIRQ( configKERNEL_INTERRUPT_PRIORITY, 0 );
  usa.initHW();
  usa.init();
  usa.itConfig( USART_IT_RXNE, ENABLE );
  usa.setOnRecv( on_received_char );
  usa.enable();
  // leds.write( 0x01 );  delay_bad_ms( 500 );

  vTaskStartScheduler();
  die4led( 0xFF );

  return 0;
}

void task_main( void *prm UNUSED ) // TMAIN
{
  uint32_t nl = 0;

  // i2c_d.init();
  pr( "***** Main loop: ****** " NL );

  srl.setSigFun( smallrl_sigint );
  srl.set_ps1( "\033[32m#\033[0m ", 2 );
  srl.re_ps();


  idle_flag = 1;
  while (1) {
    ++nl;
    if( idle_flag == 0 ) {
      pr_sd( ".. main idle  ", nl );
      srl.redraw();
    }
    idle_flag = 0;
    delay_ms( 60000 );

  }
  vTaskDelete(NULL);
}


void task_leds( void *prm UNUSED )
{
  while(1) {
    leds.toggle( BIT1 );
    delay_ms( 500 );
  }
  vTaskDelete(0);
}


STD_USART1_SEND_TASK( usa );
STD_USART1_RECV_TASK( usa );
STD_USART1_IRQ( usa );


int pr( const char *s )
{
  if( !s || !*s ) {
    return 0;
  }
  prl( s, strlen(s) );
  return 0;
}

int prl( const char *s, int l )
{
  usa.sendBlock( s, l );
  idle_flag = 1;
  return 0;
}
// ---------------------------- smallrl -----------------------


int smallrl_print( const char *s, int l )
{
  prl( s, l );
  return 1;
}

int smallrl_exec( const char *s, int l )
{
  pr( NL "Cmd: \"" );
  prl( s, l );
  pr( "\" " NL );
  exec_direct( s, l );
  return 1;
}

// will be called by real receiver: USART, USB...
void on_received_char( char c )
{
  srl.addChar( c );
  idle_flag = 1;
}

void smallrl_sigint(void)
{
  leds.toggle( BIT1 );
}

void _exit( int rc )
{
  die4led( rc & 0x0F );
}


// TEST0
int cmd_test0( int argc, const char * const * argv )
{
  int a1 = 0;
  if( argc > 1 ) {
    a1 = strtol( argv[1], 0, 0 );
  }
  pr( NL "Test0: a1= " ); pr_d( a1 ); pr( NL );
  // log_add( "Test0 " );
  //
  uint32_t cr1 = usa.getCR1();
  pr_shx( cr1 );
  uint32_t sr = usa.getSR();
  pr_shx( sr );
  delay_ms( 100 );

  usa.sendStr( "USART 2 String " );
  for( int i=0; i< a1; ++i ) {
    usa.sendStr( " 12345" );
    usa.sendStr( "67890 ***" NL );
  }
  pr( NL );

  delay_ms( 10 );

  pr( NL "test0 end." NL );
  return 0;
}


// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc

