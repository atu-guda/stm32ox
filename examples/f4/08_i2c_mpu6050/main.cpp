#include <string.h>
#include <stdlib.h>

#include <ox_gpio.h>
#include <ox_usbotgfs.h>
#include <ox_i2c.h>
#include <usbd_desc.h>
#include <usbd_cdc_vcp.h>

#include <ox_console.h>
#include <ox_debug1.h>
#include <ox_debug_i2c.h>
#include <board_stm32f407_atu_x2.h>

#include <ox_bmp085.h>

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
// --- I2C commands;
int cmd_i2c_scan( int argc, const char * const * argv );
int cmd_i2c_send( int argc, const char * const * argv );
int cmd_i2c_send_r1( int argc, const char * const * argv );
int cmd_i2c_send_r2( int argc, const char * const * argv );
int cmd_i2c_recv( int argc, const char * const * argv );
int cmd_i2c_recv_r1( int argc, const char * const * argv );
int cmd_i2c_recv_r2( int argc, const char * const * argv );

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
  { "scan",  'C', cmd_i2c_scan,   "[start [end]] - scan I2C in range" },
  { "send",  'S', cmd_i2c_send,   "val [addr] - send to I2C (def addr=var[p])" },
  { "send1",  0,  cmd_i2c_send_r1, "reg val  send to I2C, reg addr 1B (addr=var[p])" },
  { "send2",  0,  cmd_i2c_send_r2, "reg val  send to I2C, reg addr 2B (addr=var[p])" },
  { "recv",  'R', cmd_i2c_recv,    "[addr] - recv from I2C (def addr=var[p])" },
  { "recv1",  0,  cmd_i2c_recv_r1, "reg [n] - recv from I2C, reg addr 1B (addr=var[p])" },
  { "recv2",  0,  cmd_i2c_recv_r2, "reg [n] - recv from I2C, reg addr 2B (addr=var[p])" },
  { 0, 0, 0, 0 }
};


extern "C" {

void task_main( void *prm UNUSED );
void task_leds( void *prm UNUSED );

}

UsbOtgFs usbotg( &UsbOtgFsConf1, &UsbOtgFsModeVbus );
UsbOtgFs *usbfs_main = &usbotg;

void on_received_char( char c );

DevI2C i2c_d( &I2C1Conf1, &I2CModeI2C_Master );
DevI2C& i2c_dbg  = i2c_d;

int main(void)
{
  leds.initHW();

  NVIC_SetPriorityGrouping( NVIC_PriorityGroup_4 );

  leds.write( 0x0F );  delay_bad_ms( 200 );
  leds.write( 0x0A );  delay_bad_ms( 200 );
  leds.reset( 0x0F );  delay_bad_ms( 200 );

  usbotg.setOnRecv( on_received_char );

  //           fun         name    stack_sz   prm prty *handle
  xTaskCreate( task_leds, "leds", 2*def_stksz, 0, 1, 0 );
  xTaskCreate( task_usbotgfscdc_recv, "urecv", 2*def_stksz, 0, 2, 0 );
  xTaskCreate( task_main, "main", 2 * def_stksz, 0, 1, 0 );

  // usbotg.initHW(); // not req - by callback USB_OTG_BSP_Init

  vTaskStartScheduler();
  die4led( 0xFF );

  return 0;
}

void task_main( void *prm UNUSED ) // TMAIN
{
  uint32_t nl = 0;

  usbotg.init( &USR_desc, &USBD_CDC_cb, &USR_cb );

  i2c_d.initHW();
  i2c_d.init();

  pr( "***** Main loop: ****** " NL );
  microrl_init( &mrl, microrl_print );
  microrl_set_execute_callback( &mrl, microrl_exec );
  // microrl_set_complete_callback( &mrl, microrl_get_completion );
  microrl_set_sigint_callback( &mrl, microrl_sigint );

  user_vars['t'-'a'] = 1000; // dly
  user_vars['n'-'a'] = 20;   // ncy

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

  int dly = user_vars['t'-'a'];
  int ncy = user_vars['n'-'a'];

  i2c_d.init();
  pr( " * I2C after init " NL );
  i2c_print_status( i2c_d );

  BMP085 baro( i2c_d );
  baro.readCalibrData();

  for( int i=0; i<ncy; ++i ) {

    baro.getAllCalc( 3 );
    int t10 = baro.get_T10();
    int p   = baro.get_P();
    // int t_u = baro.get_T_uncons();
    // int p_u = baro.get_P_uncons();
    pr( "T= " ); pr_d( t10 ); pr( "  P= " ); pr_d( p ); pr( NL );
    delay_ms( dly );
  }

  // const int16_t *cal = baro.getCalibr();
  // for( int i=0; i< BMP085::n_calibr_data; ++i ) {
  //   pr_d( i ); pr( "  " ); pr_d( cal[i] ); pr(NL);
  // }


  delay_ms( 10 );

  pr( NL "test0 end." NL );
  return 0;
}


// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc,ox/inc/usb_cdc
