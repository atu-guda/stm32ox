#include "stm32f10x_conf.h"
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <stdlib.h>
#include <errno.h>
#include "localfun.h"

// general buffers
char gbuf_a[GBUF_SZ];
char gbuf_b[GBUF_SZ]; // and log too

int user_vars[N_USER_VARS] = { 0x55555555, 0, 0, 0 };

void taskYieldFun()
{
  taskYIELD();
}

// int outpins_init( OutPins *oup )
// {
//   if( oup->n < 1  || oup->n + oup->start > PORT_BITS ) {
//     return 0;
//   }
//
//   oup->mask = ((uint16_t)(0xFFFF) << (PORT_BITS - oup->n));
//   oup->mask >>= (PORT_BITS - oup->n - oup->start);
//
//   GPIO_InitTypeDef  gpio;
//   // RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE ); // TODO: common
//   gpio.GPIO_Pin   = oup->mask;
//   gpio.GPIO_Mode  = GPIO_Mode_Out_PP;
//   gpio.GPIO_Speed = GPIO_Speed_50MHz;
//   // gpio_d.GPIO_PuPd  = GPIO_PuPd_NOPULL; // F4?
//   GPIO_Init( oup->GPIOx, &gpio );
//   return 1;
// }
//
// void outpins_set( const OutPins *oup, uint16_t v )
// {
//   v <<= oup->start;
//   oup->GPIOx->ODR = ( v & oup->mask ) | ( oup->GPIOx->ODR & (~oup->mask) );
// }
//
// void outpins_on( const OutPins *oup, uint16_t v )
// {
//   v <<= oup->start;
//   oup->GPIOx->ODR |= ( v & oup->mask );
// }
//
// void outpins_off( const OutPins *oup, uint16_t v )
// {
//   v <<= oup->start;
//   oup->GPIOx->ODR &= ~( v & oup->mask );
// }
//
// void outpins_toggle( const OutPins *oup, uint16_t v )
// {
//   v <<= oup->start;
//   oup->GPIOx->ODR ^= ( v & oup->mask );
// }

// OutPins *LEDS = 0;
//
// int  leds_init()
// {
//   if( !LEDS ) {
//     return 0;
//   }
//   return outpins_init( LEDS );
// }
//
// void leds_set( uint16_t v )
// {
//   if( !LEDS ) {
//     return;
//   }
//   outpins_set( LEDS, v );
// }
//
// void leds_on(  uint16_t v )
// {
//   if( !LEDS ) {
//     return;
//   }
//   outpins_on( LEDS, v );
// }
//
// void leds_off( uint16_t v )
// {
//   if( !LEDS ) {
//     return;
//   }
//   outpins_off( LEDS, v );
// }
//
// void leds_toggle( uint16_t v )
// {
//   if( !LEDS ) {
//     return;
//   }
//   outpins_toggle( LEDS, v );
// }

void die4led( uint16_t n )
{
  // leds_set( n );
  while(1) {
    delay_bad_ms( 200 );
    // leds_toggle( BIT0 );
  }
}

void GPIO_WriteBits( GPIO_TypeDef* GPIOx, uint16_t PortVal, uint16_t mask )
{
  GPIOx->ODR = ( PortVal & mask ) | ( GPIOx->ODR & (~mask) );
}


void delay_ms( uint32_t ms )
{
  vTaskDelay( ms / portTICK_RATE_MS );
}


void delay_bad_n( uint32_t dly )
{
  for( uint32_t i=0; i<dly; ++i ) {
    __asm volatile ( "nop;");
  }
}

void delay_bad_s( uint32_t s )
{
  uint32_t n = s * T_S_MUL;
  for( uint32_t i=0; i<n; ++i ) {
    __asm volatile ( "nop;");
  }
}

void delay_bad_ms( uint32_t ms )
{
  uint32_t n = ms * T_MS_MUL;
  /*__IO*/
  // for(  uint32_t i = n; i>0; --i ) {
  for(  uint32_t i = 0; i<n; ++i ) {
    __asm volatile ( "nop;");
  }
}

void delay_mcs( uint32_t mcs )
{
  int ms = mcs / 1000;
  int mcs_r = mcs % 1000;
  if( ms )
    delay_ms( ms );
  if( mcs_r )
    delay_bad_mcs( mcs_r );
}



