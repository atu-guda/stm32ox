#include <cstring>
#include <cstdlib>

#include <ox_gpio.h>
#include <ox_usbotgfs.h>
#include <usbd_desc.h>
#include <usbd_cdc_vcp.h>

#include <ox_console.h>
#include <ox_debug1.h>
#include <ox_smallrl.h>
#include <ox_smallrl_q.h>
#include <board_stm32f407_atu_x2.h>

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

void task_motor( void *prm UNUSED );

}

UsbOtgFs usbotg( &UsbOtgFsConf1, &UsbOtgFsModeVbus );
UsbOtgFs *usbfs_main = &usbotg;

void on_received_char( char c );

PinsOut motor( &gpio_e, 0, 4 ); // E0-E3

int main(void)
{
  leds.initHW();
  motor.initHW();

  user_vars['t'-'a'] = 100; // step time
  user_vars['d'-'a'] = 1;   // direction

  NVIC_SetPriorityGrouping( NVIC_PriorityGroup_4 );

  leds.write( 0x0F );  delay_bad_ms( 200 );
  leds.write( 0x0A );  delay_bad_ms( 200 );
  leds.reset( 0x0F );  delay_bad_ms( 200 );

  global_smallrl = &srl;
  SMALLRL_INIT_QUEUE;

  usbotg.setOnRecv( on_received_char );

  //           fun         name    stack_sz   prm prty *handle
  xTaskCreate( task_leds, "leds", 2*def_stksz, 0, 1, 0 );
  xTaskCreate( task_usbotgfscdc_recv, "urecv", 2*def_stksz, 0, 2, 0 );
  xTaskCreate( task_main, "main", 2 * def_stksz, 0, 1, 0 );
  xTaskCreate( task_smallrl_cmd, "smallrl_cmd", def_stksz, 0, 1, 0 );
  xTaskCreate( task_motor, "motor", def_stksz, 0, 1, 0 );

  // usbotg.initHW(); // not req - by callback USB_OTG_BSP_Init

  vTaskStartScheduler();
  die4led( 0xFF );

  return 0;
}

void task_main( void *prm UNUSED ) // TMAIN
{
  uint32_t nl = 0;

  usbotg.init( &USR_desc, &USBD_CDC_cb, &USR_cb );
  // i2c_d.init();
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


STD_OTG_FS_IRQ( usbotg );
STD_OTG_FS_RECV_TASK( usbotg );


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
  // us2.sendBlock( s, l );
  usbotg.sendBlock( s, l );
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
  leds.toggle( BIT3 );
  user_vars['o'-'a'] = 0;
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

  int prty = uxTaskPriorityGet( 0 );
  pr_sdx( prty );
  const char *nm = pcTaskGetTaskName( 0 );
  pr( "name: \"" ); pr( nm ); pr( "\"" NL );


  // log_add( "Test0 " );


  pr( NL "delay start..." );
  delay_ms( 5000 );
  pr( NL "...delay end" NL );

  pr( NL "test0 end." NL );
  return 0;
}

void task_motor( void *prm UNUSED )
{
  static const uint8_t hsteps[] = { 1, 3, 2, 6, 4, 12, 8, 9 };
  int i=0;
  while(1) {
    if( user_vars['o'-'a'] ) { // run
      motor.write( hsteps[i] );
      delay_ms( user_vars['t'-'a'] );
      i += user_vars['d'-'a'];
      if( i < 0 ) { i = 7; }
      if( i >= 8 ) { i = 0; }
    } else { // stop
      motor.write( 0 );
      delay_ms( 1000 );
    }
  };
}

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc,ox/inc/usb_cdc
