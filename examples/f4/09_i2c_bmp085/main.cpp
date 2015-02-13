#include <cstring>
#include <cstdlib>

#include <ox_gpio.h>
#include <ox_usbotgfs.h>
#include <ox_i2c.h>
#include <usbd_desc.h>
#include <usbd_cdc_vcp.h>

#include <ox_console.h>
#include <ox_debug1.h>
#include <ox_smallrl.h>
#include <ox_smallrl_q.h>
#include <ox_debug_i2c.h>
#include <board_stm32f407_atu_x2.h>

#include <ox_bmp085.h>

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

int idle_flag = 0;

// --- local commands;
int cmd_test0( int argc, const char * const * argv );
CmdInfo CMDINFO_TEST0 { "test0", 'T', cmd_test0, " - test something 0"  };

const CmdInfo* global_cmds[] = {
  DEBUG_CMDS,
  DEBUG_I2C_CMDS,

  &CMDINFO_TEST0,
  nullptr
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

int brk = 0;

int main(void)
{
  leds.initHW();

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

  i2c_d.initHW();
  i2c_d.init();

  pr( "***** Main loop: ****** " NL );

  user_vars['t'-'a'] = 1000; // dly
  user_vars['n'-'a'] = 20;   // ncy

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
  brk = 1;
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

  brk = 0;

  for( int i=0; i<ncy && !brk ; ++i ) {

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
