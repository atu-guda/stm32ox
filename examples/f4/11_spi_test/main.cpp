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

#include <ox_spi.h>
#include <ox_spimem_at.h>

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

int cmd_spi_info( int argc, const char * const * argv );
CmdInfo CMDINFO_SPI_INFO { "spii", 'I', cmd_spi_info,  "- Outputs SPI regs" };

int cmd_spi_send1( int argc, const char * const * argv );
CmdInfo CMDINFO_SPI_SEND1 { "spi_s1", 0, cmd_spi_send1,  "- send value to SPI, show result" };

int cmd_spi_send1x( int argc, const char * const * argv );
CmdInfo CMDINFO_SPI_SEND1X { "spi_s1x", 0, cmd_spi_send1x,  "- send value to SPI, w/o result" };

int cmd_spi_send1rN( int argc, const char * const * argv );
CmdInfo CMDINFO_SPI_SEND1RN { "spi_s1rn", 0, cmd_spi_send1rN,  " - send value to SPI, get N to gbuf_b" };

int cmd_spi_duplexN( int argc, const char * const * argv );
CmdInfo CMDINFO_SPI_DUPLEXN { "spi_dn", 0, cmd_spi_duplexN,  " - send N from gbuf_a, recv to gbuf_b" };

int idle_flag = 0;

const CmdInfo* global_cmds[] = {
  DEBUG_CMDS,

  &CMDINFO_TEST0,
  &CMDINFO_SPI_INFO,
  &CMDINFO_SPI_SEND1,
  &CMDINFO_SPI_SEND1X,
  &CMDINFO_SPI_SEND1RN,
  &CMDINFO_SPI_DUPLEXN,
  nullptr
};



extern "C" {

void task_main( void *prm UNUSED );
void task_leds( void *prm UNUSED );

}

UsbOtgFs usbotg( &UsbOtgFsConf1, &UsbOtgFsModeVbus );
UsbOtgFs *usbfs_main = &usbotg;

void on_received_char( char c );


int brk = 0;

using SCF = DevSPI::CFG;
DevSPI spi1( &SPI1Conf1, &SPIMode_Duplex_Master_NSS_Soft,
      SCF::L2_DUPLEX | SCF::DS_8b | SCF::CPOL_LOW | SCF::CHPAL_1E
    | SCF::NSS_SOFT | SCF::MSB_FIRST | SCF::BRP_2 ); // SCF::BRP_256

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
  delay_ms( 10 );
  // spi1.set_inv_nss( true );
  spi1.initHW();
  spi1.init();
  spi1.enable();

  pr( NL "**** " PROJ_NAME NL ); // may be not seed if connected late
  pr( NL " ***** Main loop: ****** " NL NL );
  delay_ms( 10 );

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

  int prty = uxTaskPriorityGet( 0 );
  pr_sdx( prty );
  const char *nm = pcTaskGetTaskName( 0 );
  pr( "name: \"" ); pr( nm ); pr( "\"" NL );

  // log_add( "Test0 " );
  DevSPIMem_AT smem( spi1 );
  // pr( "Chip erase..." );
  // bool esta = smem.erase_chip();
  pr( "Sector 0 erase..." );
  bool esta = smem.erase_sector( 0 );
  pr_sdx( esta );
  pr( "... done erase" NL );

  uint8_t sta = smem.status();
  pr_sdx( sta );

  uint16_t chip_id = smem.read_id();
  pr_shx( chip_id );


  // uint32_t rv0 = 0;
  // uint16_t nt0 = spi1.send1_recvN_b( 0x15, (uint8_t*)&rv0, 2 ); // ID for AT25F512
  // int wac0 = spi1.get_wait_count();
  // pr_shx( rv0 );
  // pr_sdx( nt0 );
  // pr_sdx( wac0 );
  //
  // rv0 = 0;
  // nt0 = spi1.send1_recvN_b( 0x05, (uint8_t*)&rv0, 1 ); // RDSR
  // pr_shx( rv0 );
  // pr_sdx( nt0 );

  for( int i=0; i<32; ++i ) { gbuf_a[i] = ' '+i; gbuf_b[i] = 0; };
  int nt0 = smem.write( (const uint8_t*)gbuf_a, 7, 0x20 );
  sta = smem.status();
  pr_sdx( sta );
  delay_ms( 100 );
  sta = smem.status();
  pr_sdx( sta );
  pr( "Write: " ); pr_sdx( nt0 ); pr_shx( sta );


  nt0 = smem.read( (uint8_t*)gbuf_b, 5, 0x40 );
  pr( "Read: " ); pr_sdx( nt0 );
  dump8( gbuf_b, 0x40 );


  pr( NL "test0 end." NL );
  return 0;
}

int cmd_spi_info( int argc UNUSED, const char * const * argv UNUSED )
{
  auto s = spi1.getDev();
  pr_sh( "spi= ", (uint32_t)(s) );
  pr_sh( "CR1= ", s->CR1 );
  pr_sh( "CR2= ", s->CR2 );
  pr_sh( "SR = ", s->SR  );
  pr_sh( "DR = ", s->DR  );
  return 0;
}

int cmd_spi_send1( int argc, const char * const * argv )
{
  uint16_t v0 = 0, r0 = 0;
  if( argc > 1 ) {
    v0 = (uint8_t)strtol( argv[1], 0, 0 );
  }
  pr( NL "Send 1 byte to SPI: v0= " ); pr_h( v0 ); pr( NL );
  int nt = spi1.send1_recv1( v0, &r0 );
  pr_sdx( nt );
  pr_shx( r0 );

  return 0;
}

int cmd_spi_send1x( int argc, const char * const * argv )
{
  uint16_t v0 = 0;
  if( argc > 1 ) {
    v0 = (uint8_t)strtol( argv[1], 0, 0 );
  }
  pr( NL "Send 1 byte to SPI: v0= " ); pr_h( v0 ); pr( NL );
  int nt = spi1.send1x( v0 );
  pr_sdx( nt );

  return 0;
}


int cmd_spi_send1rN( int argc, const char * const * argv )
{
  uint8_t v0 = 0;
  int n = 1;
  if( argc > 1 ) {
    v0 = (uint8_t)strtol( argv[1], 0, 0 );
  }
  if( argc > 2 ) {
    n = strtol( argv[2], 0, 0 );
  }
  if( n >= GBUF_SZ ) { n = GBUF_SZ-1; }
  pr( NL "Send 1 byte to SPI: v0= " ); pr_h( v0 ); pr_sdx( n ); pr( NL );

  int nt = spi1.send1_recvN_b( v0, (uint8_t*)(gbuf_b), n );
  pr_sdx( nt );
  dump8( gbuf_b, n );

  return 0;
}

int cmd_spi_duplexN( int argc, const char * const * argv )
{
  int n = 1;
  if( argc > 1 ) {
    n = strtol( argv[1], 0, 0 );
  }
  if( n >= GBUF_SZ ) { n = GBUF_SZ-1; }
  pr( NL "Send/recv N byte via SPI: " ); pr_sdx( n ); pr( NL );

  int nt = spi1.duplexN_b( (const uint8_t*)(gbuf_a), n, (uint8_t*)(gbuf_b) );
  pr_sdx( nt );
  dump8( gbuf_b, n );

  return 0;

}

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc,ox/inc/usb_cdc
