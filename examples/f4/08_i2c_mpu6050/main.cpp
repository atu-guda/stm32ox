#include <string.h>
#include <stdlib.h>

#include <ox_gpio.h>
#include <ox_usbotgfs.h>
#include <ox_i2c.h>
#include <usbd_desc.h>
#include <usbd_cdc_vcp.h>

#include <ox_console.h>
#include <ox_debug1.h>
#include <board_stm32f407_atu_x2.h>
#include <ox_mpu6050.h>

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
void i2c_print_status( DevI2C &i2c_d );

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

  MPU6050 accel( i2c_d );
  int16_t adata[MPU6050::mpu6050_alldata_sz];
  accel.setDLP( MPU6050::DLP_BW::bw_10 );
  accel.init();
  int tick_start = xTaskGetTickCount();
  for( int i=0; i<ncy; ++i ) {
    accel.getAll( adata );
    for( int j=0; j<MPU6050::mpu6050_alldata_sz; ++j ) {
      pr_d( (int)(adata[j]) ); pr( " " );
    }
    pr( NL );
    delay_ms( dly );
  }
  int tick_stop = xTaskGetTickCount();
  tick_stop -= tick_start;
  pr_sdx( tick_stop );


  delay_ms( 10 );

  pr( NL "test0 end." NL );
  return 0;
}


int cmd_i2c_scan( int argc, const char * const * argv )
{
  int n = 0;
  uint8_t addr_start = 2, addr_end = 127;
  long l;
  char *eptr;

  if( argc > 1 ) {
    l = strtol( argv[1], &eptr, 0 );
    if( eptr != argv[1] ) {
      addr_start = (uint8_t)(l);
    }
  }

  if( argc > 2 ) {
    l = strtol( argv[2], &eptr, 0 );
    if( eptr != argv[2] ) {
      addr_end = (uint8_t)(l);
    }
  }

  if( addr_start < 2 ) {
    addr_start = 2;
  }
  if( addr_end <= addr_start ) {
    addr_end = addr_start+1;
  }
  pr( NL "I2C Scan in range [ " ); pr_d( addr_start );
  pr( " ; " ); pr_d( addr_end ); pr( " ] " NL );

  i2c_d.init();
  uint32_t ev;

  for( uint8_t addr=addr_start; addr < addr_end; ++addr ) {
    // init_i2c();
    delay_ms( 10 );
    n = i2c_d.ping( addr );
    ev = i2c_d.getLastEv();
    if( n ) {
      pr_d( addr ); pr( " (0x" ); pr_h( addr ); pr( ") " );
      pr(" : " );      pr_d( n ); pr( NL );
    } else {
      // pr_shx( ev );
    }
  }

  ev  = i2c_d.getEv();
  pr_shx( ev );

  pr( NL "I2C scan end." NL );

  return 0;
}

int cmd_i2c_send( int argc, const char * const * argv )
{
  if( argc < 2 ) {
    pr( "error: value required" NL );
    return 1;
  }
  uint8_t v = 0;
  long l;
  char *eptr;

  l = strtol( argv[1], &eptr, 0 );
  if( eptr != argv[1] ) {
    v = (uint8_t)(l);
  }

  uint8_t addr = (uint8_t)(user_vars['p'-'a']);

  if( argc > 2 ) {
    l = strtol( argv[2], &eptr, 0 );
    if( eptr != argv[2] ) {
      addr = (uint8_t)(l);
    }
  }

  pr( NL "I2C Send  " ); pr_d( v );
  pr( " to " ); pr_h( addr ); pr( NL );

  i2c_d.init();

  int n = i2c_d.send( addr, v );
  pr( "I2C Send Result  " ); pr_d( n ); pr( NL );

  i2c_print_status( i2c_d );

  return 0;
}

int cmd_i2c_send_r1( int argc, const char * const * argv )
{
  if( argc < 3 ) {
    pr( "** ERR: reg val required" NL );
    return -1;
  }
  uint8_t addr = (uint8_t)(user_vars['p'-'a']);
  uint8_t reg = (uint8_t) strtol( argv[1], 0, 0 );
  uint8_t val = (uint8_t) strtol( argv[2], 0, 0 );

  pr( NL "I2C Send  " ); pr_d( val );
  pr( " to " ); pr_h( addr ); pr( ": " ); pr_h( reg ); pr( NL );

  i2c_d.init();

  int n = i2c_d.send_reg1( addr, reg, &val, 1 );
  pr( "I2C Send Result  r1:" ); pr_d( n ); pr( NL );

  i2c_print_status( i2c_d );

  return 0;
}

int cmd_i2c_send_r2( int argc, const char * const * argv )
{
  if( argc < 3 ) {
    pr( "** ERR: reg val required" NL );
    return -1;
  }
  uint8_t  addr = (uint8_t)(user_vars['p'-'a']);
  uint16_t reg = (uint8_t) strtol( argv[1], 0, 0 );
  uint8_t  val = (uint8_t) strtol( argv[2], 0, 0 );

  pr( NL "I2C Send  r2: " ); pr_d( val );
  pr( " to " ); pr_h( addr ); pr( ": " ); pr_h( reg ); pr( NL );

  i2c_d.init();

  int n = i2c_d.send_reg2( addr, reg, &val, 1 );
  pr( "I2C Send Result  " ); pr_d( n ); pr( NL );

  i2c_print_status( i2c_d );

  return 0;
}


int cmd_i2c_recv( int argc, const char * const * argv )
{
  // uint8_t v = 0;
  uint8_t addr = (uint8_t)(user_vars['p'-'a']);
  long l;
  int r, nr = 1;
  char *eptr;

  if( argc > 1 ) {
    l = strtol( argv[1], &eptr, 0 );
    if( eptr != argv[1] ) {
      addr = (uint8_t)(l);
    }
  }

  if( argc > 2 ) {
    l = strtol( argv[2], &eptr, 0 );
    if( eptr != argv[2] ) {
      nr = l;
    }
  }

  pr( NL "I2C Recv from " );  pr_h( addr );

  i2c_d.init();

  if( nr < 2 ) {
    r = i2c_d.recv( addr );
  } else {
    r = i2c_d.recv( addr, (uint8_t*)(gbuf_a), nr );
    dump8( gbuf_a, nr );
  }
  pr( "  Recv Result = " ); pr_d( r );
  pr( " 0x" ); pr_h( r ); pr( NL );

  i2c_print_status( i2c_d );

  return 0;
}

int cmd_i2c_recv_r1( int argc, const char * const * argv )
{
  if( argc < 2 ) {
    pr( "** ERR: reg required" NL );
    return -1;
  }
  uint8_t addr = (uint8_t)(user_vars['p'-'a']);
  uint8_t reg = (uint8_t) strtol( argv[1], 0, 0 );
  uint16_t n = 1;
  uint8_t val = 0;

  if( argc > 2 ) {
    char *eptr;
    long l = strtol( argv[2], &eptr, 0 );
    if( eptr != argv[2] ) {
      n = (uint16_t)( l );
    }
    if( n > sizeof(gbuf_a) ) {
      n = sizeof(gbuf_a);
    }
  }

  pr( NL "I2C recv r1 from  " );
  pr_h( addr ); pr( ":" ); pr_h( reg ); pr( " n= " ); pr_d( n );

  i2c_d.init();

  int r = i2c_d.recv_reg1( addr, reg, (uint8_t*)(gbuf_a), n );
  pr_sdx( r );
  if( n == 1 ) {
    pr_h( val );
  } else {
    dump8( gbuf_a, n );
  }
  pr( NL );
  i2c_print_status( i2c_d );

  return 0;
}

void i2c_print_status( DevI2C &i2c )
{
  int nt, err, ev;
  err = i2c.getErr();    pr_sdx( err );
  nt  = i2c.getNTrans(); pr_sdx( nt );
  ev  = i2c.getLastEv(); pr_shx( ev );
}

int cmd_i2c_recv_r2( int argc, const char * const * argv )
{
  if( argc < 2 ) {
    pr( "** ERR: reg required" NL );
    return -1;
  }
  uint8_t addr = (uint8_t)(user_vars['p'-'a']);
  uint16_t reg  = (uint8_t) strtol( argv[1], 0, 0 );
  uint16_t n = 1;
  uint8_t val = 0;

  if( argc > 2 ) {
    char *eptr;
    long l = strtol( argv[2], &eptr, 0 );
    if( eptr != argv[2] ) {
      n = (uint16_t)( l );
    }
    if( n > sizeof(gbuf_a) ) {
      n = sizeof(gbuf_a);
    }
  }

  pr( NL "I2C recv r2 from  " );
  pr_h( addr ); pr( ":" ); pr_h( reg ); pr( " n= " ); pr_d( n );

  i2c_d.init();

  int r = i2c_d.recv_reg2( addr, reg, (uint8_t*)(gbuf_a), n );
  pr_sdx( r );
  if( n == 1 ) {
    pr_h( val );
  } else {
    dump8( gbuf_a, n );
  }
  pr( NL );
  i2c_print_status( i2c_d );

  return 0;
}



// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc,ox/inc/usb_cdc