// void BOARD_initBtn(void)
// {
//   /*
//   GPIO_InitTypeDef  gpio_a;
//   RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE );
//   gpio_a.GPIO_Pin   = GPIO_Pin_0;
//   gpio_a.GPIO_Mode  = GPIO_Mode_IN;
//   gpio_a.GPIO_OType = GPIO_OType_PP;
//   gpio_a.GPIO_Speed = GPIO_Speed_25MHz;
//   gpio_a.GPIO_PuPd  = GPIO_PuPd_DOWN;
//   GPIO_Init( GPIOA, &gpio_a );
//   */
// }

// uint8_t BOARD_btnState(void)
// {
//  return GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_0 );
// }

const char hex_digits[] = "0123456789ABCDEFG";
const char dec_digits[] = "0123456789???";


char* str2addr( const char *str )
{
  if( str[0] == 'a' && str[1] == '\0' ) {
    return gbuf_a;
  } else if ( str[0] == 'b' && str[1] == '\0' ) {
    return gbuf_b;
  }
  char *eptr;
  char *addr = (char*)( strtol( str, &eptr, 0 ) );
  if( *eptr == '\0' ) {
    return addr;
  }
  return (char*)(BAD_ADDR);
}

char* char2hex( char c, char *s )
{
  if( s != 0 ) {
    s[0] = hex_digits[ (uint8_t)(c) >> 4 ];
    s[1] = hex_digits[ c & 0x0F ];
    s[2] = 0;
  }
  return s;
}

char* word2hex( uint32_t d,  char *s )
{
  if( s != 0 ) {
    int i;
    for( i=7; i>=0; --i ) {
      s[i] = hex_digits[ d & 0x0F ];
      d >>= 4;
    }
    s[8] = 0;
  }
  return s;
}

char* i2dec( int n, char *s )
{
  static char sbuf[INT_STR_SZ];
  char tbuf[24];
  unsigned u;
  if( !s ) {
    s = sbuf;
  }
  char *bufptr = s, *tmpptr = tbuf + 1;
  *tbuf = '\0';

  if( n < 0 ){ //  sign
    u = ( (unsigned)(-(1+n)) ) + 1; // INT_MIN handling
    *bufptr++ = '-';
  } else {
    u=n;
  }

  do {
    *tmpptr++ = dec_digits[ u % 10 ];
  } while( u /= 10 );

  while( ( *bufptr++ = *--tmpptr ) != '\0' ) /* NOP */;
  return s;
}

void dump8( const void *addr, int n )
{
  char b[8]; // for char2hex
  const char* ad = (const char*)(addr);
  if( !ad  ||  ad == BAD_ADDR ) {
    return;
  }
  const char* ad0 = ad;
  pr( NL );

  int i;
  for( i=0; i<n; ++i ) {
    if( ( i & 0x0F ) == 0 ) {
      pr_a( ad0 ); pr( ": " );
    }
    pr( char2hex( ad[i], b ) ); pr( " " );
    if( (i & 0x0F) == 0x0F ) {
      pr( NL );
      ad0 += 16;
    }
  }
  pr( NL "-------------------------" NL );
}

int pr_d( int d )
{
  char b[32];
  return pr( i2dec( d, b ) );
}

int pr_h( uint32_t d )
{
  char b[12];
  return pr( word2hex( d, b ) );
}

int pr_sd( const char *s, int d )
{
  pr( s );
  pr_d( d );
  return pr( NL );
}

int pr_sh( const char *s, int d )
{
  pr( s );
  pr_h( d );
  return pr( NL );
}

int log_buf_idx = 0;

// TODO: mutex
void log_add( const char *s )
{
  if( !s ) {
    return;
  }

  while( *s !=0  && log_buf_idx < GBUF_SZ-1 ) {
    gbuf_b[log_buf_idx++] = *s++;
  }
  gbuf_b[log_buf_idx] = '\0'; // not++
}

void log_reset()
{
  log_buf_idx = 0;
  gbuf_b[0] = '\0';
}

void log_print()
{
  if( log_buf_idx > 0 ) {
    pr( gbuf_b );
    pr( NL );
    pr_sd( "log_buf_idx", log_buf_idx );
    pr( NL );
    delay_ms( 100 );
  }
}

void print_user_var( int idx )
{
  if( idx < 0  ||  idx >= N_USER_VARS ) {
    pr_sd( NL "err: bad var index: ", idx );
    return;
  }
  char b[4] = "0= ";
  b[0] = (char)('a' + idx);
  pr( b ); pr_d( user_vars[idx] ); pr( " = " );
  pr_h( user_vars[idx] ); pr( NL );
}

// common commands
//
int cmd_info( int argc, const char * const * argv )
{
  RCC_ClocksTypeDef clst;
  RCC_GetClocksFreq( &clst );
  pr( "SYSCLK: " );  pr_d( clst.SYSCLK_Frequency );
  pr( "  HCLK: " );  pr_d( clst.HCLK_Frequency );
  pr( "  PCLK1: " ); pr_d( clst.PCLK1_Frequency );
  pr( "  PCLK2: " ); pr_d( clst.PCLK2_Frequency );
  pr( "  HSE_VALUE: " ); pr_d( HSE_VALUE );
  pr( NL );
  pr( "RCC_APB2ENR: " ); pr_h( RCC->APB2ENR );
  pr( "   RCC_APB1ENR: " ); pr_h( RCC->APB1ENR );
  pr( "   RCC_AHBENR: "  ); pr_h( RCC->AHBENR );
  pr( NL );
  pr_sdx( errno );
  pr_sdx( SystemCoreClock );
  int tick_count = xTaskGetTickCount();
  pr_sdx( tick_count );
  return 0;
}

int cmd_echo( int argc, const char * const * argv )
{
  pr( NL ); pr_sdx( argc ); pr( NL );
  int i;
  for( i=0; i<argc; ++i ) {
    pr( "  arg" ); pr_d( i ); pr( " = \"" ) ; pr( argv[i] ); pr( "\"" NL );
  }
  return 0;
}

int cmd_help( int argc, const char * const * argv )
{
  pr( "commands:" NL );
  char b1[2]; b1[0] = b1[1] = 0;
  for( int i=0; i<CMDS_NMAX; ++i ) {
    if( global_cmds[i].name == 0 ) {
      break;
    }
    pr( global_cmds[i].name ); pr( " " );
    pr( global_cmds[i].hint ); pr( " " );
    if( global_cmds[i].acr != 0 ) {
      pr( " (" ); b1[0] = global_cmds[i].acr; pr( b1 ); pr( ")" );
    }
    pr( NL );
  }
  return 0;
}

int cmd_dump( int argc, const char * const * argv )
{
  if( argc < 2 ) {
    return 1;
  }

  const char* addr = str2addr( argv[1] );
  if( addr == BAD_ADDR ) {
    pr( "** error: dump: bad address \"" );  pr( argv[1] );  pr( "\"" NL );
    return 2;
  }

  int n = 1;
  if( argc >= 3 ) {
    n = strtol( argv[2], 0, 0 );
  }

  pr( NL "** dump: argc=" ); pr_d( argc ); pr( " addr=" ); pr_a( addr );
  pr_sdx( n );
  dump8( addr, n );
  return 0;
}

int cmd_fill( int argc, const char * const * argv )
{
  if( argc < 2 ) {
    return 1;
  }

  char* addr = str2addr( argv[1] );
  if( addr == BAD_ADDR ) {
    pr( "** error: fill: bad address \"" );  pr( argv[1] );  pr( "\"" NL );
    return 2;
  }

  char v = 0;
  if( argc >= 3 ) {
    if( argv[2][0] == '\'' ) {
      v = argv[2][1];
    } else {
      v = ( char ) ( strtol( argv[2], 0, 0 ) );
    }
  }

  int n = 1;
  if( argc >= 4 ) {
    n = strtol( argv[3], 0, 0 );
  }
  pr( "** fill: addr=" ); pr_a( addr );
  pr_sdx( v );
  pr_sdx( n );

  for( int i=0; i<n; ++i, ++addr ) {
    *addr = v;
  }
  pr( NL "---------- done---------------" NL );
  return 0;
}

int cmd_pvar( int argc, const char * const * argv )
{
  uint8_t start = 0, end = N_USER_VARS;
  if( argc > 1 ) {
    start = argv[1][0]-'a'; end = argv[1][1]-'a';
  }
  if( end <= start ) {
    end = start+1;
  }

  for( int i=start; i<end; ++i ) {
    print_user_var( i );
  }
  return 0;
}

int cmd_svar( int argc, const char * const * argv )
{
  if( argc < 3 ) {
    return 1;
  }
  int idx = argv[1][0] - 'a';
  if( idx < 0 || idx >= N_USER_VARS ) {
    return 2;
  }
  user_vars[idx] = strtol( argv[2], 0, 0 );
  return 0;
}


int cmd_die( int argc, const char * const * argv )
{
  int v = 0;
  if( argc > 1 ) {
    v = strtol( argv[1], 0, 0 );
  }
  die4led( v );
  return 0; // never ;-)
}

int cmd_reboot( int argc, const char * const * argv )
{
  NVIC_SystemReset();
  return 0; // never ;-)
}

