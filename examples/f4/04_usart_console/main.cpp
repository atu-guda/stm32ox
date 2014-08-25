#include <string.h>
#include <stdlib.h>

#include <ox_gpio.h>
#include <ox_usart.h>
#include <ox_console.h>
#include <ox_debug1.h>
#include <board_stm32f407_atu_x2.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <microrl.h>

BOARD_DEFINE_LEDS;
const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

// microrl storage and config
microrl_t mrl;
void microrl_print( const char *s );
int  microrl_exec( int argc, const char * const * argv );
// char ** microrl_get_completion(int argc, const char * const * argv );
void microrl_sigint(void);
// --- local commands;
int cmd_log_print( int argc, const char * const * argv );
int cmd_log_reset( int argc, const char * const * argv );
int cmd_test0( int argc, const char * const * argv );

int idle_flag = 0;

CmdInfo global_cmds[] = {
  { "help",  'h', cmd_help,       " - List of commands and arguments" },
  { "info",  'i', cmd_info,       " - Output general info" },
  { "dump",  'd', cmd_dump,       " {a|b|addr} [n] - HexDumps given area" },
  { "fill",  'f', cmd_fill,       " {a|b|addr} val [n] - Fills memory by value" },
  { "echo",  'e', cmd_echo,       " [args] - output args" },
  { "reboot", 0,  cmd_reboot,     " reboot system" },
  { "die",    0,  cmd_die,        " [val] - die with value" },
  { "lp",     0,  cmd_log_print,  "  - print log buffer" },
  { "lr",     0,  cmd_log_reset,  "  - reset log buffer" },
  { "test0", 'T', cmd_test0,      " - test something 0" },
  { "print", 'p', cmd_pvar,       "name - print user var a-z" },
  { "set",   's', cmd_svar,       "name value - set var a-z" },
  { 0, 0, 0, 0 }
};


extern "C" {

void task_main( void *prm UNUSED );
void task_leds( void *prm UNUSED );

}

Usart us2( &USART2Conf1, &UsartModeAsync, 115200 );
void on_received_char( char c );

int main(void)
{
  leds.initHW();

  NVIC_SetPriorityGrouping( NVIC_PriorityGroup_4 );

  leds.write( 0x0F );  delay_bad_ms( 200 );
  leds.write( 0x0A );  delay_bad_ms( 200 );
  leds.reset( 0x0F );  delay_bad_ms( 200 );

  //           fun         name    stack_sz   prm prty *handle
  xTaskCreate( task_leds, "leds", 2*def_stksz, 0, 1, 0 );
  xTaskCreate( task_usart2_send, "send", 2*def_stksz, 0, 2, 0 );
  xTaskCreate( task_usart2_recv, "recv", 2*def_stksz, 0, 2, 0 );
  xTaskCreate( task_main, "main", 2 * def_stksz, 0, 1, 0 );

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

void task_main( void *prm UNUSED ) // TMAIN
{
  uint32_t nl = 0;

  // i2c_d.init();

  pr( "***** Main loop: ****** " NL );
  microrl_init( &mrl, microrl_print );
  microrl_set_execute_callback( &mrl, microrl_exec );
  // microrl_set_complete_callback( &mrl, microrl_get_completion );
  microrl_set_sigint_callback( &mrl, microrl_sigint );

  idle_flag = 1;
  while (1) {
    ++nl;
    if( idle_flag == 0 ) {
      pr_sd( ".. main idle  ", nl );
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


STD_USART2_SEND_TASK( us2 );
STD_USART2_RECV_TASK( us2 );
STD_USART2_IRQ( us2 );


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
  // VCP_DataTx( (const uint8_t*)(s), l );
  us2.sendBlock( s, l );
  idle_flag = 1;
  return 0;
}
// ---------------------------- microrl -----------------------

void microrl_print( const char *s )
{
  pr( s );
}

// will be called by real receiver: USART, USB...
void on_received_char( char c )
{
  microrl_insert_char( &mrl, c );
  idle_flag = 1;
}

int  microrl_exec( int argc, const char * const * argv )
{
  return default_microrl_exec( argc, argv );
}

// char ** microrl_get_completion(int argc, const char * const * argv );

void microrl_sigint(void)
{
  leds.toggle( BIT3 );
}

int cmd_log_print( int argc UNUSED, const char * const * argv UNUSED )
{
  log_print();
  return 0;
}

int cmd_log_reset( int argc UNUSED, const char * const * argv UNUSED )
{
  log_reset();
  return 0;
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
  uint32_t cr1 = us2.getCR1();
  pr_shx( cr1 );
  uint32_t sr = us2.getSR();
  pr_shx( sr );
  delay_ms( 100 );

  us2.sendStr( "USART 2 String " );
  for( int i=0; i< a1; ++i ) {
    us2.sendStr( " 12345" );
    us2.sendStr( "67890 ***" NL );
  }
  pr( NL );

  delay_ms( 10 );

  pr( NL "test0 end." NL );
  return 0;
}


// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc

